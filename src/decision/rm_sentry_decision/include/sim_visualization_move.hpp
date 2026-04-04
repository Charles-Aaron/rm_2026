#ifndef RM_SENTRY_DECISION__SIM_VISUALIZATION_MOVE_HPP_
#define RM_SENTRY_DECISION__SIM_VISUALIZATION_MOVE_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "pose_behavior.hpp"
#include <atomic>
#include <chrono>
#include <memory>

namespace rm_sentry_decision {

class SimVisualizationMove : public BT::StatefulActionNode {
public:
    SimVisualizationMove(const std::string& name, 
                        const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("move_type", "", "移动类型：attack/defense/patrol 或留空自动检测"),
            BT::InputPort<double>("linear_speed", 0.5, "线速度 (m/s)"),
            BT::InputPort<double>("angular_speed", 0.0, "角速度 (rad/s)"),
            BT::InputPort<double>("move_duration", 2.0, "移动持续时间 (秒)"),
            BT::InputPort<bool>("continuous", false, "是否持续移动")
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    std::shared_ptr<PoseManager> pose_manager_;
    
    geometry_msgs::msg::Twist current_cmd_;
    std::chrono::steady_clock::time_point start_time_;
    double move_duration_;
    bool continuous_;
    bool movement_started_;
    
    void publishStopCommand();
    void calculateMovementFromPose(RobotPose pose, double linear_speed);
    void calculateMovementFromType(const std::string& move_type, double linear_speed);
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__SIM_VISUALIZATION_MOVE_HPP_

