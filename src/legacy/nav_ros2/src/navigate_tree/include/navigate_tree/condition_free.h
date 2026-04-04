#ifndef CONDITION_FREE_HPP
#define CONDITION_FREE_HPP

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "customize_messages/msg/remotedata.hpp"  

class condition_free : public BT::ConditionNode {
public:
      condition_free (const std::string& name, const BT::NodeConfiguration& config);

    static BT::PortsList providedPorts();

    BT::NodeStatus tick() override;

private:
    rclcpp::Node::SharedPtr node_;
     rclcpp::Subscription<customize_messages::msg::Remotedata>::SharedPtr sub_;
    bool flag_ = false;
};

#endif  // 