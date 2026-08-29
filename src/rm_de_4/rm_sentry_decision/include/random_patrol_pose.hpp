#pragma once

#include "behaviortree_cpp_v3/behavior_tree.h"
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class RandomPatrolPose : public BT::SyncActionNode {
public:
  RandomPatrolPose(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;

private:
   std::mt19937 rng_{static_cast<unsigned>(
      std::chrono::steady_clock::now().time_since_epoch().count())};
 std::chrono::steady_clock::time_point last_switch_{};
  std::string cached_pose_;
  int interval_sec_{8};
  std::atomic<int32_t> own_base_hp_{2000}; 
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr sub_base_;
   rclcpp::Node::SharedPtr node_;  // 需要手动管理节点指针
   std::shared_ptr<ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision 