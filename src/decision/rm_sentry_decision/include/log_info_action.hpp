#ifndef RM_SENTRY_DECISION__LOG_INFO_ACTION_HPP_
#define RM_SENTRY_DECISION__LOG_INFO_ACTION_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"

#include <chrono>

namespace rm_sentry_decision
{

class LogInfoAction : public BT::SyncActionNode
{
public:
  LogInfoAction(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("message", "行为树流程日志"),
      BT::InputPort<int>("period_ms", 1000, "重复打印最小间隔，单位 ms")
    };
  }

  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  std::chrono::steady_clock::time_point last_log_time_{};
  bool logged_once_{false};
};

}  // namespace rm_sentry_decision

#endif
