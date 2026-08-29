#include "robot_status_sub_optimized.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

RobotStatusSubOptimized::RobotStatusSubOptimized(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::ConditionNode(name, config)
{
    // 从黑板获取ROS节点
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取数据管理器实例
    data_manager_ = &DataManager::getInstance(node_);

    // 初始化参数管理器
    param_manager_ = ParameterManager::getInstance(node_);

    RCLCPP_DEBUG(node_->get_logger(),
                "RobotStatusSubOptimized initialized with DataManager");
}

BT::NodeStatus RobotStatusSubOptimized::tick() {
    RCLCPP_DEBUG(node_->get_logger(), "RobotStatusSubOptimized tick called");
    
    // 检查数据管理器是否已初始化
    if (!data_manager_ || !data_manager_->isInitialized()) {
        RCLCPP_ERROR(node_->get_logger(), "DataManager not initialized");
        return BT::NodeStatus::FAILURE;
    }
    
    // 检查数据是否新鲜
    if (!data_manager_->isDataFresh(2000)) {  // 2秒内的数据认为是新鲜的
        RCLCPP_WARN(node_->get_logger(), "Data is stale, waiting for fresh data");
        return BT::NodeStatus::FAILURE;
    }
    
    int min_hp{};
    int min_bullets{};

    bool has_hp = false;
    bool has_bullets = false;

    if (config().input_ports.count("min_hp")) {
        has_hp = getInput("min_hp", min_hp);
    }
    if (config().input_ports.count("min_bullets")) {
        has_bullets = getInput("min_bullets", min_bullets);
    }

    if (!has_hp) {
        min_hp = param_manager_->getRobotStatusParam<int>("min_hp");
    }
    if (!has_bullets) {
        min_bullets = param_manager_->getRobotStatusParam<int>("min_bullets");
    }

    RCLCPP_INFO(node_->get_logger(),
               "min_hp=%d (port:%d) min_bullets=%d (port:%d)",
               min_hp, has_hp, min_bullets, has_bullets);

    // 从数据管理器获取当前状态
    const uint16_t current_hp = data_manager_->current_hp_.load();
    const uint16_t bullets_remaining = data_manager_->bullets_remaining_.load();

    // 检查状态
    const bool is_status_ok = (current_hp >= min_hp) && 
                            (bullets_remaining >= min_bullets);

    // 设置输出端口
    setOutput("is_status_ok", is_status_ok);

    // 日志输出
    if (is_status_ok) {
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                           "机器人状态正常 (HP: %d ≥ %d, Bullets: %d ≥ %d)",
                           current_hp, min_hp,
                           bullets_remaining, min_bullets);
        return BT::NodeStatus::SUCCESS;
    }
    
    RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                        "机器人状态异常 (HP: %d/%d, Bullets: %d/%d)", 
                        current_hp, min_hp,
                        bullets_remaining, min_bullets);
    return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::RobotStatusSubOptimized>("RobotStatusSubOptimized");
} 