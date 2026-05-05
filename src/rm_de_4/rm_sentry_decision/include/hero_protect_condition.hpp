#pragma once

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"

namespace rm_sentry_decision {

class HeroProtectCondition : public BT::ConditionNode {
public:
    HeroProtectCondition(
        const std::string& name,
        const BT::NodeConfiguration& config);

    static BT::PortsList providedPorts();
    BT::NodeStatus tick() override;

private:
    void protectCallback(const std_msgs::msg::Bool::SharedPtr msg);

    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr protect_sub_;
    bool is_protect_;
};

}  // namespace rm_sentry_decision

