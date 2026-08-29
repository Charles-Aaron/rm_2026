#include "need_supply_optimized.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

NeedSupplyOptimized::NeedSupplyOptimized(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
    
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
                "NeedSupplyOptimized initialized with DataManager");
}

BT::NodeStatus NeedSupplyOptimized::tick() {
    // 检查数据管理器是否已初始化
    if (!data_manager_ || !data_manager_->isInitialized()) {
        RCLCPP_ERROR(node_->get_logger(), "DataManager not initialized");
        return BT::NodeStatus::FAILURE;
    }
    
    // 检查数据是否新鲜
    if (!data_manager_->isDataFresh(2000)) {
        RCLCPP_WARN(node_->get_logger(), "Data is stale, waiting for fresh data");
        return BT::NodeStatus::FAILURE;
    }
    
    // 获取输入参数
    int critical_time, low_hp_threshold, low_ammo_threshold;
    if (!getInput("critical_time", critical_time)) {
        critical_time = 150;  // 默认值
    }
    if (!getInput("low_hp_threshold", low_hp_threshold)) {
        low_hp_threshold = 200;  // 默认值
    }
    if (!getInput("low_ammo_threshold", low_ammo_threshold)) {
        low_ammo_threshold = 200;  // 默认值
    }
    
    // 从数据管理器获取当前状态
    const uint16_t remaining_time = data_manager_->stage_remain_time_.load();
    const uint16_t current_hp = data_manager_->current_hp_.load();
    const uint16_t remaining_ammo = data_manager_->bullets_remaining_.load();
    
    // 判断是否需要补给
    // 当剩余时间 <= critical_time 且 HP、弹药均低于阈值时返回 SUCCESS
    bool need_supply = (remaining_time <= critical_time) && 
                      (current_hp < low_hp_threshold) && 
                      (remaining_ammo < low_ammo_threshold);
    
    // 日志输出
    if (need_supply) {
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                           "需要补给 - 时间: %d/%d, HP: %d/%d, 弹药: %d/%d",
                           remaining_time, critical_time,
                           current_hp, low_hp_threshold,
                           remaining_ammo, low_ammo_threshold);
    } else {
        RCLCPP_DEBUG_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
                             "无需补给 - 时间: %d/%d, HP: %d/%d, 弹药: %d/%d",
                             remaining_time, critical_time,
                             current_hp, low_hp_threshold,
                             remaining_ammo, low_ammo_threshold);
    }
    
    return need_supply ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::NeedSupplyOptimized>("NeedSupplyOptimized");
} 