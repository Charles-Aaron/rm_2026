#include "hero_protect_condition.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"

namespace rm_sentry_decision {

HeroProtectCondition::HeroProtectCondition(
    const std::string& name,
    const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config),
      node_(rclcpp::Node::make_shared("hero_protect_condition_node")),
      is_protect_(false)
{
    protect_sub_ = node_->create_subscription<std_msgs::msg::Bool>(
        "/hero_protect_status",
        10,
        std::bind(&HeroProtectCondition::protectCallback, this, std::placeholders::_1)
    );

    RCLCPP_INFO(node_->get_logger(), "HeroProtectCondition initialized, default=false");
}

BT::PortsList HeroProtectCondition::providedPorts()
{
    return {};
}

void HeroProtectCondition::protectCallback(
    const std_msgs::msg::Bool::SharedPtr msg)
{
    is_protect_ = msg->data;
    RCLCPP_DEBUG(
        node_->get_logger(),
        "HeroProtectCondition updated: %s",
        is_protect_ ? "true" : "false");
}

BT::NodeStatus HeroProtectCondition::tick()
{
    // 关键：强制刷新 ROS 回调
    rclcpp::spin_some(node_);

    return is_protect_
        ? BT::NodeStatus::SUCCESS
        : BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory)
{
    factory.registerNodeType<rm_sentry_decision::HeroProtectCondition>(
        "HeroProtectAttack");
}

