#ifndef RM_SENTRY_DECISION__HERO_PROTECT_ATTACK_HPP_
#define RM_SENTRY_DECISION__HERO_PROTECT_ATTACK_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include <behaviortree_cpp_v3/action_node.h>
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <atomic>
#include <string>
#include <memory>

#pragma once

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class HeroProtectAttack : public BT::SyncActionNode
{
public:
  HeroProtectAttack(const std::string& name, const BT::NodeConfiguration& config);
  BT::NodeStatus tick() override;
  static BT::PortsList providedPorts() { return {}; }

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr sub_;
  std::shared_ptr<ParameterManager> param_manager_;

  std::atomic<int32_t> hero_hp_{-1};
  std::atomic<int32_t> home_base_hp_{0};
  std::atomic<bool>    is_blue_{false};
  int32_t      last_hero_hp_{-1};
};

}  // namespace rm_sentry_decision   // ← 千万别漏分号

#endif // RM_SENTRY_DECISION__HERO_PROTECT_ATTACK_HPP_ 
