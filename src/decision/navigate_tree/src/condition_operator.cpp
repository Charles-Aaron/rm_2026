#include "condition_operator.h"

condition_operator::condition_operator(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
     node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
     sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
         "/remote_data",  // 主题名称
        10,              // 队列大小
        [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
           // 提取 operator_control 字段
           flag_ = (msg->operator_control == 1);  // 如果 operator_control 为 1，flag_ 为 true
         });
}

BT::PortsList condition_operator::providedPorts() {
    return {};
}

BT::NodeStatus condition_operator::tick() {
    return flag_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}