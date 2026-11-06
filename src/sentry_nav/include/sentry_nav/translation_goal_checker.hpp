#pragma once
#include <nav2_core/goal_checker.hpp>
#include <optional>

namespace sentry_nav {
// Arrival requires continuously acceptable XY position and translational speed.
class TranslationGoalChecker : public nav2_core::GoalChecker {
public:
  void initialize(const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent,
    const std::string & name,
    const std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap) override;
  void reset() override {ordered_route_ = false; waypoints_passed_ = false; reached_since_.reset();}
  void setWaypointProgress(bool ordered_route, bool passed) {
    ordered_route_ = ordered_route; waypoints_passed_ = passed;
  }
  bool requiresWaypointProgress() const {return require_waypoint_progress_;}
  bool isGoalReached(const geometry_msgs::msg::Pose & pose,
    const geometry_msgs::msg::Pose & goal,
    const geometry_msgs::msg::Twist & velocity) override;
  bool getTolerances(geometry_msgs::msg::Pose & pose,
    geometry_msgs::msg::Twist & velocity) override;
private:
  double xy_tolerance_ = 0.2;
  double stopped_velocity_ = 0.04;
  double confirmation_time_ = 0.2;
  rclcpp::Clock::SharedPtr clock_ = std::make_shared<rclcpp::Clock>(RCL_STEADY_TIME);
  std::optional<rclcpp::Time> reached_since_;
  bool require_waypoint_progress_ = false, ordered_route_ = false, waypoints_passed_ = false;
};
}  // namespace sentry_nav
