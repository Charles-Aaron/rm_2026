#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <rcl_interfaces/srv/set_parameters.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>
#include <sentry_nav_interfaces/action/navigate_route.hpp>
#include <sentry_nav_interfaces/action/rotate_to_yaw.hpp>
#include <sentry_nav_interfaces/srv/append_route_point.hpp>
#include <sentry_nav_interfaces/srv/prepare_route.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <optional>
#include <vector>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;
using Point = sentry_nav_interfaces::msg::RoutePoint;
using Route = sentry_nav_interfaces::action::NavigateRoute;
using Rotation = sentry_nav_interfaces::action::RotateToYaw;
using Single = nav2_msgs::action::NavigateToPose;
using Multi = nav2_msgs::action::NavigateThroughPoses;
using RouteHandle = rclcpp_action::ServerGoalHandle<Route>;

static bool valid_route(const std::vector<Point> & points) {
  if (points.empty() || points.size() > 128) return false;
  bool end = false;
  for (const auto & p : points) {
    const auto & v = p.pose.pose.position; const auto & q = p.pose.pose.orientation;
    if (p.pose.header.frame_id != "map" || !std::isfinite(v.x) || !std::isfinite(v.y) ||
      !std::isfinite(v.z) || !std::isfinite(q.x) || !std::isfinite(q.y) ||
      !std::isfinite(q.z) || !std::isfinite(q.w) ||
      q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w < 1e-12) return false;
    if (end) {if (p.kind != Point::SPECIAL_END) return false; end = false;}
    else if (p.kind == Point::SPECIAL_BEGIN) end = true;
    else if (p.kind != Point::NORMAL) return false;
  }
  return !end;
}

class RouteManager : public rclcpp::Node {
  using Bool = std_msgs::msg::Bool;
  using Append = sentry_nav_interfaces::srv::AppendRoutePoint;
  using Prepare = sentry_nav_interfaces::srv::PrepareRoute;
  using SetBool = std_srvs::srv::SetBool;
  using Params = rcl_interfaces::srv::SetParameters;
  enum class Phase {Idle, Navigating, Entering, Aligning, Leaving, Canceling};
  enum class SpinPhase {Idle, Configuring, Releasing};
  struct Child {
    bool pending = true, done = false, success = false, abandoned = false, cancel_sent = false;
    size_t progress = 0;
    rclcpp_action::GoalUUID id{};
    std::string reason;
    std::function<void()> cancel;
    Clock::time_point deadline;
  };
  rclcpp_action::Client<Single>::SharedPtr single_;
  rclcpp_action::Client<Multi>::SharedPtr multi_;
  rclcpp_action::Client<Rotation>::SharedPtr rotate_;
  rclcpp_action::Server<Route>::SharedPtr server_;
  rclcpp::Client<Params>::SharedPtr speed_client_;
  rclcpp::Client<Prepare>::SharedPtr prepare_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cancel_client_;
  rclcpp::Service<Append>::SharedPtr append_;
  rclcpp::Service<SetBool>::SharedPtr spin_service_;
  rclcpp::Publisher<Bool>::SharedPtr spin_pub_, spin_state_pub_, override_pub_;
  rclcpp::Subscription<Bool>::SharedPtr override_sub_, permitted_sub_;
  rclcpp::TimerBase::SharedPtr timer_, event_timer_;
  tf2_ros::Buffer tf_;
  tf2_ros::TransformListener listener_;
  std::shared_ptr<RouteHandle> route_;
  std::shared_ptr<Child> nav_, turn_;
  std::vector<Point> points_, chunk_;
  size_t index_ = 0, chunk_end_ = 0, end_index_ = 0;
  Phase phase_ = Phase::Idle;
  SpinPhase spin_phase_ = SpinPhase::Idle;
  bool segment_ = false, requested_ = false, spin_enabled_ = false, end_processed_ = false;
  bool permitted_ = false, cancel_fallback_sent_ = false;
  std::optional<bool> override_state_;
  Clock::time_point override_at_{}, permitted_at_{}, phase_at_{}, dispatch_until_{};
  std::string failure_;
  std::optional<rclcpp_action::GoalUUID> prepare_owner_;
  std::optional<int64_t> prepare_request_, speed_request_, cancel_request_;
  Clock::time_point prepare_until_{}, spin_until_{};
  std::shared_ptr<rmw_request_id_t> spin_header_;
  SetBool::Response spin_response_;

  // Humble 的 Action 回调持有客户端内部锁，取消/发送操作需退出回调后执行。
  void wake() {event_timer_->reset();}

  bool competing() {return count_publishers("/navigation_done") > 1;}
  bool permitted() const {return permitted_ && Clock::now() - permitted_at_ < 500ms;}
  bool override_fresh() const {return override_state_ && Clock::now() - override_at_ < 500ms;}
  bool confirmed(bool value, Clock::time_point after = {}) const {
    return override_fresh() && *override_state_ == value && override_at_ >= after;
  }
  void publish_controls() {
    Bool m;
    if (!competing()) {m.data = spin_enabled_ && !segment_; spin_pub_->publish(m);}
    m.data = spin_enabled_; spin_state_pub_->publish(m);
    if (segment_) {m.data = requested_; override_pub_->publish(m);}
  }
  bool inside(const Point & p) {
    try {
      const auto t = tf_.lookupTransform("map", "chassis_link", tf2::TimePointZero);
      const double age = (now() - rclcpp::Time(t.header.stamp, get_clock()->get_clock_type())).seconds();
      return age >= -0.05 && age <= 0.2 &&
        std::hypot(t.transform.translation.x-p.pose.pose.position.x,
                   t.transform.translation.y-p.pose.pose.position.y) <= get_parameter("special_point_radius").as_double();
    } catch (const std::exception &) {return false;}
  }
  static bool settled(const std::shared_ptr<Child> & child) {return !child || child->done;}
  static void cancel(const std::shared_ptr<Child> & child) {
    if (!child || child->done || child->abandoned) return;
    child->abandoned = true;
  }
  void preparation(bool clear) {
    if (!prepare_owner_ || !prepare_client_->service_is_ready()) return;
    auto request = std::make_shared<Prepare::Request>(); request->predecessor_id = *prepare_owner_;
    if (!clear) {
      size_t end = chunk_end_ + 1;
      if (chunk_end_ >= points_.size()) return;
      if (route_->get_goal()->continuous) {
        while (end < points_.size() && points_[end-1].kind != Point::SPECIAL_BEGIN) ++end;
      }
      for (size_t i = chunk_end_; i < end; ++i) request->goals.push_back(points_[i].pose);
    }
    if (prepare_request_) prepare_client_->remove_pending_request(*prepare_request_);
    auto future = prepare_client_->async_send_request(request,
      [this](rclcpp::Client<Prepare>::SharedFuture response) {
        const auto value = response.get();
        if (!value->success) RCLCPP_WARN(get_logger(), "Preparation skipped: %s", value->reason.c_str());
        prepare_request_.reset();
      });
    prepare_request_ = future.request_id; prepare_until_ = Clock::now()+1s;
  }
  void fail(const std::string & reason) {
    if (phase_ == Phase::Canceling) return;
    failure_ = reason; phase_ = Phase::Canceling; phase_at_ = Clock::now();
    if (segment_) {spin_enabled_ = false; requested_ = true;}
    preparation(true); cancel(nav_); cancel(turn_); publish_controls(); wake();
  }
  void finish_route(bool success) {
    auto result = std::make_shared<Route::Result>(); result->success = success;
    result->reason = success ? "completed" : failure_;
    if (success) route_->succeed(result);
    else {
      RCLCPP_WARN(get_logger(), "Route ended: reason=%s point_index=%zu segment_active=%d",
        failure_.c_str(), index_, static_cast<int>(segment_));
      if (route_->is_canceling()) route_->canceled(result); else route_->abort(result);
    }
    route_.reset(); points_.clear(); chunk_.clear(); prepare_owner_.reset(); phase_ = Phase::Idle;
    if (cancel_request_) {cancel_client_->remove_pending_request(*cancel_request_); cancel_request_.reset();}
  }
  template<class Action> std::shared_ptr<Child> send(
      typename rclcpp_action::Client<Action>::SharedPtr client, const typename Action::Goal & request) {
    auto child = std::make_shared<Child>(); child->deadline = Clock::now()+3s;
    typename rclcpp_action::Client<Action>::SendGoalOptions options;
    options.goal_response_callback = [this,child,client](auto handle) {
      child->pending = false;
      if (!handle) {child->done = true; child->reason = "request_rejected";}
      else {
        child->id = handle->get_goal_id();
        child->cancel = [client,handle]() {client->async_cancel_goal(handle);};
      }
      wake();
    };
    options.result_callback = [this,child](const auto & result) {
      child->cancel = {};
      child->done = true; child->success = result.code == rclcpp_action::ResultCode::SUCCEEDED;
      if constexpr (std::is_same_v<Action, Rotation>) {
        child->success = child->success && result.result && result.result->success;
        child->reason = result.result ? result.result->reason : "missing_rotation_result";
      }
      wake();
    };
    if constexpr (std::is_same_v<Action, Multi>) {
      const auto count = request.poses.size();
      options.feedback_callback = [this,child,count](auto, const std::shared_ptr<const Multi::Feedback> feedback) {
        auto remaining = std::max<int64_t>(0, feedback->number_of_poses_remaining);
        child->progress = remaining >= static_cast<int64_t>(count) ? 0 :
          std::min(count-1, count-static_cast<size_t>(remaining));
        wake();
      };
    }
    client->async_send_goal(request, options); return child;
  }
  void start_chunk() {
    if (index_ >= points_.size()) {
      if (segment_) fail("unfinished_special_segment"); else finish_route(true);
      return;
    }
    chunk_end_ = index_+1;
    if (route_->get_goal()->continuous) {
      while (chunk_end_ < points_.size() && points_[chunk_end_-1].kind != Point::SPECIAL_BEGIN) ++chunk_end_;
    }
    chunk_.assign(points_.begin()+index_, points_.begin()+chunk_end_);
    end_index_ = std::find_if(chunk_.begin(),chunk_.end(),[](const auto & p) {
      return p.kind == Point::SPECIAL_END;
    }) - chunk_.begin();
    end_processed_ = false; nav_.reset(); turn_.reset();
    phase_ = Phase::Navigating; dispatch_until_ = Clock::now()+3s; wake();
  }
  void launch_navigation() {
    if (route_->get_goal()->continuous && chunk_.size() > 1) {
      if (!multi_->action_server_is_ready()) return;
      Multi::Goal request; for (const auto & p : chunk_) request.poses.push_back(p.pose);
      nav_ = send<Multi>(multi_, request);
    } else {
      if (!single_->action_server_is_ready()) return;
      Single::Goal request; request.pose = chunk_.front().pose; nav_ = send<Single>(single_, request);
    }
  }
  void launch_rotation() {
    const double axis = get_parameter("special_axis_yaw").as_double();
    if (!std::isfinite(axis)) {fail("invalid_special_axis"); return;}
    if (!rotate_->action_server_is_ready()) {
      if (Clock::now() >= dispatch_until_) fail("rotation_unavailable");
      return;
    }
    try {
      const auto t = tf_.lookupTransform("map", "chassis_link", tf2::TimePointZero);
      const double age = (now()-rclcpp::Time(t.header.stamp, get_clock()->get_clock_type())).seconds();
      if (age < -0.05 || age > 0.2) {fail("stale_special_heading"); return;}
      auto q = t.transform.rotation;
      const double norm = std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
      if (!std::isfinite(norm) || norm < 1e-12) {fail("invalid_special_heading"); return;}
      q.x/=norm; q.y/=norm; q.z/=norm; q.w/=norm;
      const double current = std::atan2(2*(q.w*q.z+q.x*q.y), 1-2*(q.y*q.y+q.z*q.z));
      const double a = std::atan2(std::sin(axis), std::cos(axis));
      const double b = std::atan2(std::sin(axis+M_PI), std::cos(axis+M_PI));
      auto diff = [current](double x) {return std::abs(std::atan2(std::sin(x-current),std::cos(x-current)));};
      Rotation::Goal request; request.target_yaw = diff(a) <= diff(b) ? a : b;
      request.tolerance = get_parameter("special_yaw_tolerance").as_double();
      request.timeout = get_parameter("special_rotation_timeout").as_double(); request.allow_translation = true;
      turn_ = send<Rotation>(rotate_, request);
      dispatch_until_ = Clock::now()+std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(request.timeout+3.0));
    } catch (const std::exception &) {fail("special_heading_unavailable");}
  }
  void step() {
    if (phase_ == Phase::Canceling) {
      if (settled(nav_) && settled(turn_)) finish_route(false);
      else if (Clock::now()-phase_at_ >= 5s) {failure_ += ":child_cancel_unconfirmed"; finish_route(false);}
      else if (Clock::now()-phase_at_ >= 3s && !settled(nav_) && !cancel_fallback_sent_ &&
        cancel_client_->service_is_ready()) {
        // 保留原有取消服务兜底，仍等子任务结果确认，不阻塞事件循环。
        cancel_fallback_sent_ = true;
        auto future = cancel_client_->async_send_request(std::make_shared<std_srvs::srv::Trigger::Request>(),
          [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture) {cancel_request_.reset();});
        cancel_request_ = future.request_id;
      }
      return;
    }
    if (route_->is_canceling()) {fail("canceled"); return;}
    if (segment_) {
      if (!permitted()) {fail("segment_override_lost"); return;}
      if (competing()) {fail("another_spin_publisher_active"); return;}
      if (phase_ != Phase::Entering && phase_ != Phase::Leaving && !confirmed(true)) {
        fail("segment_override_lost"); return;
      }
    }
    if (nav_ && nav_->pending && Clock::now() >= nav_->deadline) {fail("navigation_request_timeout"); return;}
    if (nav_ && nav_->done && !nav_->success) {fail("navigation_failed"); return;}
    if (phase_ == Phase::Entering) {
      if (confirmed(true, phase_at_)) {
        phase_ = Phase::Aligning; dispatch_until_ = Clock::now()+3s;
        prepare_owner_ = nav_->id; preparation(false); launch_rotation();
      } else if (Clock::now()-phase_at_ >= 2s) fail("segment_override_unconfirmed");
      return;
    }
    if (phase_ == Phase::Leaving) {
      if (confirmed(false, phase_at_)) {
        segment_ = false; end_processed_ = true; phase_ = Phase::Navigating; wake();
      }
      else if (Clock::now()-phase_at_ >= 2s) fail("segment_release_unconfirmed");
      return;
    }
    if (phase_ == Phase::Aligning) {
      if (!turn_) {launch_rotation(); return;}
      if (turn_->pending && Clock::now() >= turn_->deadline) {fail("rotation_request_timeout"); return;}
      if (turn_->done && !turn_->success) {
        fail("rotation_failed" + (turn_->reason.empty() ? std::string{} : ":"+turn_->reason)); return;
      }
      if (Clock::now() >= dispatch_until_ && !turn_->done) {fail("rotation_timeout"); return;}
      // 导航成功与转向成功缺一不可；预规划就绪不构成运动许可。
      if (nav_->done && turn_->done) {index_ = chunk_end_; start_chunk();}
      return;
    }
    if (!nav_) {
      launch_navigation();
      if (!nav_ && Clock::now() >= dispatch_until_) fail("server_unavailable");
      return;
    }
    if (nav_->pending) return;
    if (segment_ && !end_processed_ && end_index_ < chunk_.size() &&
      (end_index_ <= nav_->progress || nav_->done) && inside(chunk_[end_index_])) {
      requested_ = false; phase_ = Phase::Leaving; phase_at_ = Clock::now(); publish_controls(); return;
    }
    const bool begin = chunk_.back().kind == Point::SPECIAL_BEGIN;
    if (begin && (nav_->progress == chunk_.size()-1 || nav_->done) && inside(chunk_.back())) {
      if (segment_) {fail("special_end_not_reached"); return;}
      segment_ = true; requested_ = true; phase_ = Phase::Entering; phase_at_ = Clock::now();
      publish_controls(); return;
    }
    if (nav_->done) {
      if (begin) {fail("special_begin_not_reached"); return;}
      if (segment_ && !end_processed_ && end_index_ < chunk_.size()) {fail("special_end_not_reached"); return;}
      index_ = chunk_end_; start_chunk();
    }
  }
  void advance() {
    for (const auto & child : {nav_, turn_}) {
      if (child && child->abandoned && !child->done && !child->cancel_sent && child->cancel) {
        child->cancel_sent = true; child->cancel();
      }
    }
    if (!route_) return;
    try {step();} catch (const std::exception & e) {fail(std::string("route_exception:")+e.what());}
  }
  std::string phase_name() const {
    switch (phase_) {
      case Phase::Entering: return "entering_special_segment";
      case Phase::Aligning: return nav_ && nav_->done ? "waiting_for_rotation" : "navigating_and_rotating";
      case Phase::Leaving: return "leaving_special_segment";
      case Phase::Canceling: return "canceling";
      default: return "navigating";
    }
  }
  void reply_spin(bool success, const std::string & reason) {
    spin_response_.success = success; spin_response_.message = reason;
    spin_service_->send_response(*spin_header_, spin_response_); spin_header_.reset(); spin_phase_ = SpinPhase::Idle;
    if (speed_request_) {speed_client_->remove_pending_request(*speed_request_); speed_request_.reset();}
  }
  void speed_confirmed(bool success) {
    if (!success) {reply_spin(false,"spin_speed_not_confirmed"); return;}
    if (!permitted() || competing()) {reply_spin(false,"chassis_not_in_navigation_mode"); return;}
    if (segment_ && route_) {reply_spin(false,"special_segment_active"); return;}
    if (segment_) {
      requested_ = false; phase_at_ = Clock::now(); spin_until_ = phase_at_+2s;
      spin_phase_ = SpinPhase::Releasing; publish_controls();
    } else {spin_enabled_ = true; reply_spin(true,"spin_enabled");}
  }
  void set_spin(std::shared_ptr<rmw_request_id_t> header, const SetBool::Request::SharedPtr request) {
    auto deny = [&](const std::string & reason) {
      SetBool::Response response; response.message = reason; spin_service_->send_response(*header,response);
    };
    if (spin_header_) {deny("spin_update_pending"); return;}
    if (competing()) {deny("another_spin_publisher_active"); return;}
    if (!override_fresh()) {deny("chassis_override_state_unavailable"); return;}
    if (confirmed(true) && !segment_) {segment_ = true; requested_ = true;}
    if (segment_ && route_) {deny("special_segment_active"); return;}
    if (request->data && (!settled(nav_) || !settled(turn_))) {deny("child_cancel_unconfirmed"); return;}
    spin_header_ = header;
    if (!request->data) {
      spin_enabled_ = false; reply_spin(true,segment_ ? "spin_disabled_override_held" : "spin_disabled"); return;
    }
    if (!permitted()) {reply_spin(false,"chassis_not_in_navigation_mode"); return;}
    const double speed = get_parameter("spin_speed").as_double();
    if (!std::isfinite(speed) || std::abs(speed) > 6.0 || !speed_client_->service_is_ready()) {
      reply_spin(false,"spin_speed_service_unavailable_or_invalid"); return;
    }
    auto req = std::make_shared<Params::Request>();
    req->parameters.push_back(rclcpp::Parameter("navigation_rotation_speed",speed).to_parameter_msg());
    spin_phase_ = SpinPhase::Configuring; spin_until_ = Clock::now()+2s;
    auto future = speed_client_->async_send_request(req,[this](rclcpp::Client<Params>::SharedFuture response) {
      if (!spin_header_ || spin_phase_ != SpinPhase::Configuring) return;
      const auto value = response.get(); speed_request_.reset();
      speed_confirmed(value && !value->results.empty() && value->results.front().successful);
    });
    speed_request_ = future.request_id;
  }
  void tick() {
    if (prepare_request_ && Clock::now() >= prepare_until_) {
      prepare_client_->remove_pending_request(*prepare_request_); prepare_request_.reset();
    }
    if (spin_header_) {
      if (spin_phase_ == SpinPhase::Releasing && confirmed(false,phase_at_)) {
        segment_ = false; spin_enabled_ = true; reply_spin(true,"spin_enabled");
      } else if (Clock::now() >= spin_until_ || !permitted()) {
        if (segment_) requested_ = true;
        reply_spin(false,spin_phase_ == SpinPhase::Releasing ? "segment_release_unconfirmed" : "spin_speed_not_confirmed");
      }
    }
    advance(); publish_controls();
    if (route_) {
      auto feedback = std::make_shared<Route::Feedback>();
      feedback->current_index = index_; feedback->total_points = points_.size(); feedback->phase = phase_name();
      route_->publish_feedback(feedback);
    }
  }
public:
  RouteManager() : Node("sentry_route_manager"), tf_(get_clock()), listener_(tf_) {
    declare_parameter("spin_speed",2.0); declare_parameter("special_point_radius",0.5);
    declare_parameter("special_axis_yaw",0.0); declare_parameter("special_yaw_tolerance",0.1);
    declare_parameter("special_rotation_timeout",6.0);
    single_ = rclcpp_action::create_client<Single>(this,"navigate_to_pose");
    multi_ = rclcpp_action::create_client<Multi>(this,"navigate_through_poses");
    rotate_ = rclcpp_action::create_client<Rotation>(this,"rotate_to_yaw");
    speed_client_ = create_client<Params>("/chassis_sentry_controller/set_parameters");
    prepare_client_ = create_client<Prepare>("prepare_route");
    cancel_client_ = create_client<std_srvs::srv::Trigger>("cancel_navigation");
    spin_pub_ = create_publisher<Bool>("/navigation_done",rclcpp::SensorDataQoS());
    spin_state_pub_ = create_publisher<Bool>("navigation_spin_requested",rclcpp::QoS(1).transient_local());
    override_pub_ = create_publisher<Bool>("/navigation_special_segment",1);
    override_sub_ = create_subscription<Bool>("/navigation_special_segment_state",1,[this](Bool::ConstSharedPtr m) {
      override_state_ = m->data; override_at_ = Clock::now(); wake();
    });
    permitted_sub_ = create_subscription<Bool>("/navigation_spin_permitted",1,[this](Bool::ConstSharedPtr m) {
      permitted_ = m->data; permitted_at_ = Clock::now(); wake();
    });
    append_ = create_service<Append>("append_route_points",[this](const Append::Request::SharedPtr req, Append::Response::SharedPtr res) {
      if (!route_ || phase_ == Phase::Canceling) {res->reason = "no_active_route"; return;}
      if (!valid_route(req->points) || points_.size()+req->points.size() > 128) {res->reason = "invalid_or_unpaired_points"; return;}
      if (std::any_of(req->points.begin(),req->points.end(),[](const auto & p) {return p.kind == Point::SPECIAL_BEGIN;}) && competing()) {
        res->reason = "another_spin_publisher_active"; return;
      }
      points_.insert(points_.end(),req->points.begin(),req->points.end()); res->success = true; res->reason = "appended";
    });
    spin_service_ = create_service<SetBool>("set_navigation_spin",
      [this](std::shared_ptr<rmw_request_id_t> header, SetBool::Request::SharedPtr req) {set_spin(header,req);});
    server_ = rclcpp_action::create_server<Route>(this,"navigate_route",
      [this](const auto &, std::shared_ptr<const Route::Goal> goal) {
        if (!valid_route(goal->points) || route_ || segment_ || override_state_.value_or(false) ||
          spin_header_ || !settled(nav_) || !settled(turn_)) return rclcpp_action::GoalResponse::REJECT;
        const bool special = std::any_of(goal->points.begin(),goal->points.end(),[](const auto & p) {
          return p.kind == Point::SPECIAL_BEGIN;
        });
        if (special && (!permitted() || competing() || !override_fresh())) return rclcpp_action::GoalResponse::REJECT;
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
      },[](auto) {return rclcpp_action::CancelResponse::ACCEPT;},
      [this](std::shared_ptr<RouteHandle> h) {
        route_ = h; points_ = h->get_goal()->points; index_ = 0; failure_.clear();
        cancel_fallback_sent_ = false; start_chunk(); advance();
      });
    // 所有业务回调串行推进状态，位置/超时检查维持 40 Hz，无阻塞等待。
    event_timer_ = create_wall_timer(1ns,[this]() {event_timer_->cancel(); advance();});
    event_timer_->cancel();
    timer_ = create_wall_timer(25ms,[this]() {tick();});
  }
};

int main(int argc, char ** argv) {
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<RouteManager>());
  rclcpp::shutdown(); return 0;
}
