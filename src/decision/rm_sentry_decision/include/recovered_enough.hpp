#ifndef RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_
#define RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <atomic>
#include <memory>

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class RecoveredEnough : public BT::ConditionNode {
public:
  RecoveredEnough(const std::string &name, const BT::NodeConfiguration &config);
  static BT::PortsList providedPorts() { return {}; }
  BT::NodeStatus tick() override;
private:
  std::atomic<int32_t> current_hp_{0};
  std::atomic<int32_t> remaining_ammo_{0};
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr status_sub_;
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_ 