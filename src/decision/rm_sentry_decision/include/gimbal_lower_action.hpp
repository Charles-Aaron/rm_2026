#ifndef RM_SENTRY_DECISION__GIMBAL_LOWER_ACTION_HPP_
#define RM_SENTRY_DECISION__GIMBAL_LOWER_ACTION_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rm_decision_interfaces/msg/gimbal_command.hpp"
#include "rm_msgs/msg/gimbal_status.hpp"
#include "rclcpp/rclcpp.hpp"

#include <atomic>
#include <chrono>
#include <string>

namespace rm_sentry_decision {

class GimbalLowerAction : public BT::StatefulActionNode {
public:
  GimbalLowerAction(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<bool>("target_lower", true, "true=低下云台，false=抬起云台"),
      BT::InputPort<bool>("wait_state", true, "是否等待电控状态反馈到目标值"),
      BT::InputPort<int>("timeout_ms", 3000, "等待反馈超时时间(ms)"),
      BT::InputPort<int>("publish_period_ms", 100, "重复发送命令周期(ms)"),
      BT::InputPort<int>("publish_duration_ms", 1500, "不等待反馈时的重复发送时长(ms)"),
      BT::InputPort<bool>("continue_on_timeout", true, "等待超时后是否继续流程"),
      BT::InputPort<std::string>(
        "command_topic", "/sentry/gimbal_lower_command", "云台低头命令话题"),
      BT::InputPort<std::string>(
        "state_topic", "/gimbal_controller/gimbal_status", "云台低头状态反馈话题")
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  static constexpr uint8_t kGimbalRaisedMode = 1;
  static constexpr uint8_t kGimbalFoldedMode = 5;
  static constexpr uint8_t kCommandRaised = 0;
  static constexpr uint8_t kCommandFolded = 1;

  void publishCommand();
  bool isTargetModeReached(uint8_t gimbal_mode) const;
  bool elapsedMsAtLeast(const std::chrono::steady_clock::time_point &start, int limit_ms) const;

  rclcpp::Node::SharedPtr node_;
  rclcpp::Publisher<rm_decision_interfaces::msg::GimbalCommand>::SharedPtr command_pub_;
  rclcpp::Subscription<rm_msgs::msg::GimbalStatus>::SharedPtr state_sub_;

  bool target_lower_{true};
  bool wait_state_{true};
  bool continue_on_timeout_{true};
  int timeout_ms_{3000};
  int publish_period_ms_{100};
  int publish_duration_ms_{1500};
  std::string command_topic_{"/sentry/gimbal_lower_command"};
  std::string state_topic_{"/gimbal_controller/gimbal_status"};

  std::atomic<bool> state_received_{false};
  std::atomic<uint8_t> current_gimbal_mode_{0};
  std::chrono::steady_clock::time_point start_time_;
  std::chrono::steady_clock::time_point last_publish_time_;
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__GIMBAL_LOWER_ACTION_HPP_
