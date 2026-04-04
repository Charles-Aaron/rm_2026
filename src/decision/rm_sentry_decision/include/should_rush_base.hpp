#ifndef RM_SENTRY_DECISION__SHOULD_RUSH_BASE_HPP_
#define RM_SENTRY_DECISION__SHOULD_RUSH_BASE_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include <atomic>

namespace rm_sentry_decision {

class ShouldRushBase : public BT::ConditionNode {
public:
  ShouldRushBase(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() { return {}; }

  BT::NodeStatus tick() override;

private:
  std::atomic<int32_t> enemy_base_hp_{0};
  std::atomic<int32_t> our_base_hp_{0};
  std::atomic<bool> is_blue_{false};

  rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr hp_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr status_sub_;
  rclcpp::Node::SharedPtr node_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__SHOULD_RUSH_BASE_HPP_ 