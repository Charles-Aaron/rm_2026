#include "enemy.hpp"
#include <sstream>
#include <iomanip>

namespace rm_sentry_decision
{

Enemy::Enemy(
    const std::string& name,
    const BT::NodeConfiguration& config)
  : BT::StatefulActionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }

    /*========  固定 action 名  ========*/
    action_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(
        node_, "navigate_to_pose");

    /*========  新增：订阅 enemy_pose  ========*/
    sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            enemy_x_.store(msg->enemy_x);
            enemy_y_.store(msg->enemy_y);
        });
        nav_done_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/navigation_done", 10);
}

BT::NodeStatus Enemy::onStart()
{
    rclcpp::spin_some(node_);
    const double x = enemy_x_.load();
    const double y = enemy_y_.load();

    /*========  零坐标直接失败  ========*/
    if (x == 0.0 || y == 0.0) {return BT::NodeStatus::FAILURE;}

    /*========  直接构造导航目标  ========*/
    nav2_msgs::action::NavigateToPose::Goal goal;
    goal.pose.header.frame_id = "map";
    goal.pose.header.stamp    = node_->now();
    goal.pose.pose.position.x = x;
    goal.pose.pose.position.y = y;
    goal.pose.pose.position.z = 0.0;
    goal.pose.pose.orientation.x = 0; 
    goal.pose.pose.orientation.y = 0; 
    goal.pose.pose.orientation.z = 0; 
    goal.pose.pose.orientation.w = 1.0;   // 单位四元数
 msg.data = true;

    for (int i = 0; i < 100; ++i) {
        nav_done_pub_->publish(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 200 Hz
    }
     if(!action_client_->wait_for_action_server(std::chrono::seconds(5))) {
        RCLCPP_ERROR(node_->get_logger(), "Action server not available after 5s");
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

BT::NodeStatus Enemy::onRunning()
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

void Enemy::onHalted()
{
    if (goal_handle_) {
        action_client_->async_cancel_goal(goal_handle_);
        goal_handle_.reset();
    }
}
} // namespace rm_sentry_decision