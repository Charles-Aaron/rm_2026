#ifndef RM_SENTRY_DECISION__SELF_HP_BELOW_HPP_
#define RM_SENTRY_DECISION__SELF_HP_BELOW_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <cstdint>
#include <atomic>

namespace rm_sentry_decision {

class SelfHpBelow : public BT::ConditionNode {
public:
  SelfHpBelow(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<int>("threshold", 100, "HP threshold (<=)"),
      BT::InputPort<bool>("require_armed", false, "Need one HP>threshold sample before allowing trigger")
    };
  }

  BT::NodeStatus tick() override;

private:
  std::atomic<uint16_t> current_hp_{0};
  std::atomic<uint8_t> game_progress_{0};
  std::atomic<bool> has_status_{false};
  std::atomic<bool> seen_hp_above_threshold_{false};
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr status_sub_;
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__SELF_HP_BELOW_HPP_ 
