#include "recovered_enough.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

RecoveredEnough::RecoveredEnough(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  // 初始化参数管理器
  param_manager_ = ParameterManager::getInstance(node_);

  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        current_hp_.store(msg->current_hp);
        remaining_ammo_.store(msg->bullet_allowance_17mm); // 使用实际的17mm弹量
        // 移除 shooter_heat 订阅，只使用 bullet_allowance_17mm
      });

  // Subscribe bullet allowance for actual ammo count
  // bullet_sub_ = node_->create_subscription<rm_decision_interfaces::msg::BulletAllowance>(
  //     "/bullet_allowance", 10,
  //     [this](const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg) {
  //       remaining_ammo_.store(msg->bullet_allowance_17mm); // 使用实际的17mm弹量
  //     });
}

BT::NodeStatus RecoveredEnough::tick() {
  // 从参数管理器获取恢复阈值
  int recovery_hp = param_manager_->getHpThreshold("recovery");
  int recovery_ammo = param_manager_->getAmmoThreshold("recovery");

  if (current_hp_.load() >= recovery_hp && remaining_ammo_.load() >= recovery_ammo) {
    RCLCPP_DEBUG(node_->get_logger(), "RecoveredEnough: HP=%d>=%d, Ammo=%d>=%d", 
                 current_hp_.load(), recovery_hp, remaining_ammo_.load(), recovery_ammo);
    return BT::NodeStatus::SUCCESS;
  }
  
  RCLCPP_DEBUG(node_->get_logger(), "RecoveredEnough: Not recovered yet - HP=%d<%d or Ammo=%d<%d", 
               current_hp_.load(), recovery_hp, remaining_ammo_.load(), recovery_ammo);
  return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::RecoveredEnough>("RecoveredEnough");
} 