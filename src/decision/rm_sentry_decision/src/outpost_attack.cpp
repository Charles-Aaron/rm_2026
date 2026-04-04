#include "outpost_attack.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

OutpostAttack::OutpostAttack(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::ConditionNode(name, config),
    red_outpost_hp_(0),
    blue_outpost_hp_(0),
    is_blue_team_(false)
{
  // 从blackboard获取ROS2节点
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  
  // 创建前哨站血量订阅
  outpost_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
    "/game_detection_status", 10,
    [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
      red_outpost_hp_.store(msg->red_outpost_hp);
      blue_outpost_hp_.store(msg->blue_outpost_hp);
       is_blue_team_.store(msg->team_color);
    });

  // 创建队伍状态订阅（修正比较方式）
  
  // 暂时注释掉参数管理器初始化，使用硬编码值测试
  // param_manager_ = rm_sentry_decision::ParameterManager::getInstance(node_);
  RCLCPP_INFO(node_->get_logger(), "OutpostAttack initialized with hardcoded values");
}

BT::NodeStatus OutpostAttack::tick() {
  // 暂时使用硬编码值
  int min_hp = -10000;  // 硬编码敌人阵亡阈值

  // 根据最新队伍颜色计算敌我血量
  bool blue = is_blue_team_.load();
  int enemy_hp = blue ? red_outpost_hp_.load() : blue_outpost_hp_.load();

  RCLCPP_INFO(
    node_->get_logger(),
    "OutpostAttack check - Team: %s, EnemyOutpostHP: %d, Result: %s",
    blue ? "Blue" : "Red", enemy_hp, (enemy_hp > 0) ? "SUCCESS" : "FAILURE"
  );

  return (enemy_hp > 0) 
    ? BT::NodeStatus::SUCCESS 
    : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

// v3版本注册方式
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::OutpostAttack>("OutpostAttack");
}
