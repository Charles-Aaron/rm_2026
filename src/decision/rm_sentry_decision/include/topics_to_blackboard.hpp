#ifndef RM_SENTRY_DECISION__TOPICS_TO_BLACKBOARD_HPP_
#define RM_SENTRY_DECISION__TOPICS_TO_BLACKBOARD_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"

namespace rm_sentry_decision {

class TopicsToBlackboard : public BT::SyncActionNode {
public:
  TopicsToBlackboard(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() { return {}; }

  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  DataManager *data_manager_{nullptr};
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__TOPICS_TO_BLACKBOARD_HPP_
