#include "sentry_nav/translation_goal_checker.hpp"
#include <pluginlib/class_list_macros.hpp>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace sentry_nav {
void TranslationGoalChecker::initialize(
  const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent, const std::string & name,
  const std::shared_ptr<nav2_costmap_2d::Costmap2DROS>)
{
  const auto node = parent.lock();
  if (!node) {throw std::invalid_argument("goal checker parent expired");}
  auto parameter = [&](const std::string & key, double fallback) {
      const auto full_name = name + "." + key;
      if (!node->has_parameter(full_name)) {node->declare_parameter(full_name, fallback);}
      const double value = node->get_parameter(full_name).as_double();
      if (!std::isfinite(value) || value < 0) {
        throw std::invalid_argument(full_name + " must be finite and nonnegative");
      }
      return value;
    };
  xy_tolerance_ = parameter("xy_goal_tolerance", 0.2);
  stopped_velocity_ = parameter("trans_stopped_velocity", 0.04);
  confirmation_time_ = parameter("goal_confirmation_time", 0.2);
  clock_ = node->get_clock();
  reached_since_.reset();
  const auto progress_parameter = name + ".require_waypoint_progress";
  if (!node->has_parameter(progress_parameter)) {node->declare_parameter(progress_parameter, false);}
  require_waypoint_progress_ = node->get_parameter(progress_parameter).as_bool();
}

bool TranslationGoalChecker::isGoalReached(const geometry_msgs::msg::Pose & pose,
  const geometry_msgs::msg::Pose & goal, const geometry_msgs::msg::Twist & velocity)
{
  if ((require_waypoint_progress_ || ordered_route_) &&
    (!ordered_route_ || !waypoints_passed_)) {reached_since_.reset(); return false;}
  const double distance = std::hypot(pose.position.x - goal.position.x,
    pose.position.y - goal.position.y);
  const double speed = std::hypot(velocity.linear.x, velocity.linear.y);
  if (!std::isfinite(distance) || !std::isfinite(speed) ||
    distance > xy_tolerance_ || speed > stopped_velocity_) {
    reached_since_.reset(); return false;
  }
  const auto now = clock_->now();
  if (!reached_since_ || now < *reached_since_) {reached_since_ = now;}
  return (now - *reached_since_).seconds() >= confirmation_time_;
}

bool TranslationGoalChecker::getTolerances(geometry_msgs::msg::Pose & pose,
  geometry_msgs::msg::Twist & velocity)
{
  // GoalChecker uses lowest() for unmeasured fields.
  const double unused = std::numeric_limits<double>::lowest();
  pose.position.x = pose.position.y = xy_tolerance_;
  pose.position.z = unused;
  pose.orientation.x = pose.orientation.y = pose.orientation.z = pose.orientation.w = unused;
  velocity.linear.x = velocity.linear.y = stopped_velocity_;
  velocity.linear.z = unused;
  velocity.angular.x = velocity.angular.y = velocity.angular.z = unused;
  return true;
}
}  // namespace sentry_nav

PLUGINLIB_EXPORT_CLASS(sentry_nav::TranslationGoalChecker, nav2_core::GoalChecker)
