#include "send_goal.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include <sstream>
#include <iomanip>

namespace rm_sentry_decision
{

SendGoalAction::SendGoalAction(
    const std::string& name, 
    const BT::NodeConfiguration& config)
  : BT::StatefulActionNode(name, config)
{
    // 从blackboard获取ROS2节点
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取动作名称
    std::string action_name;
    if(!getInput("action_name", action_name)) {
        action_name = "navigate_to_pose";
    }
    
    // 创建动作客户端
    action_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(
        node_, action_name);
    nav_done_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/navigation_done", 10);
}

geometry_msgs::msg::PoseStamped SendGoalAction::parseGoalPose(const std::string& pose_str)
{
    geometry_msgs::msg::PoseStamped pose;
    std::stringstream ss(pose_str);
    std::string token;
    std::vector<std::string> tokens;
    
    while(std::getline(ss, token, ';')) {
        // 去除可能的空白
        if(!token.empty()){
            tokens.push_back(token);
        }
    }

    // 基本格式校验
    if(tokens.size() != 7) {
        throw std::runtime_error("goal_pose requires 7 semicolon-separated numeric fields, got " + std::to_string(tokens.size()));
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
        throw std::runtime_error(std::string("Failed to parse goal_pose: ") + e.what());
    }
    
    pose.header.frame_id = "map";
    pose.header.stamp = node_->now();
    return pose;
}

BT::NodeStatus SendGoalAction::onStart()
{
     msg.data = false;

    for (int i = 0; i < 100; ++i) {
        nav_done_pub_->publish(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 200 Hz
    }
    RCLCPP_INFO(node_->get_logger(), "SendGoalAction started");
    auto goal_pose = getInput<std::string>("goal_pose");
    if(!goal_pose) {
        RCLCPP_ERROR(node_->get_logger(), "Missing required input [goal_pose]");
        return BT::NodeStatus::FAILURE;
    }

    // 添加空字符串检查
    if(goal_pose.value().empty()) {
        RCLCPP_ERROR(node_->get_logger(), "goal_pose is empty string");
        return BT::NodeStatus::FAILURE;
    }

    auto goal = nav2_msgs::action::NavigateToPose::Goal();
    try {
        goal.pose = parseGoalPose(goal_pose.value());
    } catch(const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "Invalid goal_pose format: %s", e.what());
        return BT::NodeStatus::FAILURE;
    }

    // 获取等待 action server 的超时时长 (秒)
    int timeout_sec = 5;
    getInput("server_timeout", timeout_sec);

    if(!action_client_->wait_for_action_server(std::chrono::seconds(timeout_sec))) {
        RCLCPP_ERROR(node_->get_logger(), "Action server not available after %d s", timeout_sec);
        return BT::NodeStatus::FAILURE;
    }

    auto send_goal_options = rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SendGoalOptions();
    send_goal_options.feedback_callback = [this](rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr,
                                                 const std::shared_ptr<const nav2_msgs::action::NavigateToPose::Feedback>& feedback) {
        // 处理反馈信息
        RCLCPP_INFO(node_->get_logger(), "Feedback: Distance remaining: %.2f", feedback->distance_remaining);
    };
    send_goal_options.result_callback = [this](const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::WrappedResult& result) {
        // 处理结果信息
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(node_->get_logger(), "Navigation succeeded!");

        } else if (result.code == rclcpp_action::ResultCode::CANCELED) {
            RCLCPP_INFO(node_->get_logger(), "Navigation canceled!");

        } else {
            RCLCPP_INFO(node_->get_logger(), "Navigation failed!");
        }
    };

     future = action_client_->async_send_goal(goal, send_goal_options);
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SendGoalAction::onRunning()
{
    
   if (future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // 动作仍在进行中，或者 future 还未准备好
        return BT::NodeStatus::RUNNING;
    }
    goal_handle_ = future.get();
    if(!goal_handle_) {
        RCLCPP_ERROR(node_->get_logger(), "No active goal handle");
        return BT::NodeStatus::FAILURE;
    }

    auto status = goal_handle_->get_status();
    switch(status) {
        case action_msgs::msg::GoalStatus::STATUS_SUCCEEDED:
            // RCLCPP_INFO(node_->get_logger(), "Navigation succeeded!");
             msg.data = true;

    for (int i = 0; i < 100; ++i) {
        nav_done_pub_->publish(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 200 Hz
    }
            return BT::NodeStatus::SUCCESS;
        case action_msgs::msg::GoalStatus::STATUS_ABORTED:
            // RCLCPP_ERROR(node_->get_logger(), "Navigation aborted");
            return BT::NodeStatus::FAILURE;
        case action_msgs::msg::GoalStatus::STATUS_CANCELED:
            // RCLCPP_WARN(node_->get_logger(), "Navigation canceled");
            return BT::NodeStatus::FAILURE;
        default:
            return BT::NodeStatus::RUNNING;
    }
}

void SendGoalAction::onHalted()
{
    // 如果 future 已经完成但 goal_handle_ 为空，尝试获取一次
    if(!goal_handle_ && future.valid() &&
       future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    {
        goal_handle_ = future.get();
    }

    if(goal_handle_) {
        RCLCPP_INFO(node_->get_logger(), "Canceling navigation goal");
        auto cancel_future = action_client_->async_cancel_goal(goal_handle_);

        // 等待取消结果，最长 2 秒
        if(cancel_future.wait_for(std::chrono::seconds(2)) == std::future_status::ready) {
            auto resp = cancel_future.get();
            RCLCPP_INFO(node_->get_logger(), "Cancel response code: %d", resp->return_code);
        } else {
            RCLCPP_WARN(node_->get_logger(), "Cancel request timed out");
        }
        goal_handle_.reset();
    }
}

static BT::PortsList providedPorts() {
    return {
      BT::InputPort<uint16_t>("remain_time"),
      BT::InputPort<uint16_t>("robot1_hp"),
      BT::InputPort<uint16_t>("robot3_hp"),
      BT::InputPort<uint16_t>("robot4_hp"),
      /* 其他端口 … */
    };
}

} // namespace rm_behavior_tree

// v3版本注册

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::SendGoalAction>("SendGoalAction");
}