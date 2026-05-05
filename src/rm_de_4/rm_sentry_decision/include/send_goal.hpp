#ifndef RM_SENTRY_DECISION__SEND_GOAL_HPP_
#define RM_SENTRY_DECISION__SEND_GOAL_HPP_

#include "behaviortree_cpp_v3/action_node.h"  // 使用v3专用头文件
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include <std_msgs/msg/bool.hpp>
#include <chrono>

namespace rm_sentry_decision
{

class SendGoalAction : public BT::StatefulActionNode  // 使用v3的状态节点
{
public:
    SendGoalAction(const std::string& name, 
                  const BT::NodeConfiguration& config);  // 使用标准构造函数
    
    static BT::PortsList providedPorts()
    {
        return {
            BT::InputPort<std::string>("goal_pose"),
            BT::InputPort<std::string>("action_name", "navigate_to_pose"),
            BT::InputPort<int>("server_timeout", 5, "等待 action server 的秒数"),
            BT::InputPort<double>("goal_timeout_sec", 60.0, "导航目标最大运行时间")
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr action_client_;
    rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr goal_handle_;
    std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr> goal_future_;
    std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::WrappedResult> result_future_;
    geometry_msgs::msg::PoseStamped parseGoalPose(const std::string& pose_str);
    void publishNavDone(bool done);
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr nav_done_pub_;
    std::chrono::steady_clock::time_point goal_start_time_;
    double goal_timeout_sec_{60.0};
};

} // namespace rm_behavior_tree

#endif
