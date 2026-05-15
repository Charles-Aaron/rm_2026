#ifndef RM_SENTRY_DECISION__SEND_THROUGH_GOALS_HPP_
#define RM_SENTRY_DECISION__SEND_THROUGH_GOALS_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav2_msgs/action/navigate_through_poses.hpp"
#include "rm_decision_interfaces/msg/sentry_pose_command.hpp"
#include <atomic>
#include <chrono>
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
        "through_pose_as_trigger_only",
        false,
        "through_pose 只作为触发点，不加入导航目标，避免中间点到点减速"),
      BT::InputPort<double>(
        "gimbal_trigger_distance",
        1.0,
        "距离触发点多少米内开始发布云台命令"),
      BT::InputPort<bool>(
        "release_nav_on_close",
        false,
        "到终点附近后主动取消导航并释放底盘控制"),
      BT::InputPort<double>(
        "release_distance",
        0.60,
        "触发释放导航的剩余距离阈值（米）"),
      BT::InputPort<std::string>("action_name", "navigate_through_poses"),
      BT::InputPort<int>("server_timeout", 5, "等待 action server 的秒数"),
      BT::InputPort<double>("goal_timeout_sec", 90.0, "多点导航目标最大运行时间"),
      BT::InputPort<bool>(
        "gimbal_command_on_final_leg",
        false,
        "进入最后一段导航后是否重复发布云台低头命令"),
      BT::InputPort<bool>(
        "gimbal_command_value",
        false,
        "云台命令值：true=折叠，false=抬起"),
      BT::InputPort<int>(
        "gimbal_command_duration_ms",
        1500,
        "最后一段触发后的云台命令重复发送时长(ms)"),
      BT::InputPort<int>(
        "gimbal_command_period_ms",
        100,
        "最后一段触发后的云台命令重复发送周期(ms)"),
      BT::InputPort<std::string>(
        "gimbal_command_topic",
        "/sentry/gimbal_lower_command",
        "云台低头命令话题")
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  geometry_msgs::msg::PoseStamped parsePose(const std::string &pose_str);
  void publishNavDone(bool done);
  void maybeStartGimbalCommand();
  void updateGimbalCommand();
  void publishGimbalCommand();

  rclcpp::Node::SharedPtr node_;
  rclcpp_action::Client<nav2_msgs::action::NavigateThroughPoses>::SharedPtr action_client_;
  rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::SharedPtr goal_handle_;
  std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::SharedPtr> goal_future_;
  std::shared_future<rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::WrappedResult> result_future_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr nav_done_pub_;
  bool release_nav_on_close_{false};
  bool through_pose_as_trigger_only_{false};
  double gimbal_trigger_distance_{1.0};
  geometry_msgs::msg::PoseStamped gimbal_trigger_pose_;
  bool has_gimbal_trigger_pose_{false};
  double release_distance_{0.60};
  std::atomic<bool> release_distance_reached_{false};
  bool cancel_requested_for_release_{false};
  std::chrono::steady_clock::time_point goal_start_time_;
  double goal_timeout_sec_{90.0};
  rclcpp::Publisher<rm_decision_interfaces::msg::SentryPoseCommand>::SharedPtr gimbal_command_pub_;
  bool gimbal_command_on_final_leg_{false};
  bool gimbal_command_value_{false};
  int gimbal_command_duration_ms_{1500};
  int gimbal_command_period_ms_{100};
  std::string gimbal_command_topic_{"/sentry/gimbal_lower_command"};
  std::atomic<bool> gimbal_final_leg_reached_{false};
  bool gimbal_command_active_{false};
  bool gimbal_command_done_{false};
  std::chrono::steady_clock::time_point gimbal_command_start_time_;
  std::chrono::steady_clock::time_point gimbal_last_publish_time_;
};

} // namespace rm_sentry_decision

#endif
