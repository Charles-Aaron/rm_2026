#include "allies_down.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

AlliesDown::AlliesDown(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  // 从黑板获取 rclcpp::Node
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  // 订阅三台己方机器人的状态话题，话题名根据机器人 ID 区分
  sub_1_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_detection_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        is_blue_.store(msg->team_color);
            if (is_blue_.load()) {
                robot1_hp_.store(msg->blue_1_robot_hp);
                robot3_hp_.store(msg->blue_3_robot_hp);
                robot4_hp_.store(msg->blue_4_robot_hp);
            } else {
                robot1_hp_.store(msg->red_1_robot_hp);
                robot3_hp_.store(msg->red_3_robot_hp);
                robot4_hp_.store(msg->red_4_robot_hp);
            }
      });
}

BT::NodeStatus AlliesDown::tick() {
  int robot1_hp = robot1_hp_.load();
  int robot3_hp = robot3_hp_.load();
  int robot4_hp = robot4_hp_.load();
  bool is_blue = is_blue_.load();
  
  const bool all_down = (robot1_hp == 0) && (robot3_hp == 0) && (robot4_hp == 0);
  
  RCLCPP_WARN(node_->get_logger(), 
              "AlliesDown check - Team: %s, Robot1: %d, Robot3: %d, Robot4: %d, AllDown: %s", 
              is_blue ? "Blue" : "Red", robot1_hp, robot3_hp, robot4_hp, all_down ? "true" : "false");
  
  return all_down ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) { factory.registerNodeType<rm_sentry_decision::AlliesDown>("AlliesDown"); } 