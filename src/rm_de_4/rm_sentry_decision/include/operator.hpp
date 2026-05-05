#ifndef RM_SENTRY_DECISION__OperatorHPP_
#define RM_SENTRY_DECISION__Operator_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <std_msgs/msg/bool.hpp>
namespace rm_sentry_decision
{

class Operator : public BT::StatefulActionNode
{
public:
    Operator(const std::string& name,
                   const BT::NodeConfiguration& config);

    /*========  只改这里：清空端口  ========*/
    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr action_client_;
    rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr goal_handle_;
    std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr> future;

    /*========  新增：订阅器 & 缓存  ========*/
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr sub_;
    std::atomic<double> operator_x_{0.0};
    std::atomic<double> operator_y_{0.0};
     rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr nav_done_pub_;
    // bool published_done_flag_ = false;   // 避免重复发布
    std_msgs::msg::Bool msg;
    /*========  删除 parseGoalPose —— 不再使用  ========*/
};

} // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__SEND_GOAL_HPP_