#ifndef RM_SENTRY_DECISION__ALLIES_DOWN_HPP_
#define RM_SENTRY_DECISION__ALLIES_DOWN_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <atomic>

namespace rm_sentry_decision {

// 条件节点：当本方 1/3/4 号机器人全部阵亡(HP==0)时返回 SUCCESS
class AlliesDown : public BT::ConditionNode {
public:
  AlliesDown(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() { return {}; }

  BT::NodeStatus tick() override;

private:
  std::atomic<int32_t> robot1_hp_{999};
  std::atomic<int32_t> robot3_hp_{999};
  std::atomic<int32_t> robot4_hp_{999};
  std::atomic<bool> is_blue_{false};
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr sub_1_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__ALLIES_DOWN_HPP_ 