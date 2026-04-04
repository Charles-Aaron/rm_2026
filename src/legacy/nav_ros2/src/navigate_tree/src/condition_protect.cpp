#include "condition_protect.h"

condition_protect ::condition_protect (const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
     node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
     sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
         "/remote_data",  // 主题名称
        10,              // 队列大小
        [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
           // 提取 start_flag 字段
           flag_ = (msg->protect_flag == 1);  // 如果 start_flag 为 1，flag_ 为 true
         });
}

BT::PortsList condition_protect::providedPorts() {
    return {};
}

BT::NodeStatus condition_protect::tick() {
    RCLCPP_ERROR(node_->get_logger(), "36");
    return flag_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}