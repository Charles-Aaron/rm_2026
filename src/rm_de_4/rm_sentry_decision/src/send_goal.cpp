#include "send_goal.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"

#include <sstream>
#include <vector>

namespace rm_sentry_decision
{

SendGoalAction::SendGoalAction(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::StatefulActionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
    }

    std::string action_name;
    if(!getInput("action_name", action_name)) {
        action_name = "navigate_to_pose";
    }

    action_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(
        node_, action_name);

    auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local();
    nav_done_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/navigation_done", qos);
}

geometry_msgs::msg::PoseStamped SendGoalAction::parseGoalPose(const std::string& pose_str)
{
    geometry_msgs::msg::PoseStamped pose;
    std::stringstream ss(pose_str);
    std::string token;
    std::vector<std::string> tokens;

    while(std::getline(ss, token, ';')) {
        if(!token.empty()){
            tokens.push_back(token);
        }
    }

    if(tokens.size() != 7) {
        throw std::runtime_error("goal_pose 需要 7 个用分号分隔的数字字段，实际数量 " + std::to_string(tokens.size()));
    }

    try {
        pose.pose.position.x = std::stod(tokens[0]);
        pose.pose.position.y = std::stod(tokens[1]);
        pose.pose.position.z = std::stod(tokens[2]);
        pose.pose.orientation.x = std::stod(tokens[3]);
        pose.pose.orientation.y = std::stod(tokens[4]);
        pose.pose.orientation.z = std::stod(tokens[5]);
        pose.pose.orientation.w = std::stod(tokens[6]);
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("解析 goal_pose 失败: ") + e.what());
    }

    pose.header.frame_id = "map";
    pose.header.stamp = node_->now();
    return pose;
}

void SendGoalAction::publishNavDone(bool done)
{
    std_msgs::msg::Bool msg;
    msg.data = done;
    nav_done_pub_->publish(msg);
}

BT::NodeStatus SendGoalAction::onStart()
{
    if (!rclcpp::ok()) {
        return BT::NodeStatus::FAILURE;
    }

    publishNavDone(false);
    goal_handle_.reset();
    goal_future_ = {};
    result_future_ = {};
    goal_start_time_ = std::chrono::steady_clock::now();
    getInput("goal_timeout_sec", goal_timeout_sec_);
    if (goal_timeout_sec_ <= 0.0) {
        goal_timeout_sec_ = 60.0;
    }

    auto goal_pose = getInput<std::string>("goal_pose");
    if(!goal_pose) {
        RCLCPP_ERROR(node_->get_logger(), "单点导航：缺少必填输入 [goal_pose]");
        return BT::NodeStatus::FAILURE;
    }

    if(goal_pose.value().empty()) {
        RCLCPP_ERROR(node_->get_logger(), "单点导航：goal_pose 为空");
        return BT::NodeStatus::FAILURE;
    }

    auto goal = nav2_msgs::action::NavigateToPose::Goal();
    try {
        goal.pose = parseGoalPose(goal_pose.value());
    } catch(const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "单点导航：goal_pose 格式错误: %s", e.what());
        return BT::NodeStatus::FAILURE;
    }

    int timeout_sec = 5;
    getInput("server_timeout", timeout_sec);
    if(!action_client_->wait_for_action_server(std::chrono::seconds(timeout_sec))) {
        if (rclcpp::ok()) {
            RCLCPP_ERROR(node_->get_logger(), "单点导航：等待导航 action 服务 %d 秒后仍不可用", timeout_sec);
        }
        return BT::NodeStatus::FAILURE;
    }

    auto send_goal_options = rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SendGoalOptions();
    send_goal_options.feedback_callback =
        [this](rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr,
               const std::shared_ptr<const nav2_msgs::action::NavigateToPose::Feedback>& feedback) {
            RCLCPP_DEBUG_THROTTLE(
                node_->get_logger(), *node_->get_clock(), 1000,
                "单点导航反馈：剩余距离 %.2f 米", feedback->distance_remaining);
        };

    goal_future_ = action_client_->async_send_goal(goal, send_goal_options);
    RCLCPP_INFO(node_->get_logger(), "单点导航：已发送目标点 %s", goal_pose.value().c_str());
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SendGoalAction::onRunning()
{
    if (!rclcpp::ok()) {
        return BT::NodeStatus::FAILURE;
    }

    const double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - goal_start_time_).count();
    if (elapsed > goal_timeout_sec_) {
        RCLCPP_WARN(node_->get_logger(), "单点导航：超过 %.1f 秒未到达，判定超时", goal_timeout_sec_);
        if (goal_handle_) {
            action_client_->async_cancel_goal(goal_handle_);
        }
        return BT::NodeStatus::FAILURE;
    }

    if (!goal_handle_) {
        if (!goal_future_.valid() ||
            goal_future_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            return BT::NodeStatus::RUNNING;
        }

        goal_handle_ = goal_future_.get();
        if(!goal_handle_) {
            RCLCPP_ERROR(node_->get_logger(), "单点导航：目标被导航服务拒绝");
            return BT::NodeStatus::FAILURE;
        }
        result_future_ = action_client_->async_get_result(goal_handle_);
        return BT::NodeStatus::RUNNING;
    }

    if (!result_future_.valid() ||
        result_future_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return BT::NodeStatus::RUNNING;
    }

    const auto result = result_future_.get();
    switch(result.code) {
        case rclcpp_action::ResultCode::SUCCEEDED:
            publishNavDone(true);
            RCLCPP_INFO(node_->get_logger(), "单点导航：到达目标点");
            return BT::NodeStatus::SUCCESS;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(node_->get_logger(), "单点导航：导航被中止");
            return BT::NodeStatus::FAILURE;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_WARN(node_->get_logger(), "单点导航：导航被取消");
            return BT::NodeStatus::FAILURE;
        default:
            RCLCPP_ERROR(node_->get_logger(), "单点导航：导航返回未知结果");
            return BT::NodeStatus::FAILURE;
    }
}

void SendGoalAction::onHalted()
{
    if (!rclcpp::ok()) {
        return;
    }

    if(!goal_handle_ && goal_future_.valid() &&
       goal_future_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    {
        goal_handle_ = goal_future_.get();
    }

    if(goal_handle_) {
        RCLCPP_INFO(node_->get_logger(), "单点导航：正在取消当前导航目标");
        action_client_->async_cancel_goal(goal_handle_);
        goal_handle_.reset();
    }
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::SendGoalAction>("SendGoalAction");
}
