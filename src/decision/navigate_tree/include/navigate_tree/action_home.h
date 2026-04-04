#ifndef ACTION_HOME_HPP
#define ACTION_HOME_HPP


#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp_action/rclcpp_action.hpp"
#include "rclcpp_action/client.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include <vector>
#include <memory>
#include "customize_messages/msg/remotedata.hpp"
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2/LinearMath/Quaternion.h>
#include <chrono>
#include <tf2/utils.h>
//#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <geometry_msgs/msg/transform_stamped.hpp>  // 包含正确的头文件‌:ml-citation{ref="6" data="citationList"}
#include <rclcpp/clock.hpp>
#include <memory>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>

class action_home : public BT::StatefulActionNode {
public:
    using NavigateToPose = nav2_msgs::action::NavigateToPose;

    action_home(const std::string& name, const BT::NodeConfiguration& config);

    static BT::PortsList providedPorts();

    BT::NodeStatus onStart() override;

    BT::NodeStatus onRunning() override;
    void onHalted() override; 

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp_action::Client<NavigateToPose>::SharedPtr client_;
    std::shared_future<rclcpp_action::ClientGoalHandle<NavigateToPose>::SharedPtr> future_goal_handle_;
    rclcpp_action::ClientGoalHandle<NavigateToPose>::SharedPtr goal_handle_;
    bool is_goal_done_ = false;  // 标志，表示动作是否完成
    bool is_goal_success_ = false;  // 标志，表示动作是否成功
    std::vector<geometry_msgs::msg::PoseStamped> goals_;
    rclcpp::Subscription<customize_messages::msg::Remotedata>::SharedPtr sub_;
    size_t current_goal_index_ = 0;
    char interrupt_navigation_ = 0;
};

#endif  // NAVIGATE_ACTION_HPP