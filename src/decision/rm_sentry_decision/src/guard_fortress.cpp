#include "game_status_sub.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"  // v3专用头文件

namespace rm_sentry_decision {

GameStatusSub::GameStatusSub(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config)
{
    auto node = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    
    sub_ = node->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            current_status_.store(msg->game_progress);
        });
}

BT::NodeStatus GameStatusSub::tick()
{
    int start_status;
    if (!getInput("start_status", start_status)) {
        throw BT::RuntimeError("Missing required input [start_status]");
    }
    
    int current = current_status_.load();
    bool result = (current == start_status);
    
    auto node = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
    RCLCPP_INFO(node->get_logger(), 
        "GameStatusSub check: current=%d, required=%d, result=%s",
        current, start_status, result ? "SUCCESS" : "FAILURE");
    
    return result ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

// v3版本注册方式
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::GameStatusSub>("GameStatusSub");
}
