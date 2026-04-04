#include "cancel_nav.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

RobotStatusSub::RobotStatusSub(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::ConditionNode(name, config)
{
    // 从黑板获取ROS节点
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 订阅机器人状态
    status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            current_hp_.store(msg->current_hp);
            // 移除 shooter_heat 订阅，只使用 bullet_allowance_17mm
            
            RCLCPP_DEBUG(node_->get_logger(), 
                       "Status Update - HP: %d", 
                       msg->current_hp);
                       bullets_remaining_.store(msg->bullet_allowance_17mm); // 使用实际的17mm弹量

            RCLCPP_DEBUG(node_->get_logger(),
                       "Bullet Allowance Update - 17mm Remaining: %d",
                       msg->bullet_allowance_17mm);
            enemy.store(msg->view_flag);
        });

    // 订阅子弹余量
    
    // 初始化参数管理器
    param_manager_ = rm_sentry_decision::ParameterManager::getInstance(node_);
}


BT::NodeStatus RobotStatusSub::tick() {
     RCLCPP_DEBUG(node_->get_logger(), "RobotStatusSub tick called");

    // 从参数管理器获取阈值参数
    int min_hp = param_manager_->getRobotStatusParam<int>("min_hp");
    int min_bullets = param_manager_->getRobotStatusParam<int>("min_bullets");

    // 检查状态
    const bool is_status_ok = (current_hp_.load() >= min_hp) && 
                            (bullets_remaining_.load() >= min_bullets)&&;

    // 设置输出端口
    setOutput("is_status_ok", is_status_ok);

    // 日志输出
    if (is_status_ok) {
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                           "机器人状态正常 (HP: %d ≥ %d, Bullets: %d ≥ %d)",
                           current_hp_.load(), min_hp,
                           bullets_remaining_.load(), min_bullets);
        return BT::NodeStatus::SUCCESS;
    }
    
    RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                        "机器人状态异常 (HP: %d/%d, Bullets: %d/%d)", 
                        current_hp_.load(), min_hp,
                        bullets_remaining_.load(), min_bullets);
    return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision