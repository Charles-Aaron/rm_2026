#pragma once

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include "rm_decision_interfaces/msg/bullet_allowance.hpp"
#include <atomic>
#include <chrono>

namespace rm_sentry_decision {

class LateGameDecision : public BT::SyncActionNode {
public:
  LateGameDecision(const std::string &name,
                   const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;

  // Subscriptions
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_status_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr robot_status_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr all_hp_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::BulletAllowance>::SharedPtr bullet_allowance_sub_;

  // Cached data
  std::atomic<uint16_t> stage_remain_time_{9999};
  std::atomic<uint16_t> current_hp_{0};
  std::atomic<uint16_t> ammo_{0};  // 使用shooter_heat字段暂存弹药量
  std::atomic<uint16_t> own_base_hp_{0};
  std::atomic<uint16_t> enemy_base_hp_{0};
  std::atomic<bool> is_blue_team_{false};

  // --- New for Buff Point Guard logic ---
  // Own robots HP to evaluate if all are defeated
  std::atomic<uint16_t> robot1_hp_{0};
  std::atomic<uint16_t> robot3_hp_{0};
  std::atomic<uint16_t> robot4_hp_{0};

  // Guard state tracking
  std::atomic<bool> guard_active_{false};
  std::chrono::steady_clock::time_point guard_start_time_;
};

} // namespace rm_sentry_decision 