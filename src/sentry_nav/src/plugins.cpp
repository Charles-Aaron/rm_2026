#include "sentry_nav/mpc.hpp"
#include "sentry_nav/runtime_controller.hpp"
#include "sentry_nav/minimum_snap.hpp"
#include "sentry_nav/trajectory_handover.hpp"
#include "sentry_nav/replan_checks.hpp"
#include "sentry_nav/spin_prediction.hpp"
#include "sentry_nav/local_target.hpp"
#include "sentry_nav/goal_settler.hpp"
#include "sentry_nav/ordered_route.hpp"
#include "sentry_nav/translation_goal_checker.hpp"
#include <sentry_nav_interfaces/srv/ordered_route.hpp>
#include <Eigen/Geometry>
#include <nav2_core/controller.hpp>
#include <nav2_core/global_planner.hpp>
#include <nav2_core/exceptions.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/utils.h>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <std_msgs/msg/string.hpp>
#include <sstream>
#include <iomanip>
#include <limits>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <thread>
#include <optional>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace sentry_nav {
template<class T> T parameter(const rclcpp_lifecycle::LifecycleNode::SharedPtr & node,
  const std::string & name, T value)
{
  if (!node->has_parameter(name)) {node->declare_parameter<T>(name, value);}
  return node->get_parameter(name).get_value<T>();
}
core::PreparedMap snapshot(const std::shared_ptr<nav2_costmap_2d::Costmap2DROS> & ros,
  double radius, bool unknown)
{
  core::GridMap map;
  {
    auto * source = ros->getCostmap();
    std::lock_guard<nav2_costmap_2d::Costmap2D::mutex_t> lock(*source->getMutex());
    map.width = source->getSizeInCellsX(); map.height = source->getSizeInCellsY();
    map.resolution = source->getResolution();
    map.origin = {source->getOriginX(), source->getOriginY()}; map.frame = ros->getGlobalFrameID();
    map.occupancy.resize(map.width * map.height);
    map.traversal_cost.resize(map.occupancy.size());
    for (size_t i = 0; i < map.occupancy.size(); ++i) {
      const auto value = source->getCharMap()[i];
      map.occupancy[i] = value == 255 ? -1 : (value >= 254 ? 100 : 0);
      map.traversal_cost[i] = value == 255 ? 254 : value;
    }
  }
  return core::prepare_map(std::move(map), {65, unknown, radius});
}
geometry_msgs::msg::PoseStamped in_frame(const geometry_msgs::msg::PoseStamped & pose,
  const std::string & frame, const std::shared_ptr<tf2_ros::Buffer> & tf)
{
  if (pose.header.frame_id == frame) {return pose;}
  return tf->transform(pose, frame, tf2::durationFromSec(0.05));
}

class TdtAstarPlanner : public nav2_core::GlobalPlanner {
  rclcpp_lifecycle::LifecycleNode::SharedPtr node_;
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_;
  std::shared_ptr<tf2_ros::Buffer> tf_;
  core::PlannerConfig limits_;
  double radius_ = 0.27;
  bool unknown_ = true;
public:
  void configure(const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent, std::string name,
    std::shared_ptr<tf2_ros::Buffer> tf, std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap) override
  {
    node_ = parent.lock(); tf_ = tf; costmap_ = costmap;
    radius_ = parameter(node_, name + ".robot_radius", 0.27);
    unknown_ = parameter(node_, name + ".unknown_is_obstacle", true);
    limits_.timeout_seconds = parameter(node_, name + ".timeout", 0.5);
    limits_.validate();
    if (!std::isfinite(radius_) || radius_ < 0) {throw std::invalid_argument("invalid robot radius");}
  }
  void cleanup() override {costmap_.reset(); tf_.reset(); node_.reset();}
  void activate() override {}
  void deactivate() override {}
  nav_msgs::msg::Path createPlan(const geometry_msgs::msg::PoseStamped & start,
    const geometry_msgs::msg::PoseStamped & goal) override
  {
    auto map = snapshot(costmap_, radius_, unknown_);
    const auto a = in_frame(start, map.source.frame, tf_), b = in_frame(goal, map.source.frame, tf_);
    core::PlanRequest request;
    request.start = {a.pose.position.x, a.pose.position.y}; request.goal = {b.pose.position.x, b.pose.position.y};
    const auto result = core::astar_seed(map, request, limits_);
    if (!result.success) {throw nav2_core::PlannerException("TDT A*: " + result.reason);}
    nav_msgs::msg::Path path; path.header.frame_id = map.source.frame; path.header.stamp = node_->now();
    for (const auto & p : result.points) {
      geometry_msgs::msg::PoseStamped pose; pose.header = path.header;
      pose.pose.position.x = p.position.x(); pose.pose.position.y = p.position.y();
      pose.pose.orientation = a.pose.orientation; path.poses.push_back(pose);
    }
    // Translation is independent of final body yaw.
    return path;
  }
};

class SnapMpcController : public RuntimeController {
  using Clock = std::chrono::steady_clock;
  struct Job {
    core::PreparedMap map;
    core::PlanRequest request;
    uint64_t revision;
    double speed_limit;
    rclcpp::Time start_time;
    std::vector<Eigen::Vector2d> via;
    bool final_target = false;
    Clock::time_point submitted_at = Clock::now();
  };
  struct Candidate {
    TimedTrajectory trajectory;
    rclcpp::Time start_time;
    uint64_t revision;
    bool final_target = false;
    Clock::time_point submitted_at;
    double progress = 0; // Furthest matched time; bounds cumulative rollback.
    TrajectoryStats stats;
  };
  rclcpp_lifecycle::LifecycleNode::SharedPtr node_;
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_;
  std::shared_ptr<tf2_ros::Buffer> tf_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp_lifecycle::LifecyclePublisher<nav_msgs::msg::Path>::SharedPtr path_pub_, astar_pub_;
  rclcpp_lifecycle::LifecyclePublisher<nav_msgs::msg::OccupancyGrid>::SharedPtr collision_pub_;
  rclcpp_lifecycle::LifecyclePublisher<visualization_msgs::msg::MarkerArray>::SharedPtr corridor_pub_;
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr timing_pub_, stats_pub_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread worker_;
  std::atomic<bool> stop_{true}, active_{false}, planning_busy_{false};
  std::atomic<uint64_t> revision_{0};
  nav_msgs::msg::Path route_;
  using OrderedRoute = sentry_nav_interfaces::srv::OrderedRoute;
  struct Binding {
    nav_msgs::msg::Path path;
    size_t first_index;
    std::vector<size_t> indices;
  };
  rclcpp::Service<OrderedRoute>::SharedPtr ordered_service_;
  std::string ordered_task_;
  std::vector<geometry_msgs::msg::PoseStamped> ordered_goals_;
  size_t next_waypoint_ = 0, route_first_waypoint_ = 0;
  std::vector<size_t> route_waypoint_indices_;
  std::optional<Binding> binding_;
  bool ordered_bound_ = false;
  bool route_update_pending_ = false;
  bool deviation_replan_ = false;
  double waypoint_pass_tolerance_ = 0.7;
  std::optional<Job> pending_;
  std::optional<Candidate> candidate_, trajectory_;
  nav_msgs::msg::Odometry::ConstSharedPtr latest_odom_;
  std::optional<rclcpp::Time> last_control_;
  std::optional<rclcpp::Time> last_map_display_;
  Eigen::Vector2d previous_odom_ = Eigen::Vector2d::Zero();
  core::PlannerConfig limits_;
  SnapConfig snap_;
  MpcConfig mpc_config_;
  std::unique_ptr<Mpc> mpc_;
  double snap_radius_ = 0.27;
  double radius_ = 0.27, lookahead_ = 2.0, replan_period_ = 0.1, max_candidate_age_ = 0.3;
  double odom_timeout_ = 0.2;
  double spin_yaw_prediction_gain_ = 0.0, spin_yaw_extra_delay_ = 0.0;
  double urgent_replan_period_ = 0.1, safety_horizon_ = 1.0, remaining_replan_time_ = 0.8;
  double replan_position_error_ = 0.5, replan_velocity_error_ = 1.0;
  double handover_position_error_ = 0.35, handover_velocity_error_ = 1.0;
  double similar_position_ = 0.08, similar_velocity_ = 0.2;
  double speed_limit_ = 0.6;
  double local_goal_speed_ratio_ = 0.5;
  double goal_settle_speed_ = 0.12;
  GoalSettler goal_settler_;
  bool unknown_ = true;
  Clock::time_point last_job_{};
  std::string reason_ = "waiting_for_plan";
  std::string planning_reason_ = "idle", last_mpc_failure_ = "none", last_failure_kind_;
  Clock::time_point last_failure_log_{};
  bool waypoints_passed() const {
    return ordered_bound_ && next_waypoint_ + 1 >= ordered_goals_.size();
  }
  void ordered_request(const OrderedRoute::Request::SharedPtr request,
    OrderedRoute::Response::SharedPtr response)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
      if (request->task_id.empty()) {throw std::invalid_argument("empty task identity");}
      if (request->operation == OrderedRoute::Request::QUERY) {
        if (request->goals.empty()) {throw std::invalid_argument("empty waypoint list");}
        for (const auto & goal : request->goals) {
          if (goal.header.frame_id.empty() || !std::isfinite(goal.pose.position.x) ||
            !std::isfinite(goal.pose.position.y)) {throw std::invalid_argument("invalid waypoint");}
        }
        if (ordered_task_ != request->task_id) {
          ordered_task_ = request->task_id; ordered_goals_ = request->goals;
          next_waypoint_ = 0; ordered_bound_ = false; binding_.reset();
          ++revision_; pending_.reset(); candidate_.reset(); clear_trajectory("new_ordered_task");
          goal_settler_.reset(); last_job_ = {}; mpc_->reset();
        } else if (ordered_goals_ != request->goals) {
          throw std::invalid_argument("waypoints changed without a new task identity");
        }
      } else if (request->operation == OrderedRoute::Request::BIND) {
        if (request->task_id != ordered_task_ || request->first_index > next_waypoint_ ||
          request->first_index >= ordered_goals_.size() || request->path.poses.empty() ||
          request->path.header.frame_id.empty()) {throw std::invalid_argument("stale or invalid ordered path");}
        std::vector<Eigen::Vector2d> path, goals;
        for (const auto & item : request->path.poses) {
          const Eigen::Vector2d p(item.pose.position.x, item.pose.position.y);
          if (!p.allFinite()) {throw std::invalid_argument("nonfinite path");}
          path.push_back(p);
        }
        for (size_t i = request->first_index; i < ordered_goals_.size(); ++i) {
          auto goal = ordered_goals_[i]; goal.header.stamp = builtin_interfaces::msg::Time();
          goal = in_frame(goal, request->path.header.frame_id, tf_);
          goals.emplace_back(goal.pose.position.x, goal.pose.position.y);
        }
        binding_ = Binding{request->path, request->first_index, ordered::waypoint_indices(path, goals)};
      } else {throw std::invalid_argument("unknown ordered route operation");}
      response->first_index = next_waypoint_;
      response->remaining_goals.assign(ordered_goals_.begin() + next_waypoint_, ordered_goals_.end());
      response->success = true;
    } catch (const std::exception & e) {response->reason = e.what();}
  }
  void update_waypoints(const Eigen::Vector2d & p, const std::string & frame) {
    if (!ordered_bound_) {return;}
    const size_t before = next_waypoint_;
    while (next_waypoint_ + 1 < ordered_goals_.size()) {
      auto goal = ordered_goals_[next_waypoint_]; goal.header.stamp = builtin_interfaces::msg::Time();
      goal = in_frame(goal, frame, tf_);
      if ((p - Eigen::Vector2d(goal.pose.position.x, goal.pose.position.y)).norm() > waypoint_pass_tolerance_) {break;}
      RCLCPP_INFO(node_->get_logger(), "Waypoint passed: index=%zu/%zu tolerance=%.3fm; continuing motion",
        next_waypoint_ + 1, ordered_goals_.size(), waypoint_pass_tolerance_);
      ++next_waypoint_;
    }
    if (before != next_waypoint_) {
      // Keep executing the continuous trajectory, but do not accept a delayed
      // candidate that would take us back to an already passed waypoint.
      ++revision_; pending_.reset(); candidate_.reset(); last_job_ = {};
    }
  }
  void clear_trajectory(const char * cause) {
    if (trajectory_) {
      const double progress = trajectory_->progress;
      RCLCPP_WARN(node_->get_logger(),
        "Trajectory cleared: cause=%s last_control_reason=%s final=%d progress=%.3fs remaining=%.3fs",
        cause, reason_.c_str(), trajectory_->final_target, progress, trajectory_->trajectory.duration() - progress);
    }
    trajectory_.reset();
  }
  void report_mpc_failure(const MpcResult & result, double odom_age) {
    last_mpc_failure_ = result.reason;
    const auto kind = result.reason.substr(0, result.reason.find(':'));
    const auto steady = Clock::now();
    if (kind != last_failure_kind_ || std::chrono::duration<double>(steady - last_failure_log_).count() >= 2.0) {
      RCLCPP_WARN(node_->get_logger(), "MPC failure: %s; %s odom_age=%.6fs",
        result.reason.c_str(), result.diagnostics.c_str(), odom_age);
      last_failure_kind_ = kind; last_failure_log_ = steady;
    }
  }
  void report_mpc_recovery(const MpcResult & result) {
    if (result.first_command_rate_relaxation <= 0) {return;}
    RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
      "MPC disturbance recovery: first_command_rate_relaxation=%.3fm/s^2; "
      "previous_odom_cmd=(%.3f,%.3f) new_cmd=(%.3f,%.3f); continuing trajectory",
      result.first_command_rate_relaxation, previous_odom_.x(), previous_odom_.y(),
      result.command.x(), result.command.y());
  }
  void publish_corridors(const SnapResult & result, const std::string & frame) {
    using Marker = visualization_msgs::msg::Marker;
    visualization_msgs::msg::MarkerArray debug;
    Marker clear;
    clear.header.frame_id = frame; clear.header.stamp = node_->now();
    clear.action = Marker::DELETEALL;
    debug.markers.push_back(clear);
    if (!result.corridors.empty()) {
      Marker boxes;
      boxes.header = clear.header; boxes.ns = "snap_corridor"; boxes.id = 0;
      boxes.type = Marker::LINE_LIST; boxes.action = Marker::ADD;
      boxes.pose.orientation.w = 1; boxes.scale.x = 0.02;
      boxes.color.r = result.success ? 0.2f : 1.0f;
      boxes.color.g = result.success ? 1.0f : 0.2f;
      boxes.color.b = 0.2f; boxes.color.a = 0.9f;
      Marker fixed = boxes;
      fixed.id = 1; fixed.type = Marker::SPHERE_LIST;
      fixed.scale.x = fixed.scale.y = fixed.scale.z = 0.07;
      for (const auto & box : result.corridors) {
        if (!std::all_of(box.begin(), box.end(), [](float v) {return std::isfinite(v);}) ||
          box[0] > box[2] || box[1] > box[3]) {continue;}
        geometry_msgs::msg::Point corners[4];
        for (int i = 0; i < 4; ++i) {
          corners[i].x = (i == 0 || i == 3) ? box[0] : box[2];
          corners[i].y = i < 2 ? box[1] : box[3];
          corners[i].z = 0.06;
        }
        if (box[0] == box[2] && box[1] == box[3]) {
          fixed.points.push_back(corners[0]);
        } else {
          for (int i = 0; i < 4; ++i) {
            boxes.points.push_back(corners[i]); boxes.points.push_back(corners[(i + 1) % 4]);
          }
        }
      }
      if (!boxes.points.empty()) {debug.markers.push_back(std::move(boxes));}
      if (!fixed.points.empty()) {debug.markers.push_back(std::move(fixed));}
    }
    corridor_pub_->publish(debug);
  }
  void work() {
    struct TimingStats {
      size_t count = 0;
      double sum = 0, maximum = 0;
      void add(double ms) {++count; sum += ms; maximum = std::max(maximum, ms);}
      double mean() const {return count ? sum / count : 0;}
    };
    TimingStats astar_stats, snap_stats, compute_stats, ready_stats;
    size_t ready_count = 0;
    auto last_timing_log = Clock::now();
    while (!stop_) {
      std::optional<Job> job;
      {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]() {return stop_ || pending_.has_value();});
        if (stop_) {return;} job = std::move(pending_); pending_.reset();
        planning_busy_ = true;
      }
      // Clear busy on success, cancellation, early continue and exceptions.
      struct BusyGuard {
        std::atomic<bool> & busy;
        ~BusyGuard() {busy = false;}
      } guard{planning_busy_};
      const auto started_at = Clock::now();
      const auto canceled = [&]() {return stop_ || !active_ || revision_.load() != job->revision ||
        std::chrono::duration<double>(Clock::now() - job->submitted_at).count() > max_candidate_age_;};
      auto limits = limits_;
      limits.max_speed = std::min(limits.max_speed, job->speed_limit);
      limits.max_axis_velocity = limits.max_speed;
      try {
        auto seed = ordered::seed(job->map, job->request, limits, job->via, canceled);
        const double astar_ms = std::chrono::duration<double, std::milli>(Clock::now() - started_at).count();
        {
          std::lock_guard<std::mutex> lock(mutex_);
          if (canceled()) {continue;}
          // Publish even when TDT MinimumSnap later fails; an empty path clears a failed seed.
          nav_msgs::msg::Path debug;
          debug.header.frame_id = job->map.source.frame; debug.header.stamp = node_->now();
          if (seed.success) {
            for (const auto & point : seed.points) {
              geometry_msgs::msg::PoseStamped pose; pose.header = debug.header;
              pose.pose.position.x = point.position.x(); pose.pose.position.y = point.position.y();
              pose.pose.orientation.w = 1; debug.poses.push_back(pose);
            }
          }
          astar_pub_->publish(debug);
          publish_corridors({}, job->map.source.frame);
        }
        SnapResult result;
        double snap_ms = -1;
        if (seed.success) {
          auto snap = snap_;
          // Both transit and final endpoints have explicit velocity targets.
          snap.constrain_goal_velocity = true;
          // Rebuild from the same raw snapshot, never inflate the A* mask again.
          // Only TDT MinimumSnap sees this radius; A* and target selection keep job->map.
          if (snap_radius_ == radius_) {
            result = optimize_snap(job->map, job->request, limits, seed, snap, canceled);
          } else {
            const auto snap_map = core::prepare_map(job->map.source, {65, unknown_, snap_radius_});
            result = optimize_snap(snap_map, job->request, limits, seed, snap, canceled);
          }
          snap_ms = result.elapsed_ms;
        }
        const bool candidate_ready = result.success && !result.segments.empty();
        TimedTrajectory planned{std::move(result.segments)};
        const auto stats = candidate_ready ? planned.stats() : TrajectoryStats{};
        const auto computed_at = Clock::now();
        std::lock_guard<std::mutex> lock(mutex_);
        if (canceled()) {continue;}
        publish_corridors(result, job->map.source.frame);
        if (candidate_ready) {
          const bool final_target = job->final_target;
          candidate_ = Candidate{std::move(planned), job->start_time, job->revision, final_target, job->submitted_at, 0, stats};
          planning_reason_ = "candidate_ready";
        } else {
          planning_reason_ = seed.success ? "TDT MinimumSnap: " + result.reason : "A*: " + seed.reason;
          RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
            "%s; start=(%.3f,%.3f) goal=(%.3f,%.3f) start_v=(%.3f,%.3f)", planning_reason_.c_str(),
            job->request.start.x(), job->request.start.y(), job->request.goal.x(), job->request.goal.y(),
            job->request.velocity.x(), job->request.velocity.y());
        }
        const auto ready_at = Clock::now();
        const double compute_ms = std::chrono::duration<double, std::milli>(computed_at - started_at).count();
        const double ready_ms = std::chrono::duration<double, std::milli>(ready_at - job->submitted_at).count();
        astar_stats.add(astar_ms);
        if (snap_ms >= 0) {snap_stats.add(snap_ms);}
        compute_stats.add(compute_ms); ready_stats.add(ready_ms);
        if (candidate_ready) {++ready_count;}
        std::ostringstream timing;
        timing << std::fixed << std::setprecision(3)
          << "Local planning sample: state=" << planning_reason_
          << " astar_ms=" << astar_ms << " snap_ms=" << snap_ms
          << " compute_ms=" << compute_ms << " ready_ms=" << ready_ms;
        if (compute_stats.count == 1 ||
          std::chrono::duration<double>(ready_at - last_timing_log).count() >= 5.0) {
          timing << "\nLocal planning timing: samples=" << compute_stats.count
            << " candidates=" << ready_count << " failed=" << compute_stats.count - ready_count
            << " snap_samples=" << snap_stats.count
            << " astar_ms(avg/max)=" << astar_stats.mean() << "/" << astar_stats.maximum
            << " snap_ms(avg/max)=" << snap_stats.mean() << "/" << snap_stats.maximum
            << " compute_ms(avg/max)=" << compute_stats.mean() << "/" << compute_stats.maximum
            << " ready_ms(avg/max)=" << ready_stats.mean() << "/" << ready_stats.maximum;
          last_timing_log = ready_at;
        }
        std_msgs::msg::String timing_message; timing_message.data = timing.str();
        timing_pub_->publish(timing_message);
      } catch (const std::exception & e) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (canceled()) {continue;}
        publish_corridors({}, job->map.source.frame);
        planning_reason_ = e.what();
        RCLCPP_ERROR_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000, "Planning: %s", e.what());
      }
    }
  }
  // Nonzero commands and history are in the local costmap frame (odom).
  // Zero commands may be issued without a valid orientation.
  geometry_msgs::msg::TwistStamped command(const Eigen::Vector2d & odom, double yaw = 0) {
    previous_odom_ = odom;
    const Eigen::Vector2d body = Eigen::Rotation2Dd(-yaw) * odom;
    geometry_msgs::msg::TwistStamped out;
    out.header.frame_id = costmap_->getBaseFrameID(); out.header.stamp = node_->now();
    out.twist.linear.x = body.x(); out.twist.linear.y = body.y(); out.twist.angular.z = 0;
    return out;
  }
  geometry_msgs::msg::TwistStamped brake(const Eigen::Vector2d & p, double yaw,
    const Eigen::Vector2d & velocity, const core::PreparedMap & map)
  {
    mpc_->reset();
    const double decrease = mpc_config_.max_acceleration * mpc_config_.control_dt;
    Eigen::Vector2d cmd = previous_odom_;
    if (cmd.norm() <= decrease) {cmd.setZero();} else {cmd *= 1 - decrease / cmd.norm();}
    // Reject controlled braking if its nominal stopping segment is blocked.
    const Eigen::Vector2d end = p + velocity *
      (std::max(mpc_config_.response_x, mpc_config_.response_y) + velocity.norm() / (2 * mpc_config_.max_acceleration));
    if (!core::line_free(map, p, end)) {cmd.setZero();}
    return command(cmd, yaw);
  }
public:
  void stopMotion() override {
    std::lock_guard<std::mutex> lock(mutex_);
    ++revision_; pending_.reset(); candidate_.reset(); clear_trajectory("runtime_stop");
    previous_odom_.setZero(); last_control_.reset(); last_job_ = {};
    goal_settler_.reset(); mpc_->reset(); deviation_replan_ = false;
    if (active_) {
      nav_msgs::msg::Path empty; empty.header.frame_id = costmap_->getGlobalFrameID();
      empty.header.stamp = node_->now(); path_pub_->publish(empty); astar_pub_->publish(empty);
      publish_corridors({}, empty.header.frame_id);
    }
  }
  void resetTask() override {
    stopMotion();
    std::lock_guard<std::mutex> lock(mutex_);
    route_ = nav_msgs::msg::Path{}; ordered_task_.clear(); ordered_goals_.clear(); binding_.reset();
    next_waypoint_ = route_first_waypoint_ = 0; route_waypoint_indices_.clear();
    ordered_bound_ = false; route_update_pending_ = false;
  }
  OrderedRoute::Response orderedRequest(const OrderedRoute::Request & request) override {
    auto response = std::make_shared<OrderedRoute::Response>();
    ordered_request(std::make_shared<OrderedRoute::Request>(request), response);
    return *response;
  }
  nav_msgs::msg::Odometry::ConstSharedPtr odometry() override {
    std::lock_guard<std::mutex> lock(mutex_); return latest_odom_;
  }
  ~SnapMpcController() override {stop_ = true; cv_.notify_all(); if (worker_.joinable()) {worker_.join();}}
  void configure(const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent, std::string name,
    std::shared_ptr<tf2_ros::Buffer> tf, std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap) override
  {
    node_ = parent.lock(); tf_ = tf; costmap_ = costmap;
    auto param = [&](const std::string & key, double value) {return parameter(node_, name + "." + key, value);};
    radius_ = param("robot_radius", 0.27);
    snap_radius_ = param("snap_robot_radius", radius_);
    lookahead_ = param("lookahead", 2.0);
    waypoint_pass_tolerance_ = param("waypoint_pass_tolerance", 0.7);
    replan_period_ = param("replan_period", 0.1);
    max_candidate_age_ = param("max_candidate_age", 0.3);
    odom_timeout_ = param("odom_timeout", 0.2);
    urgent_replan_period_ = param("urgent_replan_period", 0.1);
    safety_horizon_ = param("safety_horizon", 1.0);
    remaining_replan_time_ = param("remaining_replan_time", 0.8);
    replan_position_error_ = param("replan_position_error", 0.5);
    replan_velocity_error_ = param("replan_velocity_error", 1.0);
    handover_position_error_ = param("handover_position_error", 0.35);
    handover_velocity_error_ = param("handover_velocity_error", 1.0);
    similar_position_ = param("similar_position", 0.08);
    similar_velocity_ = param("similar_velocity", 0.2);
    goal_settle_speed_ = param("goal_settle_speed", 0.12);
    local_goal_speed_ratio_ = param("local_goal_speed_ratio", 0.5);
    unknown_ = parameter(node_, name + ".unknown_is_obstacle", true);
    mpc_config_.max_speed = param("max_speed", 0.6);
    mpc_config_.max_acceleration = param("max_acceleration", 0.8);
    mpc_config_.response_x = param("response_x", 0.15); mpc_config_.response_y = param("response_y", 0.15);
    mpc_config_.spin_model_gain = param("spin_model_gain", 0.0);
    spin_yaw_prediction_gain_ = param("spin_yaw_prediction_gain", 0.0);
    spin_yaw_extra_delay_ = param("spin_yaw_extra_delay", 0.0);
    if (!std::isfinite(spin_yaw_prediction_gain_) || spin_yaw_prediction_gain_ < 0 || spin_yaw_prediction_gain_ > 2 ||
      !std::isfinite(spin_yaw_extra_delay_) || spin_yaw_extra_delay_ < 0 || spin_yaw_extra_delay_ > 0.2)
    {throw std::invalid_argument("invalid spin yaw prediction parameters");}
    mpc_config_.horizon = parameter(node_, name + ".horizon", 40);
    mpc_config_.dt = param("model_dt", 0.025);
    mpc_config_.control_dt = 1 / parameter(node_, "controller_frequency", 40.0);
    mpc_config_.timeout = param("mpc_timeout", 0.015);
    limits_.max_speed = limits_.max_axis_velocity = mpc_config_.max_speed;
    limits_.max_acceleration = limits_.max_axis_acceleration = mpc_config_.max_acceleration;
    limits_.timeout_seconds = param("astar_timeout", 0.10);
    snap_.timeout_seconds = param("snap_timeout", 0.20);
    snap_.corridor_range = param("snap_corridor_range", 0.4);
    snap_.collision_iterations = parameter(node_, name + ".snap_collision_iterations", 6);
    limits_.validate(); snap_.validate(); mpc_config_.validate();
    for (double v : {lookahead_, replan_period_, max_candidate_age_, odom_timeout_, goal_settle_speed_, waypoint_pass_tolerance_,
      urgent_replan_period_, safety_horizon_, remaining_replan_time_, replan_position_error_,
      replan_velocity_error_, handover_position_error_, handover_velocity_error_, similar_position_, similar_velocity_}) {
      if (!std::isfinite(v) || v <= 0) {throw std::invalid_argument("controller parameters must be positive");}
    }
    if (!std::isfinite(local_goal_speed_ratio_) || local_goal_speed_ratio_ <= 0 || local_goal_speed_ratio_ > 1)
    {throw std::invalid_argument("local_goal_speed_ratio must be in (0, 1]");}
    if (!std::isfinite(radius_) || radius_ < 0 || !std::isfinite(snap_radius_) || snap_radius_ < 0)
    {throw std::invalid_argument("invalid collision radius");}
    RCLCPP_INFO(node_->get_logger(), "Collision radii: local_astar=%.3fm snap=%.3fm", radius_, snap_radius_);
    speed_limit_ = mpc_config_.max_speed; mpc_ = std::make_unique<Mpc>(mpc_config_);
    if (parameter(node_, "enable_ordered_route_service", true)) {
      ordered_service_ = node_->create_service<OrderedRoute>(name + "/ordered_route",
        [this](OrderedRoute::Request::SharedPtr request, OrderedRoute::Response::SharedPtr response) {
          ordered_request(request, response);
        });
    }
    const auto odom_topic = parameter(node_, "odom_topic", std::string("chassis_odom"));
    odom_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(odom_topic, rclcpp::SensorDataQoS(),
      [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        latest_odom_ = msg;
      });
    astar_pub_ = node_->create_publisher<nav_msgs::msg::Path>(name + "/astar_path", rclcpp::QoS(1).transient_local());
    path_pub_ = node_->create_publisher<nav_msgs::msg::Path>(name + "/snap_path", 1);
    collision_pub_ = node_->create_publisher<nav_msgs::msg::OccupancyGrid>(name + "/collision_map", rclcpp::QoS(1).transient_local());
    corridor_pub_ = node_->create_publisher<visualization_msgs::msg::MarkerArray>(name + "/snap_corridors", rclcpp::QoS(1).transient_local());
    timing_pub_ = node_->create_publisher<std_msgs::msg::String>(name + "/planning_timing", 10);
    stats_pub_ = node_->create_publisher<std_msgs::msg::String>(name + "/trajectory_stats", 10);
  }
  void activate() override {
    std::lock_guard<std::mutex> lock(mutex_);
    stop_ = false; active_ = true; goal_settler_.reset(); previous_odom_.setZero(); last_control_.reset(); last_job_ = {};
    path_pub_->on_activate(); astar_pub_->on_activate(); collision_pub_->on_activate(); last_map_display_.reset();
    corridor_pub_->on_activate(); publish_corridors({}, costmap_->getGlobalFrameID());
    timing_pub_->on_activate(); stats_pub_->on_activate();
    worker_ = std::thread([this]() {work();});
  }
  void deactivate() override {
    {std::lock_guard<std::mutex> lock(mutex_); active_ = false; stop_ = true; ++revision_;
      publish_corridors({}, costmap_->getGlobalFrameID());
      pending_.reset(); candidate_.reset(); clear_trajectory("deactivated"); previous_odom_.setZero();}
    cv_.notify_all(); if (worker_.joinable()) {worker_.join();}
    path_pub_->on_deactivate(); astar_pub_->on_deactivate(); collision_pub_->on_deactivate();
    corridor_pub_->on_deactivate(); timing_pub_->on_deactivate(); stats_pub_->on_deactivate();
  }
  void cleanup() override {latest_odom_.reset(); ordered_service_.reset(); odom_sub_.reset(); path_pub_.reset(); astar_pub_.reset(); collision_pub_.reset(); corridor_pub_.reset(); timing_pub_.reset(); stats_pub_.reset(); mpc_.reset(); costmap_.reset(); tf_.reset(); node_.reset();}
  void setPlan(const nav_msgs::msg::Path & path) override {
    std::lock_guard<std::mutex> lock(mutex_);
    const bool new_goal = route_.poses.empty() || path.poses.empty() ||
      route_.header.frame_id != path.header.frame_id ||
      std::hypot(route_.poses.back().pose.position.x - path.poses.back().pose.position.x,
      route_.poses.back().pose.position.y - path.poses.back().pose.position.y) > 0.05;
    const bool restarting = last_control_ && (node_->now() - *last_control_).seconds() > 0.2;
    const bool was_ordered = ordered_bound_;
    ordered_bound_ = binding_ && binding_->path == path;
    if (ordered_bound_) {
      route_first_waypoint_ = binding_->first_index; route_waypoint_indices_ = binding_->indices;
    } else {
      ordered_task_.clear(); ordered_goals_.clear(); next_waypoint_ = 0;
      binding_.reset(); route_waypoint_indices_.clear();
    }
    // Ignore ordinary path truncation, but notice a materially changed route.
    bool changed_geometry = false;
    if (!new_goal && route_.poses.size() >= 2 && path.poses.size() >= 2) {
      for (const auto & pose : path.poses) {
        const Eigen::Vector2d point(pose.pose.position.x, pose.pose.position.y);
        double best = std::numeric_limits<double>::infinity();
        for (size_t i = 1; i < route_.poses.size(); ++i) {
          const Eigen::Vector2d a(route_.poses[i-1].pose.position.x, route_.poses[i-1].pose.position.y);
          const Eigen::Vector2d b(route_.poses[i].pose.position.x, route_.poses[i].pose.position.y);
          const Eigen::Vector2d d = b-a;
          const double f = std::clamp((point-a).dot(d) / std::max(d.squaredNorm(), 1e-12), 0.0, 1.0);
          best = std::min(best, (point-a-f*d).norm());
        }
        if (best > 0.35) {changed_geometry = true; break;}
      }
    }
    route_ = path;
    if (changed_geometry && !new_goal) {
      ++revision_; pending_.reset(); candidate_.reset(); route_update_pending_ = true;
    }
    // Periodic global route refreshes must not cancel an otherwise valid local
    // handover. The local planner uses the latest collision map.
    if (new_goal || restarting || was_ordered != ordered_bound_) {
      goal_settler_.reset(); route_update_pending_ = false;
      ++revision_; pending_.reset(); candidate_.reset(); clear_trajectory(new_goal ? "new_goal" : "controller_restarted");
      if (active_) {publish_corridors({}, costmap_->getGlobalFrameID());}
      mpc_->reset(); last_job_ = {}; reason_ = "waiting_for_plan";
    }
  }
  void setSpeedLimit(const double & limit, const bool & percentage) override {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!std::isfinite(limit) || limit < 0) {throw std::invalid_argument("invalid speed limit");}
    // Nav2 convention: zero disables a speed limit.
    speed_limit_ = limit == 0 ? mpc_config_.max_speed :
      std::min(mpc_config_.max_speed, percentage ? mpc_config_.max_speed * limit / 100 : limit);
    goal_settler_.reset();
    ++revision_; pending_.reset(); candidate_.reset(); clear_trajectory("speed_limit_changed"); last_job_ = {}; mpc_->reset();
  }
  geometry_msgs::msg::TwistStamped computeVelocityCommands(const geometry_msgs::msg::PoseStamped & /*pose*/,
    const geometry_msgs::msg::Twist & /*velocity*/, nav2_core::GoalChecker * goal_checker) override
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto * translation_checker = dynamic_cast<TranslationGoalChecker *>(goal_checker);
    if (ordered_bound_ && !translation_checker) {
      throw nav2_core::PlannerException("ordered route requires TranslationGoalChecker");
    }
    if (translation_checker) {translation_checker->setWaypointProgress(ordered_bound_, waypoints_passed());}
    if (translation_checker && translation_checker->requiresWaypointProgress() && !ordered_bound_) {
      throw nav2_core::PlannerException("ordered goal checker requires a bound waypoint path");
    }
    const auto now = node_->now();
    // One immutable sample supplies pose, body twist and freshness. Nav2's
    // separate pose/Twist inputs need not have the same sample time.
    const auto odom = latest_odom_;
    const std::optional<rclcpp::Time> odom_stamp = odom ?
      std::optional<rclcpp::Time>(rclcpp::Time(odom->header.stamp, node_->get_clock()->get_clock_type())) : std::nullopt;
    if (!active_ || route_.poses.empty() || !odom_stamp || (now - *odom_stamp).seconds() > odom_timeout_ ||
      (now - *odom_stamp).seconds() < -0.05 || !costmap_->isCurrent())
    {
      const char * cause = !active_ ? "inactive" : route_.poses.empty() ? "empty_global_route" :
        !odom_stamp ? "no_odometry" : !costmap_->isCurrent() ? "costmap_not_current" : "odometry_timestamp_invalid";
      RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
        "Controlled stop: %s; odom_age=%.3f", cause, odom_stamp ? (now - *odom_stamp).seconds() : -1.0);
      ++revision_; pending_.reset(); clear_trajectory(cause); candidate_.reset(); last_job_ = {};
      goal_settler_.reset(); mpc_->reset(); return command(Eigen::Vector2d::Zero());
    }
    if (last_control_ && ((now - *last_control_).seconds() < 0 || (now - *last_control_).seconds() > 0.2)) {
      goal_settler_.reset();
      ++revision_; clear_trajectory("control_time_jump_or_gap"); candidate_.reset(); pending_.reset(); last_job_ = {}; previous_odom_.setZero(); mpc_->reset();
    }
    last_control_ = now;
    const auto & q = odom->pose.pose.orientation;
    const double q_norm2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (odom->header.frame_id.empty() || odom->child_frame_id != costmap_->getBaseFrameID() ||
      !std::isfinite(q_norm2) || q_norm2 < 1e-12 ||
      !std::isfinite(odom->twist.twist.angular.z)) {
      RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
        "Controlled stop: invalid_odometry_frame_or_orientation_or_angular_velocity");
      return command(Eigen::Vector2d::Zero());
    }
    geometry_msgs::msg::PoseStamped odom_pose;
    odom_pose.header = odom->header; odom_pose.pose = odom->pose.pose;
    const double q_norm = std::sqrt(q_norm2);
    odom_pose.pose.orientation.x /= q_norm; odom_pose.pose.orientation.y /= q_norm;
    odom_pose.pose.orientation.z /= q_norm; odom_pose.pose.orientation.w /= q_norm;
    geometry_msgs::msg::PoseStamped local_pose;
    core::PreparedMap map;
    // Preserve the odometry stamp when a frame transform is necessary.
    try {local_pose = in_frame(odom_pose, costmap_->getGlobalFrameID(), tf_); map = snapshot(costmap_, radius_, unknown_);}
    catch (const std::exception & e) {RCLCPP_WARN(node_->get_logger(), "%s", e.what()); return command(Eigen::Vector2d::Zero());}
    if (!last_map_display_ || (now - *last_map_display_).seconds() >= 0.5 || now < *last_map_display_) {
      nav_msgs::msg::OccupancyGrid debug; debug.header.frame_id = map.source.frame; debug.header.stamp = now;
      debug.info.width = map.source.width; debug.info.height = map.source.height;
      debug.info.resolution = map.source.resolution; debug.info.origin.position.x = map.source.origin.x();
      debug.info.origin.position.y = map.source.origin.y(); debug.info.origin.orientation.w = 1;
      debug.data.resize(map.blocked.size());
      for (size_t i = 0; i < map.blocked.size(); ++i) {debug.data[i] = map.blocked[i] ? 100 : 0;}
      collision_pub_->publish(debug);
      last_map_display_ = now;
    }
    const Eigen::Vector2d p(local_pose.pose.position.x, local_pose.pose.position.y);
    const Eigen::Vector2d body_velocity(odom->twist.twist.linear.x, odom->twist.twist.linear.y);
    const double yaw = tf2::getYaw(local_pose.pose.orientation);
    const Eigen::Matrix2d rotation = Eigen::Rotation2Dd(yaw).toRotationMatrix();
    const Eigen::Vector2d v = rotation * body_velocity;
    if (!p.allFinite() || !v.allFinite() || !std::isfinite(yaw)) {return command(Eigen::Vector2d::Zero());}
    try {update_waypoints(p, map.source.frame);}
    catch (const std::exception & e) {
      RCLCPP_WARN(node_->get_logger(), "Waypoint transform: %s", e.what()); return brake(p, yaw, v, map);
    }
    if (translation_checker) {translation_checker->setWaypointProgress(ordered_bound_, waypoints_passed());}
    geometry_msgs::msg::Pose tolerance;
    geometry_msgs::msg::Twist velocity_tolerance;
    if ((!ordered_bound_ || waypoints_passed()) && goal_checker && goal_checker->getTolerances(tolerance, velocity_tolerance)) {
      try {
        auto goal_pose = route_.poses.back(); goal_pose.header = route_.header;
        const auto goal = in_frame(goal_pose, map.source.frame, tf_);
        const Eigen::Vector2d goal_position(goal.pose.position.x, goal.pose.position.y);
        const double distance = (goal_position - p).norm();
        const double speed = std::max(v.norm(), previous_odom_.norm());
        const bool was_settling = goal_settler_.active();
        double stop_distance = 0;
        bool safe_to_stop = false;
        if (!was_settling && speed <= goal_settle_speed_ && distance <= tolerance.position.x) {
          const double stop_time = std::max(mpc_config_.response_x, mpc_config_.response_y) +
            speed / (2 * mpc_config_.max_acceleration);
          stop_distance = speed * stop_time;
          if (distance + stop_distance <= tolerance.position.x) {
            safe_to_stop = core::line_free(map, p, goal_position) &&
              core::line_free(map, p, p + v * stop_time);
          }
        }
        if (goal_settler_.update(distance, tolerance.position.x, v.norm(), previous_odom_.norm(),
          stop_distance, goal_settle_speed_, safe_to_stop)) {
          if (!was_settling) {
            ++revision_; pending_.reset(); candidate_.reset(); clear_trajectory("goal_settling"); mpc_->reset();
            RCLCPP_INFO(node_->get_logger(),
              "Goal settling: distance=%.3fm tolerance=%.3fm speed=%.3fm/s; holding zero",
              distance, tolerance.position.x, v.norm());
          }
          reason_ = "goal_settling";
          return command(Eigen::Vector2d::Zero());
        }
        if (was_settling) {last_job_ = {}; reason_ = "goal_settling_released";}
      } catch (const std::exception & e) {
        RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
          "Goal settling transform failed: %s", e.what());
        return brake(p, yaw, v, map);
      }
    } else {goal_settler_.reset();}
    double old_time = 0.0;
    bool old_safe = false, urgent_replan = !trajectory_ || route_update_pending_;
    if (trajectory_) {
      old_time = trajectory_->trajectory.nearest_time(p,
        trajectory_->progress - 0.2, trajectory_->progress + 1.0);
      const auto old_reference = trajectory_->trajectory.sample(old_time);
      old_safe = trajectory_window_safe(trajectory_->trajectory, map, p, old_time, safety_horizon_);
      const double position_error = (old_reference.position - p).norm();
      const double velocity_error = (old_reference.velocity - v).norm();
      if (deviation_replan_) {
        deviation_replan_ = position_error > 0.8 * replan_position_error_ ||
          velocity_error > 0.8 * replan_velocity_error_;
      } else {
        deviation_replan_ = position_error > replan_position_error_ || velocity_error > replan_velocity_error_;
      }
      urgent_replan = route_update_pending_ || !old_safe || deviation_replan_ ||
        (!trajectory_->final_target && trajectory_->trajectory.duration() - old_time < remaining_replan_time_);
    } else {deviation_replan_ = false;}
    if (candidate_) {
      const double elapsed = (now - candidate_->start_time).seconds();
      const double age = std::chrono::duration<double>(Clock::now() - candidate_->submitted_at).count();
      const bool fresh = candidate_->revision == revision_ &&
        handover_ready(candidate_->trajectory, elapsed, age, max_candidate_age_);
      const double candidate_time = fresh ? candidate_->trajectory.nearest_time(p, 0, candidate_->trajectory.duration()) : 0;
      const bool valid = fresh && candidate_state_valid(candidate_->trajectory, candidate_time, p, v,
        handover_position_error_, handover_velocity_error_, speed_limit_, mpc_config_.max_acceleration, safety_horizon_) &&
        trajectory_window_safe(candidate_->trajectory, map, p, candidate_time, safety_horizon_);
      const bool similar = valid && trajectory_ && old_safe && !urgent_replan &&
        trajectory_->final_target == candidate_->final_target &&
        similar_trajectories(trajectory_->trajectory, old_time, candidate_->trajectory, candidate_time,
          0.5, similar_position_, similar_velocity_);
      if (valid && !similar) {
        candidate_->progress = candidate_time;
        std::ostringstream accepted;
        const auto & stats = candidate_->stats;
        accepted << std::fixed << std::setprecision(3)
          << "Trajectory accepted: length=" << stats.length << "m duration=" << stats.duration
          << "s avg_speed=" << stats.average_speed << "m/s max_speed=" << stats.max_speed
          << "m/s snapshot_age=" << age << "s reference_time=" << candidate_->progress
          << "s final=" << candidate_->final_target;
        std_msgs::msg::String stats_message; stats_message.data = accepted.str();
        stats_pub_->publish(stats_message);
        trajectory_ = std::move(candidate_);
        // The next MPC call builds a feasible seed from the new trajectory.
        reason_ = "tracking";
        old_safe = true;
        urgent_replan = false;
        nav_msgs::msg::Path debug; debug.header.frame_id = map.source.frame; debug.header.stamp = now;
        for (double t = 0; t <= trajectory_->trajectory.duration(); t += 0.05) {
          const auto sample = trajectory_->trajectory.sample(t);
          geometry_msgs::msg::PoseStamped item; item.header = debug.header;
          item.pose.position.x = sample.position.x(); item.pose.position.y = sample.position.y();
          item.pose.orientation = local_pose.pose.orientation; debug.poses.push_back(item);
        }
        path_pub_->publish(debug);
      } else if (!similar) {
        reason_ = "handover_rejected"; last_job_ = {};
        RCLCPP_WARN(node_->get_logger(),
          "Handover rejected: revision_match=%d fresh=%d state_or_safety_valid=%d elapsed=%.6fs snapshot_age=%.6fs old_trajectory=%d",
          candidate_->revision == revision_, fresh, valid, elapsed, age, bool(trajectory_));
      }
      candidate_.reset();
    }
    const auto steady = Clock::now();
    // Keep planning until low-speed goal holding takes over above, including
    // inside position tolerance when the robot still needs to decelerate.
    if (!pending_ && !planning_busy_ && !candidate_ &&
      std::chrono::duration<double>(steady - last_job_).count() >=
        (urgent_replan ? urgent_replan_period_ : replan_period_)) {
      std::vector<Eigen::Vector2d> route;
      try {
        geometry_msgs::msg::TransformStamped transform;
        const bool convert = route_.header.frame_id != map.source.frame;
        if (convert) {transform = tf_->lookupTransform(map.source.frame, route_.header.frame_id, tf2::TimePointZero, tf2::durationFromSec(0.05));}
        for (auto item : route_.poses) {
          if (convert) {geometry_msgs::msg::PoseStamped output; tf2::doTransform(item, output, transform); item = output;}
          route.emplace_back(item.pose.position.x, item.pose.position.y);
        }
      } catch (const std::exception &) {return brake(p, yaw, v, map);}
      ordered::LocalPlan local;
      if (ordered_bound_) {
        local = ordered::local_plan(route, route_waypoint_indices_,
          next_waypoint_ - route_first_waypoint_, p, map, lookahead_);
      } else {
        const auto [target, direction] = local_target(route, p, map, lookahead_);
        local.target = target; local.direction = direction; local.final = (target - route.back()).norm() <= 0.05;
      }
      const auto & target = local.target;
      if (((target - p).norm() > 0.05 || !local.via.empty()) && !map.is_blocked(target)) {
        core::PlanRequest request;
        // Plan from measured state; adoption matches position on the returned curve.
        request.start = p; request.velocity = v; request.goal = target;
        if (!local.final) {
          request.goal_velocity = local.direction * (speed_limit_ * local_goal_speed_ratio_);
        }
        planning_reason_ = "planning";
        pending_ = Job{map, request, revision_.load(), speed_limit_, now, local.via, local.final};
        last_job_ = steady; route_update_pending_ = false; cv_.notify_one();
      }
    }
    double duration = -1.0, time = -1.0, remaining = -1.0;
    if (!trajectory_) {reason_ = "waiting_for_plan";}
    if (trajectory_ && !old_safe) {
      reason_ = "trajectory_obstructed";
      return brake(p, yaw, v, map);
    }
    if (trajectory_) {
      duration = trajectory_->trajectory.duration();
      // Stay on the local branch. A disturbance does not trigger a global search.
      time = trajectory_->trajectory.nearest_time(p,
        trajectory_->progress - 0.2, trajectory_->progress + 1.0);
      trajectory_->progress = std::max(trajectory_->progress, time);
      remaining = duration - time;
      const double omega = odom->twist.twist.angular.z;
      auto result = mpc_->solve(p, v, previous_odom_, trajectory_->trajectory, time, speed_limit_, omega);
      if (result.success) {
        const double output_age = (node_->now() - *odom_stamp).seconds();
        if (!std::isfinite(output_age) || output_age > odom_timeout_ || output_age < -0.05) {
          reason_ = "odometry_stale_at_output";
          return command(Eigen::Vector2d::Zero());
        }
        const double predicted_yaw = predict_output_yaw(yaw, omega, output_age,
          spin_yaw_extra_delay_, spin_yaw_prediction_gain_);
        report_mpc_recovery(result); reason_ = "tracking"; last_failure_kind_.clear();
        return command(result.command, predicted_yaw);
      }
      reason_ = result.reason;
      report_mpc_failure(result, (now - *odom_stamp).seconds());
    }
    if (trajectory_ && trajectory_->final_target) {
      clear_trajectory("final_trajectory_control_failed"); last_job_ = {};
    }
    RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
      "Controlled stop: %s; trajectory_duration=%.3fs reference_time=%.3fs remaining=%.3fs; "
      "planning_state=%s last_mpc_failure=%s candidate_age=%.3fs; pose=(%.3f,%.3f) measured_v_odom=(%.3f,%.3f) previous_odom_cmd=(%.3f,%.3f) odom_age=%.3f",
      reason_.c_str(), duration, time, remaining,
      planning_reason_.c_str(), last_mpc_failure_.c_str(), candidate_ ? (now - candidate_->start_time).seconds() : -1.0,
      p.x(), p.y(), v.x(), v.y(), previous_odom_.x(), previous_odom_.y(), (now - *odom_stamp).seconds());
    return brake(p, yaw, v, map);
  }
};
std::unique_ptr<RuntimeController> make_runtime_controller() {
  return std::make_unique<SnapMpcController>();
}
std::unique_ptr<nav2_core::GlobalPlanner> make_runtime_planner() {
  return std::make_unique<TdtAstarPlanner>();
}
}
PLUGINLIB_EXPORT_CLASS(sentry_nav::TdtAstarPlanner, nav2_core::GlobalPlanner)
PLUGINLIB_EXPORT_CLASS(sentry_nav::SnapMpcController, nav2_core::Controller)
