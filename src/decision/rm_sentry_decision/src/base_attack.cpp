#include "base_attack.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

BaseAttack::BaseAttack(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::ConditionNode(name, config)
{
  // 从blackboard获取ROS2节点
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  
  // 血量数据订阅
  hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
    "/all_robot_hp", 10,
    [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
      red_outpost_hp_.store(msg->red_outpost_hp);
      blue_outpost_hp_.store(msg->blue_outpost_hp);
      red_base_hp_.store(msg->red_base_hp);
      blue_base_hp_.store(msg->blue_base_hp);
      RCLCPP_DEBUG(node_->get_logger(), "HP Update - R_Out: %d B_Out: %d R_Base: %d B_Base: %d", 
                 red_outpost_hp_.load(), blue_outpost_hp_.load(), red_base_hp_.load(), blue_base_hp_.load());
    });

  // 队伍状态订阅
  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
    "/robot_status", 10,
    [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
      is_blue_team_.store(msg->team_color );
      RCLCPP_DEBUG(node_->get_logger(), "Team: %s", 
                  is_blue_team_.load() ? "Blue" : "Red");
    });
}

BT::NodeStatus BaseAttack::tick() {
  bool blue = is_blue_team_.load();
  uint16_t enemy_outpost_hp = blue ? red_outpost_hp_.load() : blue_outpost_hp_.load();
  uint16_t enemy_base_hp = blue ? red_base_hp_.load() : blue_base_hp_.load();

  const bool can_attack = (enemy_outpost_hp == 0) && 
                         (enemy_base_hp > 0);

  if (can_attack) {
    RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                       "Base attack condition met! (Outpost: 0, Base: %d)",
                       enemy_base_hp);
    return BT::NodeStatus::SUCCESS;
  }
  
  return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

// v3版本注册
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::BaseAttack>("BaseAttack");
}