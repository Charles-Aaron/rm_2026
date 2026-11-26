#include "sentry_nav/runtime_controller.hpp"
#include "sentry_nav/translation_goal_checker.hpp"
#include "localization_guard.hpp"
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>
#include <nav2_msgs/msg/speed_limit.hpp>
#include <nav2_util/node_thread.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/transform_listener.h>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <lifecycle_msgs/msg/state.hpp>
#include <sentry_nav_interfaces/srv/prepare_route.hpp>
#include <future>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <atomic>
#include <cmath>
#include <limits>
#include <sstream>
#include <csignal>
#include <thread>

namespace sentry_nav {
using namespace std::chrono_literals;
class NavigationRuntime : public rclcpp_lifecycle::LifecycleNode {
  using Single = nav2_msgs::action::NavigateToPose;
  using Multi = nav2_msgs::action::NavigateThroughPoses;
  using SingleHandle = rclcpp_action::ServerGoalHandle<Single>;
  using MultiHandle = rclcpp_action::ServerGoalHandle<Multi>;
  using Ordered = sentry_nav_interfaces::srv::OrderedRoute;
  using Prepare = sentry_nav_interfaces::srv::PrepareRoute;
  using Steady = std::chrono::steady_clock;
  using Return = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
  struct Plan {
    uint64_t generation;
    size_t first_index;
    nav_msgs::msg::Path path;
    std::string error;
    Steady::time_point submitted;
    geometry_msgs::msg::PoseStamped start;
    bool prepared = false, reused = false;
  };
  struct Preparation {
    rclcpp_action::GoalUUID owner;
    std::vector<geometry_msgs::msg::PoseStamped> goals;
    geometry_msgs::msg::PoseStamped start;
    bool submitted = false;
    std::optional<Plan> result;
  };
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> global_, local_;
  std::unique_ptr<nav2_util::NodeThread> global_thread_, local_thread_;
  std::shared_ptr<tf2_ros::Buffer> tf_;
  std::shared_ptr<tf2_ros::TransformListener> listener_;
  std::unique_ptr<nav2_core::GlobalPlanner> planner_;
  std::unique_ptr<RuntimeController> controller_;
  TranslationGoalChecker checker_;
  LocalizationGuard localization_;
  rclcpp_action::Server<Single>::SharedPtr single_server_;
  rclcpp_action::Server<Multi>::SharedPtr multi_server_;
  rclcpp_action::Client<Single>::SharedPtr topic_client_;
  std::shared_ptr<SingleHandle> single_;
  std::shared_ptr<MultiHandle> multi_;
  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr goal_sub_;
  rclcpp::Subscription<nav2_msgs::msg::SpeedLimit>::SharedPtr speed_sub_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr cancel_service_;
  rclcpp::Service<Prepare>::SharedPtr prepare_service_;
  rclcpp_lifecycle::LifecyclePublisher<geometry_msgs::msg::Twist>::SharedPtr cmd_;
  rclcpp_lifecycle::LifecyclePublisher<nav_msgs::msg::Path>::SharedPtr path_;
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr status_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr parameter_guard_;
  std::vector<geometry_msgs::msg::PoseStamped> goals_;
  std::future<Plan> planning_;
  std::optional<Preparation> preparation_;
  std::optional<Plan> candidate_;
  std::optional<rclcpp_action::GoalUUID> completed_;
  std::atomic<uint64_t> prepare_revision_{0};
  // 单个常驻线程串行访问规划器；预规划不新增线程，也不调用控制器。
  std::thread planner_worker_;
  std::mutex planner_mutex_;
  std::condition_variable planner_cv_;
  std::optional<std::packaged_task<Plan()>> planner_job_;
  bool planner_stopping_ = false;
  std::atomic<uint64_t> generation_{0};
  bool ready_ = false, task_ = false, have_path_ = false, suspended_ = false;
  size_t next_ = 0;
  int failures_ = 0, max_failures_ = 6;
  double frequency_, replan_period_, retry_period_, task_timeout_, state_timeout_;
  double progress_timeout_, progress_radius_, odom_timeout_, plan_max_age_;
  double prepared_start_tolerance_, planner_radius_;
  bool planner_unknown_ = true;
  std::string state_ = "inactive", detail_;
  Steady::time_point started_, next_plan_, progress_at_, last_feedback_;
  std::optional<Steady::time_point> invalid_since_;
  geometry_msgs::msg::Point progress_position_;
  bool progress_initialized_ = false;
  rclcpp::Time started_ros_{0, 0, RCL_ROS_TIME};

  void zero() {if (cmd_ && cmd_->is_activated()) {cmd_->publish(geometry_msgs::msg::Twist{});}}
  void status(const std::string & state, const std::string & detail = "") {
    state_ = state; detail_ = detail;
    std_msgs::msg::String message;
    std::ostringstream out;
    out << "state=" << state << " task=" << generation_.load() << " next_waypoint=" << next_
        << " total=" << goals_.size() << " planning_failures=" << failures_ << " reason=" << detail;
    message.data = out.str(); status_->publish(message);
  }
  static bool valid(const geometry_msgs::msg::PoseStamped & pose) {
    const auto & p = pose.pose.position;
    const auto & q = pose.pose.orientation;
    const double norm = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    return !pose.header.frame_id.empty() && std::isfinite(p.x) && std::isfinite(p.y) &&
      std::isfinite(p.z) && std::isfinite(norm) && norm > 1e-12;
  }
  bool admissible(const std::vector<geometry_msgs::msg::PoseStamped> & goals,
    const std::string & tree = "") const {
    return ready_ && tree.empty() && !goals.empty() && goals.size() <= 128 &&
      std::all_of(goals.begin(), goals.end(), valid);
  }
  void invalidate_preparation() {++prepare_revision_; preparation_.reset(); candidate_.reset();}
  void stop_planner() {
    ++generation_; ++prepare_revision_;
    {std::lock_guard<std::mutex> lock(planner_mutex_); planner_stopping_ = true;}
    planner_cv_.notify_one();
    if (planner_worker_.joinable()) {planner_worker_.join();}
  }
  void prepare(const Prepare::Request & request, Prepare::Response & response) {
    if (request.goals.empty()) {
      if (preparation_ && preparation_->owner == request.predecessor_id) {invalidate_preparation();}
      if (completed_ == request.predecessor_id) {completed_.reset();}
      response.success = true; response.reason = "cleared"; return;
    }
    const bool owner_active = (single_ && single_->get_goal_id() == request.predecessor_id) ||
      (multi_ && multi_->get_goal_id() == request.predecessor_id);
    // 后继点固定在全局系，避免动态 TF 改变目标而误用旧路径。
    const bool global_goals = std::all_of(request.goals.begin(), request.goals.end(), [this](const auto & goal) {
      return goal.header.frame_id == global_->getGlobalFrameID();
    });
    if (!admissible(request.goals) || !global_goals ||
      (!owner_active && (task_ || completed_ != request.predecessor_id))) {
      response.reason = "invalid_preparation_owner_or_goals"; return;
    }
    invalidate_preparation();
    auto start = goals_.back(); start.header.stamp = builtin_interfaces::msg::Time();
    start = transform(start, global_->getGlobalFrameID());
    preparation_ = Preparation{request.predecessor_id, request.goals, start, false, {}};
    response.success = true; response.reason = "queued";
  }
  void finish(bool success, const std::string & reason) {
    if (success) {completed_ = single_ ? single_->get_goal_id() : multi_->get_goal_id();}
    else {completed_.reset(); invalidate_preparation();}
    zero(); ++generation_; task_ = false; have_path_ = false;
    controller_->resetTask(); checker_.reset();
    if (single_) {
      auto result = std::make_shared<Single::Result>();
      if (single_->is_canceling()) {single_->canceled(result);}
      else if (success) {single_->succeed(result);} else {single_->abort(result);}
      single_.reset();
    }
    if (multi_) {
      auto result = std::make_shared<Multi::Result>();
      if (multi_->is_canceling()) {multi_->canceled(result);}
      else if (success) {multi_->succeed(result);} else {multi_->abort(result);}
      multi_.reset();
    }
    nav_msgs::msg::Path empty; empty.header.frame_id = global_->getGlobalFrameID();
    empty.header.stamp = now(); path_->publish(empty);
    if (success) {
      RCLCPP_INFO(get_logger(), "Navigation completed: %s", reason.c_str());
    } else {
      RCLCPP_WARN(get_logger(), "Navigation stopped: %s", reason.c_str());
    }
    status(success ? "succeeded" : "stopped", reason);
  }
  void begin(std::vector<geometry_msgs::msg::PoseStamped> goals) {
    // 仅消费紧邻成功任务的完全相同后继；追加点或替换任务自动退回正常规划。
    collect_plan();
    std::optional<Plan> prepared;
    if (preparation_ && completed_ == preparation_->owner && preparation_->result &&
      preparation_->goals == goals) {prepared = std::move(preparation_->result);}
    invalidate_preparation(); completed_.reset(); candidate_ = std::move(prepared);
    ++generation_; controller_->resetTask(); checker_.reset(); zero();
    goals_ = std::move(goals); task_ = true; have_path_ = false; suspended_ = false;
    failures_ = 0; next_ = 0; invalid_since_.reset(); progress_initialized_ = false;
    started_ = progress_at_ = next_plan_ = Steady::now(); started_ros_ = now();
    if (goals_.size() > 1) {query();}
    status("planning");
  }
  Ordered::Response query() {
    Ordered::Request request; request.operation = Ordered::Request::QUERY;
    request.task_id = std::to_string(generation_.load()); request.goals = goals_;
    auto response = controller_->orderedRequest(request);
    if (!response.success) {throw std::runtime_error(response.reason);}
    next_ = response.first_index;
    return response;
  }
  geometry_msgs::msg::PoseStamped transform(geometry_msgs::msg::PoseStamped pose,
    const std::string & frame) {
    if (pose.header.frame_id == frame) {return pose;}
    return tf_->transform(pose, frame, tf2::durationFromSec(0.02));
  }
  geometry_msgs::msg::PoseStamped current_pose(nav_msgs::msg::Odometry::ConstSharedPtr & odom) {
    if (!localization_.ready()) {throw std::runtime_error("localization_unavailable");}
    odom = controller_->odometry();
    if (!odom) {throw std::runtime_error("missing_odometry");}
    const double age = (now() - rclcpp::Time(odom->header.stamp, get_clock()->get_clock_type())).seconds();
    geometry_msgs::msg::PoseStamped pose; pose.header = odom->header; pose.pose = odom->pose.pose;
    const auto & v = odom->twist.twist;
    if (!valid(pose) || age < -0.05 || age > odom_timeout_ ||
      odom->child_frame_id != local_->getBaseFrameID() || !std::isfinite(v.linear.x) ||
      !std::isfinite(v.linear.y) || !std::isfinite(v.angular.z)) {
      throw std::runtime_error("invalid_or_stale_odometry");
    }
    if (!local_->isCurrent() || !global_->isCurrent()) {throw std::runtime_error("costmap_not_current");}
    // Global task progress uses the latest localization correction, as Nav2's
    // robot-pose lookup did. map->odom may publish slower than odometry; asking
    // for the odometry stamp would repeatedly extrapolate into its future.
    // The controller still transforms its coherent odometry at the sample stamp
    // into the local odom frame; no control-state timestamp is changed here.
    const auto stamp = pose.header.stamp;
    pose.header.stamp = builtin_interfaces::msg::Time();
    pose = transform(pose, global_->getGlobalFrameID());
    pose.header.stamp = stamp;
    return pose;
  }
  void suspend(const std::string & reason) {
    zero(); checker_.reset();
    if (!suspended_) {
      RCLCPP_WARN(get_logger(), "Navigation waiting: %s", reason.c_str());
      controller_->stopMotion(); suspended_ = true; status("waiting", reason);
    }
  }
  void start_plan(const geometry_msgs::msg::PoseStamped & start, bool preparing = false) {
    auto remaining = preparing ? preparation_->goals : goals_;
    const size_t first = !preparing && goals_.size() > 1 ? query().first_index : 0;
    remaining.erase(remaining.begin(), remaining.begin() + first);
    const auto version = preparing ? prepare_revision_.load() : generation_.load();
    const auto submitted = Steady::now();
    auto cached = preparing ? std::optional<Plan>{} : std::move(candidate_);
    candidate_.reset();
    std::packaged_task<Plan()> job([this, remaining, first, version, submitted, start, preparing,
        cached = std::move(cached)]() mutable {
      Plan result{version, first, nav_msgs::msg::Path{}, "", submitted, start, preparing, false};
      try {
        if (cached && cached->error.empty() && first == 0 &&
          std::chrono::duration<double>(submitted - cached->submitted).count() <= plan_max_age_ &&
          std::hypot(start.pose.position.x - cached->start.pose.position.x,
                     start.pose.position.y - cached->start.pose.position.y) <= prepared_start_tolerance_ &&
          prepared_path_safe(global_, cached->path, start, planner_radius_, planner_unknown_) &&
          std::chrono::duration<double>(Steady::now()-cached->submitted).count() <= plan_max_age_) {
          result.path = std::move(cached->path); result.reused = true; return result;
        }
        auto from = start;
        result.path.header.frame_id = global_->getGlobalFrameID(); result.path.header.stamp = now();
        for (auto goal : remaining) {
          if (version != (preparing ? prepare_revision_.load() : generation_.load()) || !rclcpp::ok()) {
            throw std::runtime_error("superseded");
          }
          if (std::chrono::duration<double>(Steady::now() - submitted).count() > plan_max_age_) {
            throw std::runtime_error("global_plan_expired");
          }
          // Freeze each destination in the global frame for this route snapshot.
          goal.header.stamp = builtin_interfaces::msg::Time();
          goal = transform(goal, result.path.header.frame_id);
          auto segment = planner_->createPlan(from, goal);
          if (segment.poses.empty()) {throw std::runtime_error("empty_global_path");}
          result.path.poses.insert(result.path.poses.end(), segment.poses.begin(), segment.poses.end());
          from = goal;
        }
      } catch (const std::exception & e) {result.error = e.what();}
      return result;
    });
    planning_ = job.get_future();
    {std::lock_guard<std::mutex> lock(planner_mutex_); planner_job_ = std::move(job);}
    if (preparing) {preparation_->submitted = true;}
    planner_cv_.notify_one();
  }
  void collect_plan() {
    if (!planning_.valid() || planning_.wait_for(0ms) != std::future_status::ready) {return;}
    auto result = planning_.get();
    if (result.prepared) {
      if (preparation_ && result.generation == prepare_revision_) {
        preparation_->result = std::move(result);
        RCLCPP_INFO(get_logger(), "Successor preparation: %s",
          preparation_->result->error.empty() ? "ready" : preparation_->result->error.c_str());
        status(state_, preparation_->result->error.empty() ? "successor_prepared" : "successor_preparation_failed");
      }
      return;
    }
    if (result.generation != generation_ || !task_) {return;}
    if (goals_.size() > 1 && result.first_index != query().first_index) {next_plan_ = Steady::now(); return;}
    if (std::chrono::duration<double>(Steady::now() - result.submitted).count() > plan_max_age_) {
      result.error = "global_plan_expired";
    }
    if (!result.error.empty()) {
      suspend(result.error); have_path_ = false;
      if (failures_ < std::numeric_limits<int>::max()) {++failures_;}
      // 上限为 0 时持续重试，仍保留失败计数和总任务超时。
      if (max_failures_ > 0 && failures_ >= max_failures_) {
        finish(false, "global_planning_failed: " + result.error); return;
      }
      next_plan_ = Steady::now() + std::chrono::duration_cast<Steady::duration>(std::chrono::duration<double>(retry_period_));
      status("retry_wait", result.error); return;
    }
    if (goals_.size() > 1) {
      Ordered::Request bind; bind.operation = Ordered::Request::BIND;
      bind.task_id = std::to_string(generation_.load()); bind.first_index = result.first_index;
      bind.path = result.path;
      const auto response = controller_->orderedRequest(bind);
      if (!response.success) {throw std::runtime_error("ordered_binding: " + response.reason);}
    }
    controller_->setPlan(result.path); path_->publish(result.path);
    have_path_ = true; suspended_ = false; failures_ = 0;
    next_plan_ = Steady::now() + std::chrono::duration_cast<Steady::duration>(std::chrono::duration<double>(replan_period_));
    status("tracking", result.reused ? "prepared_plan_reused" : "");
  }
  void schedule_plan(const geometry_msgs::msg::PoseStamped & pose) {
    if (planning_.valid()) {return;}
    if (task_ && Steady::now() >= next_plan_) {start_plan(pose);}
    else if (preparation_ && !preparation_->submitted) {start_plan(preparation_->start, true);}
  }
  template<class Feedback> void feedback(Feedback & out, const geometry_msgs::msg::PoseStamped & pose) {
    out.current_pose = pose;
    out.navigation_time = rclcpp::Duration::from_seconds(
      std::max(0.0, (now() - started_ros_).seconds()));
    double distance = 0; auto previous = pose;
    for (size_t i = next_; i < goals_.size(); ++i) {
      auto goal = goals_[i]; goal.header.stamp = builtin_interfaces::msg::Time();
      goal = transform(goal, pose.header.frame_id);
      distance += std::hypot(goal.pose.position.x - previous.pose.position.x,
        goal.pose.position.y - previous.pose.position.y); previous = goal;
    }
    // Geometric waypoint distance, not an obstacle-aware ETA.
    out.distance_remaining = distance;
    out.number_of_recoveries = failures_;
  }
  void tick() {
    if (!ready_) {return;}
    if (!task_) {
      zero(); collect_plan();
      if (preparation_) {
        try {
          nav_msgs::msg::Odometry::ConstSharedPtr odom;
          schedule_plan(current_pose(odom));
        } catch (const std::exception &) {completed_.reset(); invalidate_preparation();}
      }
      return;
    }
    if ((single_ && single_->is_canceling()) || (multi_ && multi_->is_canceling())) {
      finish(false, "canceled"); return;
    }
    const auto steady = Steady::now();
    if (std::chrono::duration<double>(steady - started_).count() > task_timeout_) {
      finish(false, "task_timeout"); return;
    }
    try {
      nav_msgs::msg::Odometry::ConstSharedPtr odom;
      geometry_msgs::msg::PoseStamped pose;
      try {pose = current_pose(odom);}
      catch (const std::exception & e) {
        invalidate_preparation();
        suspend(e.what());
        if (!invalid_since_) {invalid_since_ = steady;}
        if (std::chrono::duration<double>(steady - *invalid_since_).count() > state_timeout_) {
          finish(false, "state_timeout: " + std::string(e.what()));
        }
        return;
      }
      if (invalid_since_) {
        // Reject planning work produced before a state outage.
        if (planning_.valid() && planning_.wait_for(0ms) != std::future_status::ready) {zero(); return;}
        if (planning_.valid()) {planning_.get();}
        invalid_since_.reset(); have_path_ = false; next_plan_ = steady;
      }
      collect_plan();
      if (!task_) {return;}
      schedule_plan(pose);
      if (!have_path_) {zero(); return;}
      suspended_ = false;
      auto command = controller_->computeVelocityCommands(pose, odom->twist.twist, &checker_);
      // Recheck the sample after the solver; never succeed using stale state.
      if ((now() - rclcpp::Time(odom->header.stamp, get_clock()->get_clock_type())).seconds() > odom_timeout_) {
        suspend("odometry_stale_at_output"); return;
      }
      auto goal = goals_.back(); goal.header.stamp = builtin_interfaces::msg::Time();
      goal = transform(goal, pose.header.frame_id);
      if (checker_.isGoalReached(pose.pose, goal.pose, odom->twist.twist)) {
        finish(true, "goal_reached"); return;
      }
      // 超时为 0 时跳过位移进展检查。
      if (progress_timeout_ > 0) {
        const auto & p = pose.pose.position;
        if (!progress_initialized_ || std::hypot(p.x - progress_position_.x, p.y - progress_position_.y) >= progress_radius_) {
          progress_initialized_ = true; progress_position_ = p; progress_at_ = steady;
        } else if (std::chrono::duration<double>(steady - progress_at_).count() > progress_timeout_) {
          std::ostringstream reason;
          reason << "progress_timeout: moved="
                 << std::hypot(p.x - progress_position_.x, p.y - progress_position_.y)
                 << "m required=" << progress_radius_ << "m within=" << progress_timeout_
                 << "s measured_speed=" << std::hypot(odom->twist.twist.linear.x, odom->twist.twist.linear.y)
                 << "m/s requested_speed=" << std::hypot(command.twist.linear.x, command.twist.linear.y)
                 << "m/s; check motion_gate, relay and chassis feedback";
          finish(false, reason.str()); return;
        }
      }
      cmd_->publish(command.twist);
      if (steady - last_feedback_ >= 200ms) {
        if (goals_.size() > 1) {query();}
        if (single_) {auto out = std::make_shared<Single::Feedback>(); feedback(*out, pose); single_->publish_feedback(out);}
        if (multi_) {
          auto out = std::make_shared<Multi::Feedback>(); feedback(*out, pose);
          out->number_of_poses_remaining = goals_.size() - next_; multi_->publish_feedback(out);
        }
        status(state_, detail_); last_feedback_ = steady;
      }
    } catch (const std::exception & e) {finish(false, "runtime_error: " + std::string(e.what()));}
  }
public:
  NavigationRuntime() : LifecycleNode("sentry_navigation") {}
  Return on_configure(const rclcpp_lifecycle::State &) override {
    try {
      localization_.configure(*this);
      auto positive = [this](const std::string & name, double value) {
        const auto out = declare_parameter(name, value);
        if (!std::isfinite(out) || out <= 0) {throw std::invalid_argument(name + " must be positive");}
        return out;
      };
      frequency_ = positive("controller_frequency", 40.0);
      replan_period_ = positive("global_replan_period", 1.0);
      retry_period_ = positive("global_retry_period", 1.0);
      task_timeout_ = positive("task_timeout", 180.0);
      state_timeout_ = positive("state_timeout", 2.0);
      plan_max_age_ = positive("global_plan_max_age", 2.0);
      prepared_start_tolerance_ = positive("prepared_plan_start_tolerance", 0.2);
      progress_timeout_ = declare_parameter("progress_timeout", 8.0);
      if (!std::isfinite(progress_timeout_) || progress_timeout_ < 0) {
        throw std::invalid_argument("progress_timeout must be finite and nonnegative");
      }
      progress_radius_ = positive("progress_radius", 0.25);
      odom_timeout_ = positive("FollowPath.odom_timeout", 0.2);
      max_failures_ = declare_parameter("max_planning_failures", 6);
      if (max_failures_ < 0) {throw std::invalid_argument("max_planning_failures must be nonnegative");}
      declare_parameter("enable_ordered_route_service", false);
      if (get_parameter("enable_ordered_route_service").as_bool()) {
        throw std::invalid_argument("standalone runtime owns waypoint binding; external service must be disabled");
      }
      tf_ = std::make_shared<tf2_ros::Buffer>(get_clock());
      listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_);
      global_ = std::make_shared<nav2_costmap_2d::Costmap2DROS>("global_costmap", get_namespace(), "global_costmap");
      local_ = std::make_shared<nav2_costmap_2d::Costmap2DROS>("local_costmap", get_namespace(), "local_costmap");
      global_thread_ = std::make_unique<nav2_util::NodeThread>(global_);
      local_thread_ = std::make_unique<nav2_util::NodeThread>(local_);
      if (global_->configure().id() != lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE ||
        local_->configure().id() != lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE) {
        throw std::runtime_error("costmap configure failed");
      }
      planner_ = make_runtime_planner(); controller_ = make_runtime_controller();
      planner_->configure(shared_from_this(), "GridBased", tf_, global_);
      planner_radius_ = get_parameter("GridBased.robot_radius").as_double();
      planner_unknown_ = get_parameter("GridBased.unknown_is_obstacle").as_bool();
      planner_worker_ = std::thread([this]() {
        while (true) {
          std::optional<std::packaged_task<Plan()>> job;
          {
            std::unique_lock<std::mutex> lock(planner_mutex_);
            planner_cv_.wait(lock, [this]() {return planner_stopping_ || planner_job_.has_value();});
            if (planner_stopping_ && !planner_job_) {return;}
            job = std::move(planner_job_); planner_job_.reset();
          }
          (*job)();
        }
      });
      controller_->configure(shared_from_this(), "FollowPath", tf_, local_);
      checker_.initialize(shared_from_this(), "general_goal_checker", local_);
      cmd_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 1);
      path_ = create_publisher<nav_msgs::msg::Path>("plan", rclcpp::QoS(1).transient_local());
      status_ = create_publisher<std_msgs::msg::String>("navigation_status", rclcpp::QoS(1).transient_local());
      single_server_ = rclcpp_action::create_server<Single>(this, "navigate_to_pose",
        [this](const auto &, auto goal) {
          return admissible({goal->pose}, goal->behavior_tree) ?
            rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE : rclcpp_action::GoalResponse::REJECT;
        }, [](auto) {return rclcpp_action::CancelResponse::ACCEPT;},
        [this](auto handle) {
          if (task_) {finish(false, "preempted");}
          single_ = handle; begin({handle->get_goal()->pose});
        });
      multi_server_ = rclcpp_action::create_server<Multi>(this, "navigate_through_poses",
        [this](const auto &, auto goal) {
          return admissible(goal->poses, goal->behavior_tree) ?
            rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE : rclcpp_action::GoalResponse::REJECT;
        }, [](auto) {return rclcpp_action::CancelResponse::ACCEPT;},
        [this](auto handle) {
          if (task_) {finish(false, "preempted");}
          multi_ = handle; begin(handle->get_goal()->poses);
        });
      topic_client_ = rclcpp_action::create_client<Single>(this, "navigate_to_pose");
      goal_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>("goal_pose", 1,
        [this](geometry_msgs::msg::PoseStamped::ConstSharedPtr pose) {
          if (!admissible({*pose})) {RCLCPP_WARN(get_logger(), "Rejected invalid or inactive topic goal"); return;}
          // Use the same Action lifecycle so the motion gate sees topic tasks too.
          Single::Goal goal; goal.pose = *pose;
          topic_client_->async_send_goal(goal);
        });
      cancel_service_ = create_service<std_srvs::srv::Trigger>("cancel_navigation",
        [this](std_srvs::srv::Trigger::Request::SharedPtr, std_srvs::srv::Trigger::Response::SharedPtr response) {
          if (task_) {finish(false, "canceled_by_service");}
          else {completed_.reset(); invalidate_preparation();}
          response->success = true; response->message = "navigation stopped";
        });
      prepare_service_ = create_service<Prepare>("prepare_route",
        [this](Prepare::Request::SharedPtr request, Prepare::Response::SharedPtr response) {
          try {prepare(*request, *response);}
          catch (const std::exception & e) {invalidate_preparation(); response->reason = e.what();}
        });
      speed_sub_ = create_subscription<nav2_msgs::msg::SpeedLimit>("speed_limit", 1,
        [this](nav2_msgs::msg::SpeedLimit::ConstSharedPtr limit) {
          if (!std::isfinite(limit->speed_limit) || limit->speed_limit < 0) {
            RCLCPP_WARN(get_logger(), "Rejected invalid speed limit"); return;
          }
          controller_->setSpeedLimit(limit->speed_limit, limit->percentage);
        });
      // All runtime callbacks share the default mutually exclusive group.
      // Only immutable global planning jobs run outside it.
      timer_ = create_wall_timer(std::chrono::duration<double>(1.0 / frequency_), [this]() {tick();});
      parameter_guard_ = add_on_set_parameters_callback([](const auto & parameters) {
        rcl_interfaces::msg::SetParametersResult result; result.successful = true;
        for (const auto & parameter : parameters) {
          if (parameter.get_name() != "use_sim_time") {
            result.successful = false; result.reason = "navigation parameters require restart"; break;
          }
        }
        return result;
      });
      return Return::SUCCESS;
    } catch (const std::exception & e) {RCLCPP_ERROR(get_logger(), "Configure: %s", e.what()); return Return::FAILURE;}
  }
  Return on_activate(const rclcpp_lifecycle::State &) override {
    if (global_->activate().id() != lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE ||
      local_->activate().id() != lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE) {return Return::FAILURE;}
    planner_->activate(); controller_->activate();
    cmd_->on_activate(); path_->on_activate(); status_->on_activate();
    timer_->reset(); ready_ = true; status("idle"); return Return::SUCCESS;
  }
  Return on_deactivate(const rclcpp_lifecycle::State &) override {
    ready_ = false; timer_->cancel();
    completed_.reset(); invalidate_preparation();
    if (task_) {finish(false, "deactivated");} else {zero(); ++generation_;}
    if (planning_.valid()) {planning_.wait(); planning_.get();}
    controller_->deactivate(); planner_->deactivate();
    local_->deactivate(); global_->deactivate();
    cmd_->on_deactivate(); path_->on_deactivate(); status_->on_deactivate();
    return Return::SUCCESS;
  }
  bool transformsReady() {
    std::string reason;
    const bool global_ready = global_->getTfBuffer()->canTransform(
      global_->getGlobalFrameID(), global_->getBaseFrameID(), tf2::TimePointZero,
      tf2::durationFromSec(0), &reason);
    const bool local_ready = local_->getTfBuffer()->canTransform(
      local_->getGlobalFrameID(), local_->getBaseFrameID(), tf2::TimePointZero,
      tf2::durationFromSec(0), &reason);
    if (!global_ready || !local_ready) {
      RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 5000,
        "Waiting for startup TF (map/odom -> chassis): %s", reason.c_str());
    }
    return global_ready && local_ready;
  }
  void release() {
    stop_planner();
    if (controller_) {controller_->cleanup(); controller_.reset();}
    if (planner_) {planner_->cleanup(); planner_.reset();}
    if (local_) {local_->cleanup();}
    if (global_) {global_->cleanup();}
  }
  ~NavigationRuntime() override {
    stop_planner();
    // The controller owns a worker holding node references; clean it explicitly.
    if (controller_) {controller_.reset();}
    planner_.reset(); local_thread_.reset(); global_thread_.reset();
  }
};
}  // namespace sentry_nav

namespace {
volatile std::sig_atomic_t stop_requested = 0;
void request_stop(int) {stop_requested = 1;}
}

int main(int argc, char ** argv) {
  // Keep the context alive until actions, workers and costmaps are stopped.
  // The signal handler only sets a flag; ROS work runs on the executor thread.
  rclcpp::init(argc, argv, rclcpp::InitOptions(), rclcpp::SignalHandlerOptions::None);
  std::signal(SIGINT, request_stop); std::signal(SIGTERM, request_stop);
  auto node = std::make_shared<sentry_nav::NavigationRuntime>();
  if (node->configure().id() != lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE) {
    rclcpp::shutdown(); return 1;
  }
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node->get_node_base_interface());
  // Costmap activation waits for TF. Wait here first so missing startup TF
  // cannot prevent an operator from stopping the process.
  while (rclcpp::ok() && !stop_requested && !node->transformsReady()) {
    executor.spin_some(); std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  bool active = false;
  if (rclcpp::ok() && !stop_requested) {
    active = node->activate().id() == lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE;
    if (active) {
      while (rclcpp::ok() && !stop_requested) {executor.spin_once(std::chrono::milliseconds(25));}
    }
  }
  if (active) {node->deactivate();}
  node->release(); executor.remove_node(node->get_node_base_interface()); node.reset();
  rclcpp::shutdown(); return active || stop_requested ? 0 : 1;
}
