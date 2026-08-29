#include "self_hp_at_least.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

SelfHpAtLeast::SelfHpAtLeast(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("SelfHpAtLeast: Failed to get ROS node from blackboard");
  }

  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_detection_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        current_hp_.store(msg->current_hp);
        game_progress_.store(msg->game_progress);
        has_status_.store(true);
      });
}

BT::NodeStatus SelfHpAtLeast::tick() {
  int threshold = 400;
  if (!getInput("threshold", threshold)) {
    threshold = 400;
  }

  if (!has_status_.load()) {
    RCLCPP_DEBUG(node_->get_logger(), "SelfHpAtLeast: no GameStatus received yet, returning FAILURE");
    return BT::NodeStatus::FAILURE;
  }

  if (game_progress_.load() != 4) {
    RCLCPP_DEBUG(node_->get_logger(),
                 "SelfHpAtLeast: game_progress=%u (need 4), returning FAILURE",
                 static_cast<unsigned int>(game_progress_.load()));
    return BT::NodeStatus::FAILURE;
  }

  const auto hp = current_hp_.load();
  if (hp >= static_cast<uint16_t>(threshold)) {
    RCLCPP_INFO(node_->get_logger(), "SelfHpAtLeast: HP %u >= %d, returning SUCCESS",
                static_cast<unsigned int>(hp), threshold);
    return BT::NodeStatus::SUCCESS;
  }

  RCLCPP_DEBUG(node_->get_logger(), "SelfHpAtLeast: HP %u < %d, returning FAILURE",
               static_cast<unsigned int>(hp), threshold);
  return BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::SelfHpAtLeast>("SelfHpAtLeast");
}
