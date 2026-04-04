#include "hero_protect_attack.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"
// #include "rm_decision_interfaces/msg/all_robot_hp.hpp"


namespace rm_sentry_decision {

HeroProtectAttack::HeroProtectAttack(const std::string& name,
                                     const BT::NodeConfiguration& config)
  : BT::SyncActionNode(name, config)
{
  node_ = config.blackboard->template get<rclcpp::Node::SharedPtr>("node");
  if (!node_) throw std::runtime_error("no ROS node");

  // 初始化参数管理器
  param_manager_ = ParameterManager::getInstance(node_);

  sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_detection_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        is_blue_.store(msg->team_color);
        hero_hp_.store(is_blue_.load() ? msg->blue_1_robot_hp
                                       : msg->red_1_robot_hp);
        home_base_hp_.store(is_blue_.load() ? msg->blue_base_hp
                                            : msg->red_base_hp);
      });
}

BT::NodeStatus HeroProtectAttack::tick() {
  int base_critical = param_manager_->getBaseThreshold("critical");
  int base_hp = home_base_hp_.load();
  int hero_hp = hero_hp_.load();
  
  RCLCPP_INFO(node_->get_logger(), 
      "HeroProtectAttack check: BaseHP=%d (critical=%d), HeroHP=%d",
      base_hp, base_critical, hero_hp);
  
  if (base_hp < base_critical) {
    RCLCPP_INFO(node_->get_logger(), 
        "HeroProtectAttack: Base HP %d < %d, returning FAILURE", 
        base_hp, base_critical);
    return BT::NodeStatus::FAILURE;
  }

  int hero_threshold = 300;
  if (hero_hp < hero_threshold) {
    RCLCPP_INFO(node_->get_logger(), 
        "HeroProtectAttack: Hero HP %d < %d, needs protection! Returning SUCCESS",
        hero_hp, hero_threshold);
    return BT::NodeStatus::SUCCESS;
  }
  
  RCLCPP_INFO(node_->get_logger(), 
      "HeroProtectAttack: Hero HP %d >= %d, returning FAILURE",
      hero_hp, hero_threshold);
  return BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::HeroProtectAttack>("HeroProtectAttack");
}
