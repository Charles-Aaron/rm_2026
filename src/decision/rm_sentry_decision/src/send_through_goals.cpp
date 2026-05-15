#include "send_through_goals.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace rm_sentry_decision
{

SendThroughGoalsAction::SendThroughGoalsAction(
  const std::string &name,
  const BT::NodeConfiguration &config)
: BT::StatefulActionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
  }

  std::string action_name;
  if (!getInput("action_name", action_name)) {
    action_name = "navigate_through_poses";
  }

  action_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateThroughPoses>(
    node_, action_name);
  auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local();
  nav_done_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/navigation_done", qos);
}

geometry_msgs::msg::PoseStamped SendThroughGoalsAction::parsePose(const std::string &pose_str)
{
  geometry_msgs::msg::PoseStamped pose;
  std::stringstream ss(pose_str);
  std::string token;
  std::vector<std::string> tokens;

  while (std::getline(ss, token, ';')) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }

  if (tokens.size() != 7) {
    throw std::runtime_error(
            "pose 需要 7 个用分号分隔的数字字段，实际数量 " + std::to_string(tokens.size()));
  }

  try {
    pose.pose.position.x = std::stod(tokens[0]);
    pose.pose.position.y = std::stod(tokens[1]);
    pose.pose.position.z = std::stod(tokens[2]);
    pose.pose.orientation.x = std::stod(tokens[3]);
    pose.pose.orientation.y = std::stod(tokens[4]);
    pose.pose.orientation.z = std::stod(tokens[5]);
    pose.pose.orientation.w = std::stod(tokens[6]);
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("解析 pose 失败: ") + e.what());
  }

  pose.header.frame_id = "map";
  pose.header.stamp = node_->now();
  return pose;
}

void SendThroughGoalsAction::publishNavDone(bool done)
{
  std_msgs::msg::Bool msg;
  msg.data = done;
  nav_done_pub_->publish(msg);
}

BT::NodeStatus SendThroughGoalsAction::onStart()
{
  if (!rclcpp::ok()) {
    return BT::NodeStatus::FAILURE;
  }

  publishNavDone(false);
  RCLCPP_INFO(node_->get_logger(), "途经点导航：开始执行");

  auto through_pose = getInput<std::string>("through_pose");
  if (!through_pose) {
    RCLCPP_ERROR(node_->get_logger(), "途经点导航：缺少必填输入 [through_pose]");
    return BT::NodeStatus::FAILURE;
  }

  auto final_pose = getInput<std::string>("final_pose");
  if (!final_pose) {
    RCLCPP_ERROR(node_->get_logger(), "途经点导航：缺少必填输入 [final_pose]");
    return BT::NodeStatus::FAILURE;
  }

  std::string through_pose_2;
  auto through_pose_2_input = getInput<std::string>("through_pose_2");
  if (through_pose_2_input) {
    through_pose_2 = through_pose_2_input.value();
  }

  through_pose_as_trigger_only_ = false;
  getInput("through_pose_as_trigger_only", through_pose_as_trigger_only_);
  gimbal_trigger_distance_ = 1.0;
  getInput("gimbal_trigger_distance", gimbal_trigger_distance_);
  if (gimbal_trigger_distance_ < 0.0) {
    RCLCPP_WARN(
      node_->get_logger(),
      "途经点导航：gimbal_trigger_distance=%.3f 无效，改用 0.0",
      gimbal_trigger_distance_);
    gimbal_trigger_distance_ = 0.0;
  }

  release_nav_on_close_ = false;
  getInput("release_nav_on_close", release_nav_on_close_);
  release_distance_ = 0.60;
  getInput("release_distance", release_distance_);
  if (release_distance_ < 0.0) {
    RCLCPP_WARN(
      node_->get_logger(),
      "途经点导航：release_distance=%.3f 无效，改用 0.0",
      release_distance_);
    release_distance_ = 0.0;
  }
  release_distance_reached_.store(false);
  cancel_requested_for_release_ = false;
  goal_start_time_ = std::chrono::steady_clock::now();
  getInput("goal_timeout_sec", goal_timeout_sec_);
  if (goal_timeout_sec_ <= 0.0) {
    goal_timeout_sec_ = 90.0;
  }

  gimbal_command_on_final_leg_ = false;
  getInput("gimbal_command_on_final_leg", gimbal_command_on_final_leg_);
  getInput("gimbal_command_value", gimbal_command_value_);
  getInput("gimbal_command_duration_ms", gimbal_command_duration_ms_);
  getInput("gimbal_command_period_ms", gimbal_command_period_ms_);
  getInput("gimbal_command_topic", gimbal_command_topic_);
  gimbal_command_duration_ms_ = std::max(gimbal_command_duration_ms_, 1);
  gimbal_command_period_ms_ = std::max(gimbal_command_period_ms_, 1);
  gimbal_final_leg_reached_.store(false);
  gimbal_command_active_ = false;
  gimbal_command_done_ = false;
  if (gimbal_command_on_final_leg_) {
    if (!rclcpp::ok()) {
      return BT::NodeStatus::FAILURE;
    }
    gimbal_command_pub_ =
      node_->create_publisher<rm_decision_interfaces::msg::SentryPoseCommand>(
        gimbal_command_topic_, 10);
  } else {
    gimbal_command_pub_.reset();
  }

  if (through_pose.value().empty()) {
    RCLCPP_ERROR(node_->get_logger(), "途经点导航：through_pose 为空");
    return BT::NodeStatus::FAILURE;
  }

  if (final_pose.value().empty()) {
    RCLCPP_ERROR(node_->get_logger(), "途经点导航：final_pose 为空");
    return BT::NodeStatus::FAILURE;
  }

  nav2_msgs::action::NavigateThroughPoses::Goal goal;
  try {
    has_gimbal_trigger_pose_ = false;
    gimbal_trigger_pose_ = parsePose(through_pose.value());
    has_gimbal_trigger_pose_ = true;

    goal.poses.clear();
    if (!through_pose_as_trigger_only_) {
      goal.poses.push_back(gimbal_trigger_pose_);
    }
    if (!through_pose_2.empty() && !through_pose_as_trigger_only_) {
      goal.poses.push_back(parsePose(through_pose_2));
    }
    goal.poses.push_back(parsePose(final_pose.value()));

    const auto stamp = node_->now();
    for (auto &pose : goal.poses) {
      pose.header.stamp = stamp;
    }
    if (through_pose_as_trigger_only_) {
      RCLCPP_INFO(
        node_->get_logger(),
        "途经点导航：through_pose 仅作为云台触发点，不加入导航 waypoint，触发距离 %.2f 米",
        gimbal_trigger_distance_);
    }
  } catch (const std::exception &e) {
    RCLCPP_ERROR(node_->get_logger(), "途经点导航：点位格式错误: %s", e.what());
    return BT::NodeStatus::FAILURE;
  }

  int timeout_sec = 5;
  getInput("server_timeout", timeout_sec);

  if (!action_client_->wait_for_action_server(std::chrono::seconds(timeout_sec))) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(node_->get_logger(), "途经点导航：等待导航 action 服务 %d 秒后仍不可用", timeout_sec);
    }
    return BT::NodeStatus::FAILURE;
  }

  auto send_goal_options =
    rclcpp_action::Client<nav2_msgs::action::NavigateThroughPoses>::SendGoalOptions();
  send_goal_options.feedback_callback =
    [this](
    rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::SharedPtr,
    const std::shared_ptr<const nav2_msgs::action::NavigateThroughPoses::Feedback> feedback)
    {
      RCLCPP_INFO(
        node_->get_logger(),
        "途经点导航反馈：剩余距离 %.2f 米，剩余点位 %d 个",
        feedback->distance_remaining,
        feedback->number_of_poses_remaining);

      if (release_nav_on_close_ &&
        feedback->number_of_poses_remaining <= 1 &&
        feedback->distance_remaining <= release_distance_)
      {
        release_distance_reached_.store(true);
      }

      if (gimbal_command_on_final_leg_ &&
        !through_pose_as_trigger_only_ &&
        feedback->number_of_poses_remaining <= 1)
      {
        gimbal_final_leg_reached_.store(true);
      }

      if (gimbal_command_on_final_leg_ &&
        through_pose_as_trigger_only_ &&
        has_gimbal_trigger_pose_)
      {
        const double dx =
          feedback->current_pose.pose.position.x - gimbal_trigger_pose_.pose.position.x;
        const double dy =
          feedback->current_pose.pose.position.y - gimbal_trigger_pose_.pose.position.y;
        const double distance = std::hypot(dx, dy);
        if (distance <= gimbal_trigger_distance_) {
          if (!gimbal_final_leg_reached_.load()) {
            RCLCPP_INFO(
              node_->get_logger(),
              "云台提前命令：进入触发点 %.2f 米范围，开始发布 fold_type=%u",
              gimbal_trigger_distance_,
              gimbal_command_value_ ? 1 : 0);
          }
          gimbal_final_leg_reached_.store(true);
        }
      }
    };
  send_goal_options.result_callback =
    [this](
    const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::WrappedResult &result)
    {
      if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
        RCLCPP_INFO(node_->get_logger(), "途经点导航：全部目标到达");
      } else if (result.code == rclcpp_action::ResultCode::CANCELED) {
        RCLCPP_WARN(node_->get_logger(), "途经点导航：导航被取消");
      } else {
        RCLCPP_ERROR(node_->get_logger(), "途经点导航：导航失败");
      }
    };

  goal_handle_.reset();
  result_future_ = {};
  goal_future_ = action_client_->async_send_goal(goal, send_goal_options);
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SendThroughGoalsAction::onRunning()
{
  if (!rclcpp::ok()) {
    return BT::NodeStatus::FAILURE;
  }

  updateGimbalCommand();

  const double elapsed = std::chrono::duration<double>(
    std::chrono::steady_clock::now() - goal_start_time_).count();
  if (elapsed > goal_timeout_sec_) {
    RCLCPP_WARN(node_->get_logger(), "途经点导航：超过 %.1f 秒未完成，判定超时", goal_timeout_sec_);
    if (goal_handle_) {
      action_client_->async_cancel_goal(goal_handle_);
    }
    return BT::NodeStatus::FAILURE;
  }

  if (!goal_handle_) {
    if (goal_future_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      return BT::NodeStatus::RUNNING;
    }

    goal_handle_ = goal_future_.get();
    if (!goal_handle_) {
      RCLCPP_ERROR(node_->get_logger(), "途经点导航：目标被导航服务拒绝");
      return BT::NodeStatus::FAILURE;
    }

    result_future_ = action_client_->async_get_result(goal_handle_);
    return BT::NodeStatus::RUNNING;
  }

  if (release_nav_on_close_ &&
    !cancel_requested_for_release_ &&
    release_distance_reached_.load())
  {
    RCLCPP_INFO(
      node_->get_logger(),
      "途经点导航：已进入释放底盘控制距离 %.2f 米内，取消导航目标并释放底盘控制",
      release_distance_);
    action_client_->async_cancel_goal(goal_handle_);
    cancel_requested_for_release_ = true;
  }

  if (result_future_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
    return BT::NodeStatus::RUNNING;
  }

  const auto result = result_future_.get();
  switch (result.code) {
    case rclcpp_action::ResultCode::SUCCEEDED:
      if (gimbal_command_on_final_leg_ && gimbal_final_leg_reached_.load()) {
        publishGimbalCommand();
      }
      publishNavDone(true);
      return BT::NodeStatus::SUCCESS;
    case rclcpp_action::ResultCode::ABORTED:
      RCLCPP_ERROR(node_->get_logger(), "途经点导航：导航被中止");
      return BT::NodeStatus::FAILURE;
    case rclcpp_action::ResultCode::CANCELED:
      if (cancel_requested_for_release_) {
        RCLCPP_INFO(node_->get_logger(), "途经点导航：因释放底盘控制取消导航，本次按成功处理");
        publishNavDone(true);
        return BT::NodeStatus::SUCCESS;
      }
      RCLCPP_WARN(node_->get_logger(), "途经点导航：导航被取消");
      return BT::NodeStatus::FAILURE;
    default:
      RCLCPP_ERROR(node_->get_logger(), "途经点导航：导航返回未知结果");
      return BT::NodeStatus::FAILURE;
  }
}

void SendThroughGoalsAction::onHalted()
{
  if (!rclcpp::ok()) {
    return;
  }

  if (!goal_handle_ && goal_future_.valid() &&
    goal_future_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
  {
    goal_handle_ = goal_future_.get();
  }

  if (goal_handle_) {
    RCLCPP_INFO(node_->get_logger(), "途经点导航：正在取消当前导航目标");
    auto cancel_future = action_client_->async_cancel_goal(goal_handle_);
    if (cancel_future.wait_for(std::chrono::seconds(2)) == std::future_status::ready) {
      auto resp = cancel_future.get();
      RCLCPP_INFO(node_->get_logger(), "途经点导航：取消请求返回码 %d", resp->return_code);
    } else {
      RCLCPP_WARN(node_->get_logger(), "途经点导航：取消请求超时");
    }
    goal_handle_.reset();
  }

  release_distance_reached_.store(false);
  cancel_requested_for_release_ = false;
  gimbal_final_leg_reached_.store(false);
  gimbal_command_active_ = false;
  gimbal_command_done_ = false;
}

void SendThroughGoalsAction::maybeStartGimbalCommand()
{
  if (!gimbal_command_on_final_leg_ || gimbal_command_done_ ||
      !gimbal_final_leg_reached_.load())
  {
    return;
  }

  if (!gimbal_command_active_) {
    gimbal_command_active_ = true;
    gimbal_command_start_time_ = std::chrono::steady_clock::now();
    gimbal_last_publish_time_ =
      gimbal_command_start_time_ - std::chrono::milliseconds(gimbal_command_period_ms_);
    RCLCPP_INFO(
      node_->get_logger(),
      "云台提前命令：进入最后一段导航，开始发布 fold_type=%u，话题=%s，持续=%dms",
      gimbal_command_value_ ? 1 : 0, gimbal_command_topic_.c_str(), gimbal_command_duration_ms_);
  }
}

void SendThroughGoalsAction::updateGimbalCommand()
{
  maybeStartGimbalCommand();
  if (!gimbal_command_active_ || gimbal_command_done_) {
    return;
  }

  const auto now = std::chrono::steady_clock::now();
  const auto command_elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - gimbal_command_start_time_).count();
  if (command_elapsed >= gimbal_command_duration_ms_) {
    gimbal_command_done_ = true;
    gimbal_command_active_ = false;
    RCLCPP_INFO(
      node_->get_logger(),
      "云台提前命令：发布完成，持续 %dms", gimbal_command_duration_ms_);
    return;
  }

  const auto publish_elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - gimbal_last_publish_time_).count();
  if (publish_elapsed >= gimbal_command_period_ms_) {
    publishGimbalCommand();
  }
}

void SendThroughGoalsAction::publishGimbalCommand()
{
  if (!gimbal_command_pub_) {
    return;
  }

  rm_decision_interfaces::msg::SentryPoseCommand msg;
  msg.pose_type = 0;
  msg.fold_type = gimbal_command_value_ ? 1 : 0;
  gimbal_command_pub_->publish(msg);
  gimbal_last_publish_time_ = std::chrono::steady_clock::now();
}

} // namespace rm_sentry_decision
