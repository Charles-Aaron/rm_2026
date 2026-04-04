#pragma once

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <atomic>
#include <string>
#include <memory>

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class SelectSupplyPoint : public BT::SyncActionNode {
public:
  SelectSupplyPoint(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;

private:
  // ROS
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr status_sub_;

  // 参数管理器
  std::shared_ptr<ParameterManager> param_manager_;

  // Cached data
  std::atomic<int32_t> self_hp_{0};
  // 剩余弹丸量
  std::atomic<int32_t> remaining_ammo_{9999};
  std::atomic<uint16_t> enemy3_hp_{0};
  std::atomic<uint16_t> enemy4_hp_{0};
  std::atomic<uint16_t> outpost_hp_{0};
  std::atomic<bool> is_blue_{false};
  std::atomic<bool> status_ready_{false};
  std::atomic<bool> hp_ready_{false};
  std::atomic<bool> suppressed_{false};
};

} // namespace rm_sentry_decision 