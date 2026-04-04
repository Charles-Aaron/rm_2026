#ifndef RM_SENTRY_DECISION__ENDGAME_NEED_SUPPLY_HPP_
#define RM_SENTRY_DECISION__ENDGAME_NEED_SUPPLY_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include "rm_decision_interfaces/msg/bullet_allowance.hpp"
#include <atomic>

namespace rm_sentry_decision {

class EndgameNeedSupply : public BT::ConditionNode {
public:
  EndgameNeedSupply(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() { return {}; }

  BT::NodeStatus tick() override;

private:
  std::atomic<int32_t> remaining_time_{9999}; // 单位秒
  std::atomic<int32_t> current_hp_{0};
  std::atomic<int32_t> ammo_{9999};

  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr status_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::BulletAllowance>::SharedPtr bullet_sub_;
  rclcpp::Node::SharedPtr node_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__ENDGAME_NEED_SUPPLY_HPP_ 