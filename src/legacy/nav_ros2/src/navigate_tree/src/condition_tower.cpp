#include "condition_tower.h"

condition_tower::condition_tower(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
     node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
     sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
         "/remote_data",  // 主题名称
        10,              // 队列大小
        [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
           // 提取 start_flag 字段
           flag_ = (msg->tower_flag == 1);  // 如果 start_flag 为 1，flag_ 为 true
         });
}

BT::PortsList condition_tower::providedPorts() {
    return {};
}

BT::NodeStatus condition_tower::tick() {
    RCLCPP_ERROR(node_->get_logger(), "35");
    return flag_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}