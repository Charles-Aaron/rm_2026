#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <rcl_interfaces/srv/set_parameters.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>
#include <sentry_nav_interfaces/action/navigate_route.hpp>
#include <sentry_nav_interfaces/action/rotate_to_yaw.hpp>
#include <sentry_nav_interfaces/msg/route_point.hpp>
#include <sentry_nav_interfaces/srv/append_route_point.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <future>
#include <mutex>
#include <optional>
#include <thread>
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
  using SetBool = std_srvs::srv::SetBool;
  using Params = rcl_interfaces::srv::SetParameters;
  rclcpp::CallbackGroup::SharedPtr group_;
  rclcpp_action::Client<Single>::SharedPtr single_;
  rclcpp_action::Client<Multi>::SharedPtr multi_;
  rclcpp_action::Client<Rotation>::SharedPtr rotate_;
  rclcpp_action::Server<Route>::SharedPtr server_;
  rclcpp::Client<Params>::SharedPtr speed_client_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cancel_client_;
  rclcpp::Service<Append>::SharedPtr append_;
  rclcpp::Service<SetBool>::SharedPtr spin_service_;
  rclcpp::Publisher<Bool>::SharedPtr spin_pub_, spin_state_pub_, override_pub_;
  rclcpp::Subscription<Bool>::SharedPtr override_sub_, permitted_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  tf2_ros::Buffer tf_;
  tf2_ros::TransformListener listener_;
  std::mutex mutex_;
  std::vector<Point> points_;
  bool active_ = false, closed_ = false;
  size_t index_ = 0;
  std::atomic<bool> segment_{false}, requested_{false}, spin_enabled_{false}, stopping_{false};
  std::optional<bool> override_state_;
  bool permitted_ = false;
  Clock::time_point override_at_{}, permitted_at_{};
  std::thread worker_;

  bool competing() {return count_publishers("/navigation_done") > 1;}
  bool permitted() {
    std::lock_guard<std::mutex> lock(mutex_);
    return permitted_ && Clock::now() - permitted_at_ < 500ms;
  }
  bool override_confirmed(bool value, Clock::time_point after = Clock::time_point{}) {
    std::lock_guard<std::mutex> lock(mutex_);
    return override_state_.has_value() && *override_state_ == value && override_at_ >= after &&
      Clock::now() - override_at_ < 500ms;
  }
  bool override_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    return override_state_.value_or(false);
  }
  bool override_fresh() {
    std::lock_guard<std::mutex> lock(mutex_);
    return override_state_.has_value() && Clock::now() - override_at_ < 500ms;
  }
  void publish_controls() {
    Bool m;
    if (!competing()) {m.data = spin_enabled_ && !segment_; spin_pub_->publish(m);}
    m.data = spin_enabled_; spin_state_pub_->publish(m);
    if (segment_) {m.data = requested_; override_pub_->publish(m);}
  }
  bool canceled(const std::shared_ptr<RouteHandle> & h) const {
    return stopping_ || !rclcpp::ok() || (h && h->is_canceling());
  }
  bool enter(const std::shared_ptr<RouteHandle> & h) {
    if (competing()) return false;
    segment_ = true; requested_ = true;
    const auto start = Clock::now(); publish_controls();
    while (Clock::now() - start < 2s && !canceled(h)) {
      if (!permitted()) return false;
      if (override_confirmed(true, start)) return true;
      std::this_thread::sleep_for(20ms);
    }
    return false;
  }
  bool leave(const std::shared_ptr<RouteHandle> & h = {}) {
    if (competing() || canceled(h)) return false;
    requested_ = false; const auto start = Clock::now();
    while (Clock::now() - start < 2s && !stopping_ && rclcpp::ok()) {
      if (h && (canceled(h) || !permitted())) {requested_ = true; publish_controls(); return false;}
      Bool m; m.data = false; override_pub_->publish(m);
      if (override_confirmed(false, start)) {segment_ = false; return true;}
      std::this_thread::sleep_for(50ms);
    }
    requested_ = true; return false;
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
  template<class Future> bool wait_for(Future & f, std::chrono::milliseconds timeout,
      const std::shared_ptr<RouteHandle> & h = {}) {
    const auto end = Clock::now() + timeout;
    while (Clock::now() < end && !canceled(h)) {
      if (f.wait_for(20ms) == std::future_status::ready) return true;
    }
    return f.wait_for(0ms) == std::future_status::ready;
  }
  template<class Action> bool cancel_nav(typename rclcpp_action::Client<Action>::SharedPtr client,
      typename rclcpp_action::ClientGoalHandle<Action>::SharedPtr handle,
      std::shared_future<typename rclcpp_action::ClientGoalHandle<Action>::WrappedResult> & result) {
    client->async_cancel_goal(handle);
    if (result.wait_for(3s) == std::future_status::ready) return true;
    if (cancel_client_->service_is_ready()) {
      auto f = cancel_client_->async_send_request(std::make_shared<std_srvs::srv::Trigger::Request>());
      if (f.wait_for(2s) == std::future_status::ready && f.get()->success) return true;
    }
    RCLCPP_ERROR(get_logger(), "Could not confirm cancellation of child task"); return false;
  }
  std::string rotation(const std::shared_ptr<RouteHandle> & h, const std::atomic<bool> & stop) {
    const double axis = get_parameter("special_axis_yaw").as_double();
    if (!std::isfinite(axis)) return "invalid_special_axis";
    double yaw;
    try {
      const auto t = tf_.lookupTransform("map", "chassis_link", tf2::TimePointZero);
      double age = (now()-rclcpp::Time(t.header.stamp, get_clock()->get_clock_type())).seconds();
      if (age < -0.05 || age > 0.2) return "stale_special_heading";
      auto q = t.transform.rotation;
      double norm = std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
      if (!std::isfinite(norm) || norm < 1e-12) return "invalid_special_heading";
      q.x/=norm; q.y/=norm; q.z/=norm; q.w/=norm;
      const double current = std::atan2(2*(q.w*q.z+q.x*q.y), 1-2*(q.y*q.y+q.z*q.z));
      double a = std::atan2(std::sin(axis), std::cos(axis));
      double b = std::atan2(std::sin(axis+M_PI), std::cos(axis+M_PI));
      auto diff = [current](double x) {return std::abs(std::atan2(std::sin(x-current),std::cos(x-current)));};
      yaw = diff(a) <= diff(b) ? a : b;
    } catch (const std::exception &) {return "special_heading_unavailable";}
    if (!rotate_->wait_for_action_server(3s)) return "rotation_unavailable";
    Rotation::Goal req; req.target_yaw = yaw; req.tolerance = get_parameter("special_yaw_tolerance").as_double();
    req.timeout = get_parameter("special_rotation_timeout").as_double(); req.allow_translation = true;
    const auto deadline = Clock::now()+5s;
    while (Clock::now() < deadline && !canceled(h) && !stop) {
      struct Pending {std::mutex mutex; bool abandoned = false; rclcpp_action::ClientGoalHandle<Rotation>::SharedPtr handle;};
      auto pending = std::make_shared<Pending>();
      rclcpp_action::Client<Rotation>::SendGoalOptions options;
      auto client = rotate_;
      options.goal_response_callback = [pending, client](auto handle) {
        std::lock_guard<std::mutex> lock(pending->mutex);
        pending->handle = handle;
        if (handle && pending->abandoned) client->async_cancel_goal(handle);
      };
      auto f = rotate_->async_send_goal(req, options);
      if (!wait_for(f, 2s, h)) {
        std::lock_guard<std::mutex> lock(pending->mutex);
        pending->abandoned = true;
        if (pending->handle) client->async_cancel_goal(pending->handle);
        return "rotation_request_timeout";
      }
      auto handle = f.get();
      if (!handle) {std::this_thread::sleep_for(100ms); continue;}
      auto result = rotate_->async_get_result(handle);
      const auto until = Clock::now()+std::chrono::duration<double>(req.timeout+2.0);
      while (rclcpp::ok() && !stopping_ && result.wait_for(20ms) != std::future_status::ready && Clock::now() < until) {
        if (canceled(h) || stop || !override_confirmed(true) || !permitted()) {
          rotate_->async_cancel_goal(handle);
          if (result.wait_for(3s) != std::future_status::ready) return "rotation_cancel_unconfirmed";
          return canceled(h) ? "rotation_canceled" : "segment_override_lost";
        }
      }
      if (result.wait_for(0ms) != std::future_status::ready) {
        rotate_->async_cancel_goal(handle);
        return result.wait_for(3s) == std::future_status::ready ? "rotation_timeout" : "rotation_cancel_unconfirmed";
      }
      auto value = result.get();
      if (value.result) RCLCPP_INFO(get_logger(), "Special rotation result: %s", value.result->reason.c_str());
      return "rotation_finished";
    }
    return "rotation_rejected";
  }
  template<class Action> std::string navigate_impl(const std::shared_ptr<RouteHandle> & h,
      const std::vector<Point> & route, typename rclcpp_action::Client<Action>::SharedPtr client) {
    if (!client->wait_for_action_server(3s)) return "server_unavailable";
    typename Action::Goal request;
    if constexpr (std::is_same_v<Action, Single>) request.pose = route.front().pose;
    else for (const auto & p : route) request.poses.push_back(p.pose);
    auto progress = std::make_shared<std::atomic<size_t>>(0);
    struct Pending {std::mutex mutex; bool abandoned = false; typename rclcpp_action::ClientGoalHandle<Action>::SharedPtr handle;};
    auto pending = std::make_shared<Pending>();
    typename rclcpp_action::Client<Action>::SendGoalOptions options;
    options.goal_response_callback = [pending, client](auto handle) {
      std::lock_guard<std::mutex> lock(pending->mutex);
      pending->handle = handle;
      if (handle && pending->abandoned) client->async_cancel_goal(handle);
    };
    if constexpr (std::is_same_v<Action, Multi>) {
      const auto n = route.size();
      options.feedback_callback = [progress,n](auto, const std::shared_ptr<const Multi::Feedback> feedback) {
        auto remaining = std::max<int64_t>(0, feedback->number_of_poses_remaining);
        *progress = remaining >= static_cast<int64_t>(n) ? 0 : std::min(n-1, n-static_cast<size_t>(remaining));
      };
    }
    auto send = client->async_send_goal(request, options);
    if (!wait_for(send, 3s, h)) {
      std::lock_guard<std::mutex> lock(pending->mutex);
      pending->abandoned = true;
      if (pending->handle) client->async_cancel_goal(pending->handle);
      return "send_timeout_or_cancel";
    }
    auto child = send.get(); if (!child) return "navigation_rejected";
    auto result = client->async_get_result(child);
    const bool begin = route.back().kind == Point::SPECIAL_BEGIN;
    auto end_it = std::find_if(route.begin(), route.end(), [](const Point & p) {return p.kind == Point::SPECIAL_END;});
    const size_t end_index = static_cast<size_t>(end_it-route.begin());
    bool end_processed = false;
    std::atomic<bool> rotation_stop{false};
    std::future<std::string> turn;
    auto stop_turn = [&]() {rotation_stop = true; if (turn.valid()) turn.wait();};
    std::string phase = "navigating";
    while (rclcpp::ok() && !stopping_ && result.wait_for(0ms) != std::future_status::ready) {
      if (segment_ && (!override_confirmed(true) || !permitted())) {
        cancel_nav<Action>(client, child, result); stop_turn(); return "segment_override_lost";
      }
      if (segment_ && competing()) {
        cancel_nav<Action>(client, child, result); stop_turn(); return "another_spin_publisher_active";
      }
      if (canceled(h)) {cancel_nav<Action>(client, child, result); stop_turn(); return "canceled";}
      if (segment_ && !end_processed && end_it != route.end() && end_index <= *progress && inside(*end_it)) {
        phase = "leaving_special_segment";
        if (!leave(h)) {cancel_nav<Action>(client, child, result); stop_turn(); return "segment_release_unconfirmed";}
        end_processed = true;
        phase = "navigating";
      }
      if (begin && !turn.valid() && *progress == route.size()-1 && inside(route.back())) {
        if (segment_) {cancel_nav<Action>(client, child, result); return "special_end_not_reached";}
        phase = "entering_special_segment";
        if (!enter(h)) {cancel_nav<Action>(client, child, result); return "segment_override_unconfirmed";}
        phase = "navigating_and_rotating";
        turn = std::async(std::launch::async, [this,h,&rotation_stop]() {return rotation(h, rotation_stop);});
      }
      auto feedback = std::make_shared<Route::Feedback>();
      {std::lock_guard<std::mutex> lock(mutex_); feedback->current_index = index_; feedback->total_points = points_.size();}
      feedback->phase = phase; h->publish_feedback(feedback);
      std::this_thread::sleep_for(25ms);
    }
    if (canceled(h)) {cancel_nav<Action>(client, child, result); stop_turn(); return "canceled";}
    const auto nav_result = result.get();
    if (nav_result.code != rclcpp_action::ResultCode::SUCCEEDED) {stop_turn(); return "navigation_failed";}
    if (segment_ && !end_processed && end_it != route.end()) {
      if (!inside(*end_it)) {stop_turn(); return "special_end_not_reached";}
      if (!leave(h)) {stop_turn(); return "segment_release_unconfirmed";}
      end_processed = true;
    }
    if (!begin) return "completed";
    std::string outcome;
    if (!turn.valid()) {
      if (segment_) return "special_end_not_reached";
      if (!enter(h)) return "segment_override_unconfirmed";
      outcome = rotation(h, rotation_stop);
    } else outcome = turn.get();
    if (canceled(h)) return "canceled";
    if (outcome == "segment_override_lost" || outcome == "rotation_request_timeout" ||
        outcome == "rotation_cancel_unconfirmed" || outcome == "invalid_special_axis" ||
        outcome == "stale_special_heading" || outcome == "invalid_special_heading" ||
        outcome == "special_heading_unavailable") return outcome;
    RCLCPP_INFO(get_logger(), "Special begin rotation ended: %s", outcome.c_str());
    return "begin";
  }
  void execute(std::shared_ptr<RouteHandle> h) {
    std::string reason;
    auto result = std::make_shared<Route::Result>();
    try {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        points_ = h->get_goal()->points; index_ = 0;
      }
      while (!canceled(h)) {
        std::vector<Point> chunk; size_t end;
        {
          std::lock_guard<std::mutex> lock(mutex_);
          if (index_ >= points_.size()) {closed_ = true; break;}
          end = index_+1;
          if (h->get_goal()->continuous) {
            while (end < points_.size() && points_[end-1].kind != Point::SPECIAL_BEGIN) ++end;
          }
          chunk.assign(points_.begin()+index_, points_.begin()+end);
        }
        const auto outcome = h->get_goal()->continuous && chunk.size() > 1 ?
          navigate_impl<Multi>(h,chunk,multi_) : navigate_impl<Single>(h,chunk,single_);
        if (outcome != "completed" && outcome != "begin") {reason = outcome; break;}
        {std::lock_guard<std::mutex> lock(mutex_); index_ = end;}
      }
      if (canceled(h)) reason = "canceled";
      if (reason.empty() && !segment_) {result->success = true; result->reason = "completed"; h->succeed(result);}
      else {
        result->reason = reason.empty() ? "unfinished_special_segment" : reason;
        RCLCPP_WARN(get_logger(), "Route ended: reason=%s point_index=%zu segment_active=%d",
          result->reason.c_str(), index_, static_cast<int>(segment_.load()));
        if (segment_) spin_enabled_ = false;
        if (h->is_canceling()) h->canceled(result); else h->abort(result);
      }
    } catch (const std::exception & e) {
      RCLCPP_ERROR(get_logger(), "Route failed: %s", e.what());
      if (segment_) spin_enabled_ = false;
      result->success = false; result->reason = std::string("route_exception: ")+e.what();
      if (h->is_active()) {if (h->is_canceling()) h->canceled(result); else h->abort(result);}
    }
    {std::lock_guard<std::mutex> lock(mutex_); active_ = false; points_.clear();}
  }
  void set_spin(const std::shared_ptr<SetBool::Request> request, std::shared_ptr<SetBool::Response> response) {
    if (competing()) {response->message = "another_spin_publisher_active"; return;}
    if (!override_fresh()) {response->message = "chassis_override_state_unavailable"; return;}
    if (override_confirmed(true) && !segment_) {segment_ = true; requested_ = true;}
    if (segment_) {
      {std::lock_guard<std::mutex> lock(mutex_); if (active_) {response->message = "special_segment_active"; return;}}
      if (!request->data) {spin_enabled_ = false; response->success = true; response->message = "spin_disabled_override_held"; return;}
    }
    if (request->data) {
      if (!permitted()) {response->message = "chassis_not_in_navigation_mode"; return;}
      double speed = get_parameter("spin_speed").as_double();
      if (!std::isfinite(speed) || std::abs(speed) > 6.0 || !speed_client_->wait_for_service(1s)) {
        response->message = "spin_speed_service_unavailable_or_invalid"; return;
      }
      auto req = std::make_shared<Params::Request>();
      req->parameters.push_back(rclcpp::Parameter("navigation_rotation_speed", speed).to_parameter_msg());
      auto f = speed_client_->async_send_request(req);
      if (f.wait_for(2s) != std::future_status::ready) {
        response->message = "spin_speed_not_confirmed"; return;
      }
      const auto reply = f.get();
      if (!reply || reply->results.empty() || !reply->results.front().successful) {
        response->message = "spin_speed_not_confirmed"; return;
      }
      if (segment_ && !leave()) {response->message = "segment_release_unconfirmed"; return;}
    }
    spin_enabled_ = request->data; response->success = true;
    response->message = request->data ? "spin_enabled" : "spin_disabled";
  }
public:
  RouteManager() : Node("sentry_route_manager"), tf_(get_clock()), listener_(tf_) {
    group_ = create_callback_group(rclcpp::CallbackGroupType::Reentrant);
    declare_parameter("spin_speed", 2.0); declare_parameter("special_point_radius", 0.5);
    declare_parameter("special_axis_yaw", 0.0); declare_parameter("special_yaw_tolerance", 0.1);
    declare_parameter("special_rotation_timeout", 6.0);
    single_ = rclcpp_action::create_client<Single>(this, "navigate_to_pose", group_);
    multi_ = rclcpp_action::create_client<Multi>(this, "navigate_through_poses", group_);
    rotate_ = rclcpp_action::create_client<Rotation>(this, "rotate_to_yaw", group_);
    speed_client_ = create_client<Params>("/chassis_sentry_controller/set_parameters", rmw_qos_profile_services_default, group_);
    cancel_client_ = create_client<std_srvs::srv::Trigger>("cancel_navigation", rmw_qos_profile_services_default, group_);
    spin_pub_ = create_publisher<Bool>("/navigation_done", rclcpp::SensorDataQoS());
    spin_state_pub_ = create_publisher<Bool>("navigation_spin_requested", rclcpp::QoS(1).transient_local());
    override_pub_ = create_publisher<Bool>("/navigation_special_segment", 1);
    rclcpp::SubscriptionOptions opt; opt.callback_group = group_;
    override_sub_ = create_subscription<Bool>("/navigation_special_segment_state", 1, [this](Bool::ConstSharedPtr m) {
      std::lock_guard<std::mutex> lock(mutex_); override_state_ = m->data; override_at_ = Clock::now();}, opt);
    permitted_sub_ = create_subscription<Bool>("/navigation_spin_permitted", 1, [this](Bool::ConstSharedPtr m) {
      std::lock_guard<std::mutex> lock(mutex_); permitted_ = m->data; permitted_at_ = Clock::now();}, opt);
    append_ = create_service<Append>("append_route_points", [this](const std::shared_ptr<Append::Request> req, std::shared_ptr<Append::Response> res) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!active_ || closed_ || points_.empty()) {res->reason = "no_active_route"; return;}
      if (!valid_route(req->points) || points_.size()+req->points.size() > 128) {res->reason = "invalid_or_unpaired_points"; return;}
      if (std::any_of(req->points.begin(),req->points.end(),[](const Point & p){return p.kind == Point::SPECIAL_BEGIN;}) && competing()) {
        res->reason = "another_spin_publisher_active"; return;
      }
      points_.insert(points_.end(),req->points.begin(),req->points.end()); res->success = true; res->reason = "appended";
    }, rmw_qos_profile_services_default, group_);
    spin_service_ = create_service<SetBool>("set_navigation_spin", [this](const std::shared_ptr<SetBool::Request> req, std::shared_ptr<SetBool::Response> res) {set_spin(req,res);}, rmw_qos_profile_services_default, group_);
    server_ = rclcpp_action::create_server<Route>(this, "navigate_route",
      [this](const auto &, std::shared_ptr<const Route::Goal> goal) {
        if (!valid_route(goal->points) || segment_ || override_active()) return rclcpp_action::GoalResponse::REJECT;
        const bool special = std::any_of(goal->points.begin(),goal->points.end(),[](const Point & p){return p.kind == Point::SPECIAL_BEGIN;});
        if (special && (!permitted() || competing() || !override_fresh())) return rclcpp_action::GoalResponse::REJECT;
        std::lock_guard<std::mutex> lock(mutex_);
        if (active_) return rclcpp_action::GoalResponse::REJECT;
        active_ = true; closed_ = false; return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
      }, [](auto) {return rclcpp_action::CancelResponse::ACCEPT;},
      [this](std::shared_ptr<RouteHandle> h) {
        if (worker_.joinable()) worker_.join();
        worker_ = std::thread([this,h]() {execute(h);});
      }, rcl_action_server_get_default_options(), group_);
    timer_ = create_wall_timer(100ms, [this]() {publish_controls();}, group_);
  }
  ~RouteManager() override {stopping_ = true; if (worker_.joinable()) worker_.join();}
};

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<RouteManager>();
  rclcpp::executors::MultiThreadedExecutor executor(rclcpp::ExecutorOptions(), 4);
  executor.add_node(node); executor.spin(); executor.remove_node(node); node.reset();
  rclcpp::shutdown(); return 0;
}
