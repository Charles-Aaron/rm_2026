#include "attack_strategy.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_decision_interfaces/msg/robot_status.hpp"

namespace rm_sentry_decision {

AttackStrategy::AttackStrategy(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
      "/all_robot_hp", 10,
      [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
        enemy_base_hp_.store(is_blue_ ? msg->red_base_hp : msg->blue_base_hp);
        our_base_hp_.store(is_blue_ ? msg->blue_base_hp : msg->red_base_hp);
      });

  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
      "/robot_status", 10,
      [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
        is_blue_.store(msg->team_color);
      });
}

BT::NodeStatus AttackStrategy::tick() {
  bool attack_enemy = enemy_base_hp_.load() < our_base_hp_.load();
  setOutput("attack_enemy_base", attack_enemy);
  return BT::NodeStatus::SUCCESS; // always succeed, just set output
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::AttackStrategy>("AttackStrategy");
} 