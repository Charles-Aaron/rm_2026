#include "wait_for_outpost_destroyed.hpp"

namespace rm_sentry_decision {

WaitForOutpostDestroyed::WaitForOutpostDestroyed(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::StatefulActionNode(name, config),
    enemy_outpost_hp_(-1),
    is_blue_team_(false),
    initial_hp_received_(false)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    
       outpost_hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
    "/all_robot_hp", 10,
    [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
      enemy_outpost_hp_.store(is_blue_team_ ? msg->red_outpost_hp : msg->blue_outpost_hp);
      // 标记已收到首帧血量消息，允许 onRunning 继续判断
      if (!initial_hp_received_) {
          initial_hp_received_ = true;
      }
    });

  // 创建队伍状态订阅（修正比较方式）
  status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
    "/robot_status", 10,
    [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
      is_blue_team_.store(msg->team_color); // 直接使用bool值
    });
}


BT::NodeStatus WaitForOutpostDestroyed::onStart() {
    // 重置状态
    initial_hp_received_ = false;
    RCLCPP_INFO(node_->get_logger(), "开始等待前哨站被摧毁");
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus WaitForOutpostDestroyed::onRunning() {
    if (!initial_hp_received_) {
        // 等待接收第一次血量数据
        return BT::NodeStatus::RUNNING;
    }
    
    const int current_hp = enemy_outpost_hp_.load();
    RCLCPP_DEBUG(node_->get_logger(), "当前前哨站血量: %d", current_hp);
    
    if (current_hp <= 0) {
        RCLCPP_INFO(node_->get_logger(), "前哨站已被摧毁!");
        return BT::NodeStatus::SUCCESS;
    }
    
    return BT::NodeStatus::RUNNING;
}

void WaitForOutpostDestroyed::onHalted() {
    RCLCPP_INFO(node_->get_logger(), "等待前哨站摧毁任务被中断");
}

} // namespace rm_sentry_decision

// 注册节点
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::WaitForOutpostDestroyed>("WaitForOutpostDestroyed");
}