#pragma once

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"
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
  DataManager *data_manager_{nullptr};

  // Guard state tracking
  std::atomic<bool> guard_active_{false};
  std::chrono::steady_clock::time_point guard_start_time_;
};

} // namespace rm_sentry_decision 
