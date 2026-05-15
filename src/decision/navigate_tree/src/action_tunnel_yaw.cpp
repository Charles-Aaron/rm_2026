#include "action_tunnel_yaw.h"
#include <cmath>

namespace {
// 第一补给点路径上的两个隧道点，在 1.2 m 半径内时让车头对正 map +x 轴
constexpr double kTunnelPointA_x = 0.0;
constexpr double kTunnelPointA_y = 0.0;
constexpr double kTunnelPointB_x = 1.0;
constexpr double kTunnelPointB_y = 1.0;
constexpr double kTunnelTriggerRadius = 1.2;
constexpr float kAlignYawMapPositiveX = 0.0f;
}

action_tunnel_yaw::action_tunnel_yaw(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    chassis_yaw_pub_ = node_->create_publisher<std_msgs::msg::Float32>("chassis_yaw", 10);
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(node_->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
}

BT::PortsList action_tunnel_yaw::providedPorts() {
    return {};
}

BT::NodeStatus action_tunnel_yaw::tick() {
    geometry_msgs::msg::TransformStamped tf;
    try {
        tf = tf_buffer_->lookupTransform(
            "map", "chassis", rclcpp::Time(0), rclcpp::Duration(0, 0));
    } catch (const tf2::TransformException& ex) {
        RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000,
                             "tunnel_yaw TF lookup failed: %s", ex.what());
        return BT::NodeStatus::FAILURE;
    }

    const double x = tf.transform.translation.x;
    const double y = tf.transform.translation.y;
    const double dA = std::hypot(x - kTunnelPointA_x, y - kTunnelPointA_y);
    const double dB = std::hypot(x - kTunnelPointB_x, y - kTunnelPointB_y);

    if (dA < kTunnelTriggerRadius || dB < kTunnelTriggerRadius) {
        std_msgs::msg::Float32 yaw_msg;
        yaw_msg.data = kAlignYawMapPositiveX;
        chassis_yaw_pub_->publish(yaw_msg);
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                             "Inside tunnel zone, publishing chassis_yaw=0 (map +x)");
        return BT::NodeStatus::SUCCESS;
    }
    return BT::NodeStatus::FAILURE;
}
