#ifndef RM_SENTRY_DECISION__SET_NAV_SPEED_HPP_
#define RM_SENTRY_DECISION__SET_NAV_SPEED_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"

#include <string>

namespace rm_sentry_decision {

class SetNavSpeedAction : public BT::SyncActionNode {
public:
  SetNavSpeedAction(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<double>("speed_mps", 0.9, "target navigation speed (m/s)"),
      BT::InputPort<std::string>("controller_node", "", "controller node name/path"),
      BT::InputPort<int>("service_timeout_ms", 500, "set_parameters service timeout in ms")
    };
  }

  BT::NodeStatus tick() override;

private:
  std::string resolveControllerNode(const std::string &controller_node_input) const;

  rclcpp::Node::SharedPtr node_;
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__SET_NAV_SPEED_HPP_
