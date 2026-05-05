#include "gimbal_lower_action.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"

#include <algorithm>
#include <stdexcept>

namespace rm_sentry_decision {

GimbalLowerAction::GimbalLowerAction(
  const std::string &name,
  const BT::NodeConfiguration &config)
: BT::StatefulActionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("云台低头/抬头控制：无法从行为树黑板获取 ROS 节点");
  }
}

BT::NodeStatus GimbalLowerAction::onStart()
{
  getInput("target_lower", target_lower_);
  getInput("wait_state", wait_state_);
  getInput("continue_on_timeout", continue_on_timeout_);
  getInput("timeout_ms", timeout_ms_);
  getInput("publish_period_ms", publish_period_ms_);
  getInput("publish_duration_ms", publish_duration_ms_);
  getInput("command_topic", command_topic_);
  getInput("state_topic", state_topic_);

  timeout_ms_ = std::max(timeout_ms_, 1);
  publish_period_ms_ = std::max(publish_period_ms_, 1);
  publish_duration_ms_ = std::max(publish_duration_ms_, 1);

  state_received_.store(false);
  current_gimbal_mode_.store(0);

  command_pub_ =
    node_->create_publisher<rm_decision_interfaces::msg::GimbalCommand>(command_topic_, 10);
  if (wait_state_) {
    state_sub_ = node_->create_subscription<rm_msgs::msg::GimbalStatus>(
      state_topic_, 10,
      [this](const rm_msgs::msg::GimbalStatus::SharedPtr msg) {
        current_gimbal_mode_.store(msg->gimbal_mode);
        state_received_.store(true);
      });
  } else {
    state_sub_.reset();
  }

  start_time_ = std::chrono::steady_clock::now();
  last_publish_time_ = start_time_ - std::chrono::milliseconds(publish_period_ms_);
  publishCommand();

  RCLCPP_INFO(
    node_->get_logger(),
    "云台低头/抬头控制：开始执行，目标低头=%d，等待反馈=%d，命令话题=%s，状态话题=%s，低头判定模式=%u，抬头判定模式=%u",
    target_lower_, wait_state_, command_topic_.c_str(), state_topic_.c_str(),
    kGimbalFoldedMode, kGimbalRaisedMode);

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GimbalLowerAction::onRunning()
{
  const auto now = std::chrono::steady_clock::now();
  const auto publish_elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - last_publish_time_).count();
  if (publish_elapsed >= publish_period_ms_) {
    publishCommand();
  }

  if (wait_state_) {
    if (state_received_.load() && isTargetModeReached(current_gimbal_mode_.load())) {
      RCLCPP_INFO(
        node_->get_logger(),
        "云台低头/抬头控制：电控反馈已达到目标，目标低头=%d，当前 gimbal_mode=%u",
        target_lower_, current_gimbal_mode_.load());
      return BT::NodeStatus::SUCCESS;
    }

    if (elapsedMsAtLeast(start_time_, timeout_ms_)) {
      RCLCPP_WARN(
        node_->get_logger(),
        "云台低头/抬头控制：等待反馈超时 %d ms，目标低头=%d，最后收到 gimbal_mode=%u，是否继续=%d",
        timeout_ms_, target_lower_, current_gimbal_mode_.load(), continue_on_timeout_);
      return continue_on_timeout_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::RUNNING;
  }

  if (elapsedMsAtLeast(start_time_, publish_duration_ms_)) {
    RCLCPP_INFO(
      node_->get_logger(),
      "云台低头/抬头控制：命令已重复发布 %d ms，目标低头=%d",
      publish_duration_ms_, target_lower_);
    return BT::NodeStatus::SUCCESS;
  }

  return BT::NodeStatus::RUNNING;
}

void GimbalLowerAction::onHalted()
{
  RCLCPP_INFO(node_->get_logger(), "云台低头/抬头控制：被中断");
}

void GimbalLowerAction::publishCommand()
{
  if (!command_pub_) {
    return;
  }

  rm_decision_interfaces::msg::GimbalCommand msg;
  msg.fold_type = target_lower_ ? kCommandFolded : kCommandRaised;
  command_pub_->publish(msg);
  last_publish_time_ = std::chrono::steady_clock::now();

  RCLCPP_DEBUG(
    node_->get_logger(),
    "云台低头/抬头控制：已发布命令 fold_type=%u", msg.fold_type);
}

bool GimbalLowerAction::isTargetModeReached(uint8_t gimbal_mode) const
{
  return target_lower_ ? gimbal_mode == kGimbalFoldedMode : gimbal_mode == kGimbalRaisedMode;
}

bool GimbalLowerAction::elapsedMsAtLeast(
  const std::chrono::steady_clock::time_point &start,
  int limit_ms) const
{
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::steady_clock::now() - start).count();
  return elapsed >= limit_ms;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::GimbalLowerAction>("GimbalLowerAction");
}
