#include "self_hp_below.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

SelfHpBelow::SelfHpBelow(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  // 从黑板获取 ROS2 节点
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("SelfHpBelow: Failed to get ROS node from blackboard");
  }

  // 从比赛状态读取当前血量
  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_detection_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        current_hp_.store(msg->current_hp);
        game_progress_.store(msg->game_progress);
        has_status_.store(true);
      });
}

BT::NodeStatus SelfHpBelow::tick() {
  int threshold = 100;
  if (!getInput("threshold", threshold)) {
    threshold = 100;
  }
  bool require_armed = false;
  if (!getInput("require_armed", require_armed)) {
    require_armed = false;
  }

  if (!has_status_.load()) {
    RCLCPP_DEBUG(node_->get_logger(), "SelfHpBelow: no GameStatus received yet, returning FAILURE");
    return BT::NodeStatus::FAILURE;
  }

  // 只在比赛开始状态(game_progress=4)下判断血量，避免上电初始状态(常见0血量)误触发。
  if (game_progress_.load() != 4) {
    RCLCPP_DEBUG(node_->get_logger(),
                 "SelfHpBelow: game_progress=%u (need 4), returning FAILURE",
                 static_cast<unsigned int>(game_progress_.load()));
    return BT::NodeStatus::FAILURE;
  }

  const auto hp = current_hp_.load();
  if (hp > static_cast<uint16_t>(threshold)) {
    // 先看到一次“高于阈值”的有效血量，再允许后续到达阈值时触发返航。
    // 这样可避免出发初期因默认0值或异常帧立刻触发返航。
    seen_hp_above_threshold_.store(true);
    RCLCPP_DEBUG(node_->get_logger(), "SelfHpBelow: HP %u > %d, armed=true, returning FAILURE",
                 static_cast<unsigned int>(hp), threshold);
    return BT::NodeStatus::FAILURE;
  }

  if (require_armed && !seen_hp_above_threshold_.load()) {
    RCLCPP_DEBUG(node_->get_logger(),
                 "SelfHpBelow: HP %u < %d but require_armed=true and not armed yet, returning FAILURE",
                 static_cast<unsigned int>(hp), threshold);
    return BT::NodeStatus::FAILURE;
  }

  if (hp <= static_cast<uint16_t>(threshold)) {
    RCLCPP_INFO(node_->get_logger(), "SelfHpBelow: HP %u <= %d, returning SUCCESS",
                static_cast<unsigned int>(hp), threshold);
    return BT::NodeStatus::SUCCESS;
  }

  RCLCPP_DEBUG(node_->get_logger(), "SelfHpBelow: HP %u > %d, returning FAILURE",
               static_cast<unsigned int>(hp), threshold);
  return BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

// 将节点注册到 BT 工厂（备用，与手动注册二选一均可）
BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::SelfHpBelow>("SelfHpBelow");
} 
