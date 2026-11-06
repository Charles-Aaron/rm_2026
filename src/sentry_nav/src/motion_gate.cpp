#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <sentry_nav_interfaces/action/rotate_to_yaw.hpp>
#include <sentry_nav_interfaces/action/visit_special_point.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <action_msgs/msg/goal_status_array.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/utils.h>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <optional>

class MotionGate : public rclcpp::Node {
  using Rotate = sentry_nav_interfaces::action::RotateToYaw;
  using Visit = sentry_nav_interfaces::action::VisitSpecialPoint;
  using Navigate = nav2_msgs::action::NavigateToPose;
  using RotateHandle = rclcpp_action::ServerGoalHandle<Rotate>;
  using VisitHandle = rclcpp_action::ServerGoalHandle<Visit>;
  struct Point {double x, y, yaw;};
  std::map<std::string, Point> points_;
  rclcpp_action::Server<Rotate>::SharedPtr rotate_server_;
  rclcpp_action::Server<Visit>::SharedPtr visit_server_;
  rclcpp_action::Client<Navigate>::SharedPtr navigation_;
  rclcpp_action::ClientGoalHandle<Navigate>::SharedPtr navigation_goal_;
  std::shared_ptr<RotateHandle> rotate_;
  std::shared_ptr<VisitHandle> visit_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr output_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr input_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_;
  rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_sub_;
  rclcpp::Subscription<action_msgs::msg::GoalStatusArray>::SharedPtr status_;
  rclcpp::Subscription<action_msgs::msg::GoalStatusArray>::SharedPtr through_status_;
  rclcpp::Subscription<action_msgs::msg::GoalStatusArray>::SharedPtr follow_status_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr parameter_guard_;
  nav_msgs::msg::OccupancyGrid::ConstSharedPtr map_;
  rclcpp::TimerBase::SharedPtr timer_;
  tf2_ros::Buffer tf_;
  tf2_ros::TransformListener listener_;
  geometry_msgs::msg::Twist latest_, measured_;
  std::optional<rclcpp::Time> input_time_, odom_time_, stable_since_, rotation_start_, task_start_;
  std::string phase_ = "IDLE";
  double target_yaw_ = 0, tolerance_ = 0.03, rotation_timeout_ = 10, last_wz_ = 0;
  double max_wz_, max_aw_, yaw_gain_, command_timeout_, odom_timeout_, map_timeout_, radius_, task_timeout_;
  std::string map_frame_, base_frame_;
  bool nav_active_ = false, through_active_ = false, follow_active_ = false, cancel_inflight_ = false;
  bool allow_translation_ = false;

  void publish(const geometry_msgs::msg::Twist & command) {output_->publish(command);}
  void zero() {publish(geometry_msgs::msg::Twist{}); last_wz_ = 0;}
  bool fresh(const std::optional<rclcpp::Time> & stamp, double timeout) {
    return stamp && (now() - *stamp).seconds() >= -0.05 && (now() - *stamp).seconds() <= timeout;
  }
  bool stopped() const {
    return std::hypot(measured_.linear.x, measured_.linear.y) < 0.04 && std::abs(measured_.angular.z) < 0.04;
  }
  void finish(bool success, const std::string & reason) {
    if (!allow_translation_) {zero(); latest_ = geometry_msgs::msg::Twist{}; input_time_.reset();}
    last_wz_ = 0;
    if (rotate_) {
      auto result = std::make_shared<Rotate::Result>(); result->success = success; result->reason = reason;
      if (rotate_->is_canceling()) {rotate_->canceled(result);}
      else if (success) {rotate_->succeed(result);} else {rotate_->abort(result);}
      rotate_.reset();
    }
    if (visit_) {
      auto result = std::make_shared<Visit::Result>(); result->success = success; result->reason = reason;
      if (visit_->is_canceling()) {visit_->canceled(result);}
      else if (success) {visit_->succeed(result);} else {visit_->abort(result);}
      visit_.reset();
    }
    phase_ = "IDLE"; allow_translation_ = false; stable_since_.reset(); rotation_start_.reset(); task_start_.reset();
  }
  void begin_rotation(double yaw, double tolerance, double timeout) {
    target_yaw_ = yaw; tolerance_ = tolerance; rotation_timeout_ = timeout;
    phase_ = allow_translation_ ? "ROTATING" : "STOPPING"; rotation_start_ = now(); stable_since_.reset();
    if (!allow_translation_) {zero();}
  }
  void cancel_navigation() {
    cancel_inflight_ = true;
    if (navigation_goal_) {navigation_->async_cancel_goal(navigation_goal_);}
  }
  bool rotation_area_safe() {
    if (!map_ || map_->info.resolution <= 0 || map_->data.size() !=
      static_cast<size_t>(map_->info.width) * map_->info.height ||
      !fresh(std::optional<rclcpp::Time>(rclcpp::Time(map_->header.stamp, get_clock()->get_clock_type())), map_timeout_)) {return false;}
    const auto transform = tf_.lookupTransform(map_->header.frame_id, base_frame_, tf2::TimePointZero);
    if (!fresh(std::optional<rclcpp::Time>(rclcpp::Time(transform.header.stamp, get_clock()->get_clock_type())), odom_timeout_)) {return false;}
    const auto & origin = map_->info.origin;
    // Nav2 grid origins are axis-aligned; reject other grids explicitly.
    if (std::abs(tf2::getYaw(origin.orientation)) > 1e-6) {return false;}
    const double px = transform.transform.translation.x - origin.position.x;
    const double py = transform.transform.translation.y - origin.position.y;
    const double resolution = map_->info.resolution;
    const int x0 = std::floor((px - radius_) / resolution), x1 = std::floor((px + radius_) / resolution);
    const int y0 = std::floor((py - radius_) / resolution), y1 = std::floor((py + radius_) / resolution);
    if (x0 < 0 || y0 < 0 || x1 >= static_cast<int>(map_->info.width) || y1 >= static_cast<int>(map_->info.height)) {return false;}
    for (int y = y0; y <= y1; ++y) {
      for (int x = x0; x <= x1; ++x) {
        const int cost = map_->data[y * map_->info.width + x];
        if (cost < 0 || cost >= 100) {return false;}
      }
    }
    return true;
  }
  void tick() {
    if (cancel_inflight_) {zero(); return;}
    if (visit_ && !get_parameter("enable_special_points").as_bool()) {
      if (phase_ == "NAVIGATING") {cancel_navigation();}
      finish(false, "special_points_disabled"); return;
    }
    if (visit_ && task_start_ && (now() - *task_start_).seconds() > task_timeout_) {
      if (phase_ == "NAVIGATING") {cancel_navigation();}
      finish(false, "task_timeout"); return;
    }
    if (visit_) {auto feedback = std::make_shared<Visit::Feedback>(); feedback->phase = phase_; visit_->publish_feedback(feedback);}
    if (phase_ == "IDLE" || phase_ == "NAVIGATING") {
      if (!fresh(input_time_, command_timeout_) || !fresh(odom_time_, odom_timeout_)) {zero(); return;}
      auto output = latest_;
      // This gate owns all yaw commands. Ordinary navigation is translation only.
      output.angular.z = 0; publish(output); return;
    }
    if (!fresh(odom_time_, odom_timeout_)) {finish(false, "stale_odometry"); return;}
    if (!rotation_start_ || (now() - *rotation_start_).seconds() < 0 ||
      (now() - *rotation_start_).seconds() > rotation_timeout_) {finish(false, "rotation_timeout"); return;}
    if (phase_ == "STOPPING") {
      zero();
      if (!stopped()) {stable_since_.reset(); return;}
      if (!stable_since_) {stable_since_ = now();}
      if ((now() - *stable_since_).seconds() >= 0.2) {phase_ = "ROTATING"; stable_since_.reset();}
      return;
    }
    try {
      if (!rotation_area_safe()) {finish(false, "unsafe_or_stale_rotation_map"); return;}
      const auto transform = tf_.lookupTransform(map_frame_, base_frame_, tf2::TimePointZero);
      if (!fresh(std::optional<rclcpp::Time>(rclcpp::Time(transform.header.stamp, get_clock()->get_clock_type())), odom_timeout_))
      {finish(false, "stale_transform"); return;}
      const double yaw = tf2::getYaw(transform.transform.rotation);
      const double error = std::atan2(std::sin(target_yaw_ - yaw), std::cos(target_yaw_ - yaw));
      if (rotate_) {auto feedback = std::make_shared<Rotate::Feedback>(); feedback->yaw_error = error; rotate_->publish_feedback(feedback);}
      if (std::abs(error) <= tolerance_ && (allow_translation_ || stopped())) {
        if (allow_translation_) {finish(true, "completed"); return;}
        zero(); if (!stable_since_) {stable_since_ = now();}
        if ((now() - *stable_since_).seconds() >= 0.2) {finish(true, "completed");} return;
      }
      stable_since_.reset();
      // sqrt braking envelope plus P control and angular acceleration limiting.
      const double wanted = std::copysign(std::min({max_wz_, yaw_gain_ * std::abs(error),
        std::sqrt(2 * max_aw_ * std::abs(error))}), error);
      last_wz_ += std::clamp(wanted - last_wz_, -max_aw_ * 0.025, max_aw_ * 0.025);
      geometry_msgs::msg::Twist output;
      if (allow_translation_ && fresh(input_time_, command_timeout_)) {output = latest_;}
      output.angular.z = last_wz_; publish(output);
    } catch (const std::exception & e) {finish(false, std::string("tf_error: ") + e.what());}
  }
public:
  MotionGate() : Node("sentry_motion_gate"), tf_(get_clock()), listener_(tf_, this, false) {
    map_frame_ = declare_parameter<std::string>("map_frame", "map");
    base_frame_ = declare_parameter<std::string>("base_frame", "chassis_link");
    max_wz_ = declare_parameter<double>("max_wz", 0.5); max_aw_ = declare_parameter<double>("max_angular_acceleration", 0.8);
    yaw_gain_ = declare_parameter<double>("yaw_gain", 1.5);
    command_timeout_ = declare_parameter<double>("command_timeout", 0.15);
    odom_timeout_ = declare_parameter<double>("odom_timeout", 0.2);
    map_timeout_ = declare_parameter<double>("map_timeout", 0.5);
    radius_ = declare_parameter<double>("robot_radius", 0.27);
    task_timeout_ = declare_parameter<double>("task_timeout", 120.0);
    declare_parameter<bool>("enable_special_points", false);
    for (double v : {max_wz_, max_aw_, yaw_gain_, command_timeout_, odom_timeout_, map_timeout_, radius_, task_timeout_}) {
      if (!std::isfinite(v) || v <= 0) {throw std::invalid_argument("motion gate parameters must be positive");}
    }
    const auto file = declare_parameter<std::string>("special_points_file", "");
    if (!file.empty()) {
      const auto config = YAML::LoadFile(file);
      for (const auto & item : config["special_points"]) {
        Point point{item["position"][0].as<double>(), item["position"][1].as<double>(), item["target_yaw"].as<double>()};
        const auto id = item["id"].as<std::string>();
        if (id.empty() || points_.contains(id) || !std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.yaw))
        {throw std::invalid_argument("invalid special point");} points_[id] = point;
      }
    }
    output_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 1);
    input_ = create_subscription<geometry_msgs::msg::Twist>("cmd_vel_translation", 1,
      [this](geometry_msgs::msg::Twist::ConstSharedPtr msg) {
        if (!std::isfinite(msg->linear.x) || !std::isfinite(msg->linear.y)) {latest_ = geometry_msgs::msg::Twist{}; input_time_.reset(); return;}
        latest_ = *msg; input_time_ = now();
      });
    odom_ = create_subscription<nav_msgs::msg::Odometry>("chassis_odom", rclcpp::SensorDataQoS(),
      [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
        if (msg->child_frame_id != base_frame_ || !std::isfinite(msg->twist.twist.linear.x) ||
          !std::isfinite(msg->twist.twist.linear.y) || !std::isfinite(msg->twist.twist.angular.z)) {return;}
        measured_ = msg->twist.twist; odom_time_ = rclcpp::Time(msg->header.stamp, get_clock()->get_clock_type());
      });
    map_sub_ = create_subscription<nav_msgs::msg::OccupancyGrid>("local_costmap/costmap", rclcpp::QoS(1).transient_local(),
      [this](nav_msgs::msg::OccupancyGrid::ConstSharedPtr map) {map_ = map;});
    status_ = create_subscription<action_msgs::msg::GoalStatusArray>("navigate_to_pose/_action/status", rclcpp::QoS(1).transient_local(),
      [this](action_msgs::msg::GoalStatusArray::ConstSharedPtr msg) {
        nav_active_ = false;
        for (const auto & goal : msg->status_list) {if (goal.status >= 1 && goal.status <= 3) {nav_active_ = true;}}
      });
    through_status_ = create_subscription<action_msgs::msg::GoalStatusArray>("navigate_through_poses/_action/status", rclcpp::QoS(1).transient_local(),
      [this](action_msgs::msg::GoalStatusArray::ConstSharedPtr msg) {
        through_active_ = false;
        for (const auto & goal : msg->status_list) {if (goal.status >= 1 && goal.status <= 3) {through_active_ = true;}}
      });
    follow_status_ = create_subscription<action_msgs::msg::GoalStatusArray>("follow_path/_action/status", rclcpp::QoS(1).transient_local(),
      [this](action_msgs::msg::GoalStatusArray::ConstSharedPtr msg) {
        follow_active_ = false;
        for (const auto & goal : msg->status_list) {if (goal.status >= 1 && goal.status <= 3) {follow_active_ = true;}}
      });
    navigation_ = rclcpp_action::create_client<Navigate>(this, "navigate_to_pose");
    rotate_server_ = rclcpp_action::create_server<Rotate>(this, "rotate_to_yaw",
      [this](const rclcpp_action::GoalUUID &, std::shared_ptr<const Rotate::Goal> goal) {
        if (rotate_ || visit_ || cancel_inflight_ || !fresh(odom_time_, odom_timeout_) ||
          (!goal->allow_translation && (nav_active_ || through_active_ || follow_active_ || !stopped())) ||
          !std::isfinite(goal->target_yaw) || !std::isfinite(goal->tolerance) ||
          !std::isfinite(goal->timeout) || goal->tolerance <= 0 || goal->tolerance > 0.2 || goal->timeout <= 0)
        {return rclcpp_action::GoalResponse::REJECT;} return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
      },
      [this](std::shared_ptr<RotateHandle>) {return rclcpp_action::CancelResponse::ACCEPT;},
      [this](std::shared_ptr<RotateHandle> handle) {
        rotate_ = handle; const auto goal = handle->get_goal(); allow_translation_ = goal->allow_translation;
        begin_rotation(goal->target_yaw, goal->tolerance, goal->timeout);
      });
    visit_server_ = rclcpp_action::create_server<Visit>(this, "visit_special_point",
      [this](const rclcpp_action::GoalUUID &, std::shared_ptr<const Visit::Goal> goal) {
        if (!get_parameter("enable_special_points").as_bool() || rotate_ || visit_ || nav_active_ ||
          through_active_ || follow_active_ || cancel_inflight_ || !points_.contains(goal->point_id) || !navigation_->action_server_is_ready())
        {return rclcpp_action::GoalResponse::REJECT;} return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
      },
      [this](std::shared_ptr<VisitHandle>) {return rclcpp_action::CancelResponse::ACCEPT;},
      [this](std::shared_ptr<VisitHandle> handle) {
        visit_ = handle; phase_ = "NAVIGATING"; task_start_ = now();
        const auto point = points_.at(handle->get_goal()->point_id);
        Navigate::Goal goal; goal.pose.header.frame_id = map_frame_; goal.pose.header.stamp = now();
        goal.pose.pose.position.x = point.x; goal.pose.pose.position.y = point.y; goal.pose.pose.orientation.w = 1;
        rclcpp_action::Client<Navigate>::SendGoalOptions options;
        options.goal_response_callback = [this](rclcpp_action::ClientGoalHandle<Navigate>::SharedPtr response) {
          navigation_goal_ = response;
          if (!response) {cancel_inflight_ = false; finish(false, "navigation_rejected");}
          else if (cancel_inflight_) {navigation_->async_cancel_goal(response);}
        };
        options.result_callback = [this, point](const rclcpp_action::ClientGoalHandle<Navigate>::WrappedResult & result) {
          navigation_goal_.reset(); cancel_inflight_ = false;
          if (!visit_) {zero(); input_time_.reset(); return;}
          if (visit_->is_canceling()) {finish(false, "canceled"); return;}
          if (result.code != rclcpp_action::ResultCode::SUCCEEDED) {finish(false, "navigation_failed"); return;}
          try {
            const auto pose = tf_.lookupTransform(map_frame_, base_frame_, tf2::TimePointZero);
            if (std::hypot(pose.transform.translation.x - point.x, pose.transform.translation.y - point.y) > 0.25)
            {finish(false, "arrival_position_mismatch"); return;}
          } catch (const std::exception &) {finish(false, "arrival_tf_unavailable"); return;}
          begin_rotation(point.yaw, 0.03, 10.0);
        };
        navigation_->async_send_goal(goal, options);
      });
    parameter_guard_ = add_on_set_parameters_callback([](const std::vector<rclcpp::Parameter> & parameters) {
      rcl_interfaces::msg::SetParametersResult result; result.successful = true;
      for (const auto & parameter : parameters) {
        if (parameter.get_name() != "enable_special_points" && parameter.get_name() != "use_sim_time") {
          result.successful = false; result.reason = "Only enable_special_points and use_sim_time can change at runtime";
        }
      }
      return result;
    });
    timer_ = create_wall_timer(std::chrono::milliseconds(25), [this]() {
      if (rotate_ && rotate_->is_canceling()) {finish(false, "canceled");}
      if (visit_ && visit_->is_canceling()) {
        if (phase_ == "NAVIGATING") {cancel_navigation();}
        finish(false, "canceled");
      }
      tick();
    });
    RCLCPP_INFO(get_logger(), "Translation gate ready; %zu predefined special points", points_.size());
  }
};
int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  try {rclcpp::spin(std::make_shared<MotionGate>());}
  catch (const std::exception & e) {RCLCPP_ERROR(rclcpp::get_logger("sentry_motion_gate"), "%s", e.what()); rclcpp::shutdown(); return 1;}
  rclcpp::shutdown(); return 0;
}
