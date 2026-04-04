#include "send_through_goals.hpp"

#include <chrono>
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
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  std::string action_name;
  if (!getInput("action_name", action_name)) {
    action_name = "navigate_through_poses";
  }

  action_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateThroughPoses>(
    node_, action_name);
  nav_done_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/navigation_done", 10);
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
            "pose requires 7 semicolon-separated numeric fields, got " + std::to_string(tokens.size()));
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
    throw std::runtime_error(std::string("Failed to parse pose: ") + e.what());
  }

  pose.header.frame_id = "map";
  pose.header.stamp = node_->now();
  return pose;
}

void SendThroughGoalsAction::publishNavDone(bool done)
{
  std_msgs::msg::Bool msg;
  msg.data = done;
  for (int i = 0; i < 100; ++i) {
    nav_done_pub_->publish(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

BT::NodeStatus SendThroughGoalsAction::onStart()
{
  publishNavDone(false);
  RCLCPP_INFO(node_->get_logger(), "SendThroughGoalsAction started");

  auto through_pose = getInput<std::string>("through_pose");
  if (!through_pose) {
    RCLCPP_ERROR(node_->get_logger(), "Missing required input [through_pose]");
    return BT::NodeStatus::FAILURE;
  }

  auto final_pose = getInput<std::string>("final_pose");
  if (!final_pose) {
    RCLCPP_ERROR(node_->get_logger(), "Missing required input [final_pose]");
    return BT::NodeStatus::FAILURE;
  }

  std::string through_pose_2;
  auto through_pose_2_input = getInput<std::string>("through_pose_2");
  if (through_pose_2_input) {
    through_pose_2 = through_pose_2_input.value();
  }

  release_nav_on_close_ = false;
  getInput("release_nav_on_close", release_nav_on_close_);
  release_distance_ = 0.60;
  getInput("release_distance", release_distance_);
  if (release_distance_ < 0.0) {
    RCLCPP_WARN(
      node_->get_logger(),
      "release_distance=%.3f is invalid, fallback to 0.0",
      release_distance_);
    release_distance_ = 0.0;
  }
  release_distance_reached_.store(false);
  cancel_requested_for_release_ = false;

  if (through_pose.value().empty()) {
    RCLCPP_ERROR(node_->get_logger(), "through_pose is empty string");
    return BT::NodeStatus::FAILURE;
  }

  if (final_pose.value().empty()) {
    RCLCPP_ERROR(node_->get_logger(), "final_pose is empty string");
    return BT::NodeStatus::FAILURE;
  }

  nav2_msgs::action::NavigateThroughPoses::Goal goal;
  try {
    goal.poses.clear();
    goal.poses.push_back(parsePose(through_pose.value()));
    if (!through_pose_2.empty()) {
      goal.poses.push_back(parsePose(through_pose_2));
    }
    goal.poses.push_back(parsePose(final_pose.value()));

    const auto stamp = node_->now();
    for (auto &pose : goal.poses) {
      pose.header.stamp = stamp;
    }
  } catch (const std::exception &e) {
    RCLCPP_ERROR(node_->get_logger(), "Invalid pose format: %s", e.what());
    return BT::NodeStatus::FAILURE;
  }

  int timeout_sec = 5;
  getInput("server_timeout", timeout_sec);

  if (!action_client_->wait_for_action_server(std::chrono::seconds(timeout_sec))) {
    RCLCPP_ERROR(node_->get_logger(), "Action server not available after %d s", timeout_sec);
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
        "Through feedback: distance remaining: %.2f, poses remaining: %d",
        feedback->distance_remaining,
        feedback->number_of_poses_remaining);

      if (release_nav_on_close_ &&
        feedback->number_of_poses_remaining <= 1 &&
        feedback->distance_remaining <= release_distance_)
      {
        release_distance_reached_.store(true);
      }
    };
  send_goal_options.result_callback =
    [this](
    const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateThroughPoses>::WrappedResult &result)
    {
      if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
        RCLCPP_INFO(node_->get_logger(), "NavigateThroughPoses succeeded");
      } else if (result.code == rclcpp_action::ResultCode::CANCELED) {
        RCLCPP_WARN(node_->get_logger(), "NavigateThroughPoses canceled");
      } else {
        RCLCPP_ERROR(node_->get_logger(), "NavigateThroughPoses failed");
      }
    };

  goal_handle_.reset();
  result_future_ = {};
  goal_future_ = action_client_->async_send_goal(goal, send_goal_options);
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SendThroughGoalsAction::onRunning()
{
  if (!goal_handle_) {
    if (goal_future_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      return BT::NodeStatus::RUNNING;
    }

    goal_handle_ = goal_future_.get();
    if (!goal_handle_) {
      RCLCPP_ERROR(node_->get_logger(), "NavigateThroughPoses goal rejected");
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
      "release_nav_on_close triggered (distance <= %.2f), cancel goal to release chassis control",
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
      publishNavDone(true);
      return BT::NodeStatus::SUCCESS;
    case rclcpp_action::ResultCode::ABORTED:
      RCLCPP_ERROR(node_->get_logger(), "NavigateThroughPoses aborted");
      return BT::NodeStatus::FAILURE;
    case rclcpp_action::ResultCode::CANCELED:
      if (cancel_requested_for_release_) {
        RCLCPP_INFO(node_->get_logger(), "NavigateThroughPoses canceled by release_nav_on_close, treat as SUCCESS");
        publishNavDone(true);
        return BT::NodeStatus::SUCCESS;
      }
      RCLCPP_WARN(node_->get_logger(), "NavigateThroughPoses canceled");
      return BT::NodeStatus::FAILURE;
    default:
      RCLCPP_ERROR(node_->get_logger(), "NavigateThroughPoses ended with unknown result");
      return BT::NodeStatus::FAILURE;
  }
}

void SendThroughGoalsAction::onHalted()
{
  if (!goal_handle_ && goal_future_.valid() &&
    goal_future_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
  {
    goal_handle_ = goal_future_.get();
  }

  if (goal_handle_) {
    RCLCPP_INFO(node_->get_logger(), "Canceling navigate_through_poses goal");
    auto cancel_future = action_client_->async_cancel_goal(goal_handle_);
    if (cancel_future.wait_for(std::chrono::seconds(2)) == std::future_status::ready) {
      auto resp = cancel_future.get();
      RCLCPP_INFO(node_->get_logger(), "Cancel response code: %d", resp->return_code);
    } else {
      RCLCPP_WARN(node_->get_logger(), "Cancel request timed out");
    }
    goal_handle_.reset();
  }

  release_distance_reached_.store(false);
  cancel_requested_for_release_ = false;
}

} // namespace rm_sentry_decision
