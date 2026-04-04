#include "condition_blosupeng.h"

condition_blosupeng::condition_blosupeng(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
     node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
     sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
         "/remote_data",  // 主题名称
        10,              // 队列大小
        [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
           // 提取 operator_control 字段
           flag_ = (msg->home_flag==1);  // 如果 operator_control 为 1，flag_ 为 true
         });
}

BT::PortsList condition_blosupeng::providedPorts() {
    return {};
}

BT::NodeStatus condition_blosupeng::tick() {
    return flag_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}