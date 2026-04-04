#ifndef RM_SENTRY_DECISION__SEND_THROUGH_GOALS_HPP_
#define RM_SENTRY_DECISION__SEND_THROUGH_GOALS_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav2_msgs/action/navigate_through_poses.hpp"
#include <atomic>
#include <std_msgs/msg/bool.hpp>

namespace rm_sentry_decision
{

class SendThroughGoalsAction : public BT::StatefulActionNode
{
public:
  SendThroughGoalsAction(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("through_pose"),
      BT::InputPort<std::string>("through_pose_2", ""),
      BT::InputPort<std::string>("final_pose"),
      BT::InputPort<bool>(
        "release_nav_on_close",
        false,
        "到终点附近后主动取消导航并释放底盘控制"),
      BT::InputPort<double>(
        "release_distance",
        0.60,
        "触发释放导航的剩余距离阈值（米）"),
      BT::InputPort<std::string>("action_name", "navigate_through_poses"),
      BT::InputPort<int>("server_timeout", 5, "等待 action server 的秒数")
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  geometry_msgs::msg::PoseStamped parsePose(const std::string &pose_str);
  void publishNavDone(bool done);

  rclcpp::Node::SharedPtr node_;
  rclcpp_action::Client<nav2_msgs::action::NavigateThroughPoses>::SharedPtr action_client_;
  rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::SharedPtr goal_handle_;
  std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::SharedPtr> goal_future_;
  std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::WrappedResult> result_future_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr nav_done_pub_;
  bool release_nav_on_close_{false};
  double release_distance_{0.60};
  std::atomic<bool> release_distance_reached_{false};
  bool cancel_requested_for_release_{false};
};

} // namespace rm_sentry_decision

#endif
