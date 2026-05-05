#include "endgame_need_supply.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

EndgameNeedSupply::EndgameNeedSupply(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  game_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        remaining_time_.store(msg->stage_remain_time);
      });

  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
      "/robot_status", 10,
      [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
        current_hp_.store(msg->current_hp);
        // 移除 shooter_heat 订阅，只使用 bullet_allowance_17mm
      });

  // Subscribe bullet allowance for actual ammo count
  bullet_sub_ = node_->create_subscription<rm_decision_interfaces::msg::BulletAllowance>(
      "/bullet_allowance", 10,
      [this](const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg) {
        ammo_.store(msg->bullet_allowance_17mm); // 使用实际的17mm弹量
      });
}

BT::NodeStatus EndgameNeedSupply::tick() {
  // 当剩余时间 <=150s 且 HP、弹药均低于 200 时返回 SUCCESS
  bool need = (remaining_time_.load() <= 150) && (current_hp_.load() < 200) && (ammo_.load() < 200);
  return need ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::EndgameNeedSupply>("EndgameNeedSupply");
} 