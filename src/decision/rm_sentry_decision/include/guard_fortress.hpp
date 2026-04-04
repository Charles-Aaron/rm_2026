#ifndef RM_SENTRY_DECISION__GUARD_FORTRESS_HPP_
#define RM_SENTRY_DECISION__GUARD_FORTRESS_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <optional>

namespace rm_sentry_decision {

// 动作节点：到达增益点后，持续蹲守 duration 秒
class GuardFortress : public BT::StatefulActionNode {
public:
  GuardFortress(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {};
  }

private:
  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

  rclcpp::Node::SharedPtr node_;
  rclcpp::Time start_time_;
  double duration_seconds_{60.0};
  std::shared_ptr<rm_sentry_decision::ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__GUARD_FORTRESS_HPP_ 