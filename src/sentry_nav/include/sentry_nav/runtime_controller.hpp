#pragma once

#include <nav2_core/controller.hpp>
#include <nav2_core/global_planner.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sentry_nav_interfaces/srv/ordered_route.hpp>

namespace sentry_nav {
// Stage-one boundary: reuse the tested engine and costmap adapter without
// planner/controller servers or a BT navigator. Nav2 ABI remains for fallback.
class RuntimeController : public nav2_core::Controller {
public:
  virtual void resetTask() = 0;
  virtual void stopMotion() = 0;
  virtual sentry_nav_interfaces::srv::OrderedRoute::Response orderedRequest(
    const sentry_nav_interfaces::srv::OrderedRoute::Request & request) = 0;
  virtual nav_msgs::msg::Odometry::ConstSharedPtr odometry() = 0;
};
std::unique_ptr<RuntimeController> make_runtime_controller();
std::unique_ptr<nav2_core::GlobalPlanner> make_runtime_planner();
}  // namespace sentry_nav
