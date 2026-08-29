#include "log_info_action.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision
{

LogInfoAction::LogInfoAction(const std::string &name, const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("流程日志：无法从行为树黑板获取 ROS 节点");
  }
}

BT::NodeStatus LogInfoAction::tick()
{
  if (!rclcpp::ok()) {
    return BT::NodeStatus::SUCCESS;
  }

  std::string message;
  if (!getInput("message", message) || message.empty()) {
    message = name();
  }

  int period_ms = 1000;
  getInput("period_ms", period_ms);
  if (period_ms < 0) {
    period_ms = 0;
  }

  const auto now = std::chrono::steady_clock::now();
  const bool should_log =
    !logged_once_ ||
    std::chrono::duration_cast<std::chrono::milliseconds>(now - last_log_time_).count() >= period_ms;

  if (should_log) {
    RCLCPP_INFO(node_->get_logger(), "当前触发流程：%s", message.c_str());
    last_log_time_ = now;
    logged_once_ = true;
  }

  return BT::NodeStatus::SUCCESS;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<rm_sentry_decision::LogInfoAction>("LogInfoAction");
}
