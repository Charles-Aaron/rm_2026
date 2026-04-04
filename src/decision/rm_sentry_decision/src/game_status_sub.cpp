#include "guard_fortress.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

GuardFortress::GuardFortress(const std::string &name, const BT::NodeConfiguration &config)
    : BT::StatefulActionNode(name, config) {
  // 获取 ROS2 节点
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  // 初始化参数管理器
  auto param_manager = rm_sentry_decision::ParameterManager::getInstance(node_);
  param_manager_ = param_manager;
  
  // 从参数管理器获取持续时间
  duration_seconds_ = param_manager->getBuffGuardParam<int>("duration");
}

BT::NodeStatus GuardFortress::onStart() {
  start_time_ = node_->get_clock()->now();
  RCLCPP_INFO(node_->get_logger(), "开始蹲守增益点，持续 %.1f 秒", duration_seconds_);
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GuardFortress::onRunning() {
  auto elapsed = node_->get_clock()->now() - start_time_;
  if (elapsed.seconds() >= duration_seconds_) {
    RCLCPP_INFO(node_->get_logger(), "蹲守完毕");
    return BT::NodeStatus::SUCCESS;
  }
  // 仍在等待
  return BT::NodeStatus::RUNNING;
}

void GuardFortress::onHalted() {
  RCLCPP_WARN(node_->get_logger(), "GuardFortress 被中断");
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) { factory.registerNodeType<rm_sentry_decision::GuardFortress>("GuardFortress"); } 