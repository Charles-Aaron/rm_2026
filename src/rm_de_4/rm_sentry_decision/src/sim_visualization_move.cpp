#include "sim_visualization_move.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

SimVisualizationMove::SimVisualizationMove(const std::string& name, 
                                           const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name, config)
    , movement_started_(false)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("SimVisualizationMove: Failed to get ROS node from blackboard");
    }
    
    cmd_vel_pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    
    pose_manager_ = PoseManager::getInstance(node_);
    
    RCLCPP_INFO(node_->get_logger(), "SimVisualizationMove initialized");
}

BT::NodeStatus SimVisualizationMove::onStart() {
    movement_started_ = false;
    
    double linear_speed = 0.5;
    double angular_speed = 0.0;
    move_duration_ = 2.0;
    continuous_ = false;
    std::string move_type = "";
    
    getInput("linear_speed", linear_speed);
    getInput("angular_speed", angular_speed);
    getInput("move_duration", move_duration_);
    getInput("continuous", continuous_);
    getInput("move_type", move_type);
    
    current_cmd_.linear.x = 0.0;
    current_cmd_.linear.y = 0.0;
    current_cmd_.linear.z = 0.0;
    current_cmd_.angular.x = 0.0;
    current_cmd_.angular.y = 0.0;
    current_cmd_.angular.z = 0.0;
    
    if (!move_type.empty()) {
        calculateMovementFromType(move_type, linear_speed);
        RCLCPP_INFO(node_->get_logger(), 
            "========================================");
        RCLCPP_INFO(node_->get_logger(), 
            "🚗 SIM VISUALIZATION MOVE: %s", move_type.c_str());
        RCLCPP_INFO(node_->get_logger(), 
            "   linear.x=%.2f, linear.y=%.2f, angular.z=%.2f",
            current_cmd_.linear.x, current_cmd_.linear.y, current_cmd_.angular.z);
        RCLCPP_INFO(node_->get_logger(), 
            "========================================");
    } else if (pose_manager_) {
        RobotPose current_pose = pose_manager_->getCurrentPose();
        calculateMovementFromPose(current_pose, linear_speed);
        RCLCPP_INFO(node_->get_logger(), 
            "========================================");
        RCLCPP_INFO(node_->get_logger(), 
            "🚗 SIM VISUALIZATION MOVE (from pose): %s", 
            pose_manager_->poseToString(current_pose).c_str());
        RCLCPP_INFO(node_->get_logger(), 
            "   linear.x=%.2f, linear.y=%.2f, angular.z=%.2f",
            current_cmd_.linear.x, current_cmd_.linear.y, current_cmd_.angular.z);
        RCLCPP_INFO(node_->get_logger(), 
            "========================================");
    } else {
        RCLCPP_WARN(node_->get_logger(), "No move_type or pose_manager, using default forward movement");
        current_cmd_.linear.x = linear_speed;
    }
    
    start_time_ = std::chrono::steady_clock::now();
    movement_started_ = true;
    
    cmd_vel_pub_->publish(current_cmd_);
    
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SimVisualizationMove::onRunning() {
    if (!movement_started_) {
        return BT::NodeStatus::FAILURE;
    }
    
    cmd_vel_pub_->publish(current_cmd_);
    
    if (continuous_) {
        return BT::NodeStatus::RUNNING;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();
    
    if (elapsed >= move_duration_) {
        publishStopCommand();
        RCLCPP_INFO(node_->get_logger(), "SimVisualizationMove completed after %.1f seconds", move_duration_);
        return BT::NodeStatus::SUCCESS;
    }
    
    return BT::NodeStatus::RUNNING;
}

void SimVisualizationMove::onHalted() {
    publishStopCommand();
    RCLCPP_INFO(node_->get_logger(), "SimVisualizationMove halted, stopping robot");
}

void SimVisualizationMove::publishStopCommand() {
    geometry_msgs::msg::Twist stop_cmd;
    stop_cmd.linear.x = 0.0;
    stop_cmd.linear.y = 0.0;
    stop_cmd.linear.z = 0.0;
    stop_cmd.angular.x = 0.0;
    stop_cmd.angular.y = 0.0;
    stop_cmd.angular.z = 0.0;
    
    for (int i = 0; i < 10; ++i) {
        cmd_vel_pub_->publish(stop_cmd);
    }
}

void SimVisualizationMove::calculateMovementFromPose(RobotPose pose, double linear_speed) {
    switch (pose) {
        case RobotPose::ATTACK:
            current_cmd_.linear.x = linear_speed;
            current_cmd_.linear.y = 0.0;
            current_cmd_.angular.z = 0.0;
            break;
        case RobotPose::DEFENSE:
            current_cmd_.linear.x = -linear_speed;
            current_cmd_.linear.y = 0.0;
            current_cmd_.angular.z = 0.0;
            break;
        case RobotPose::MOVEMENT:
        default:
            current_cmd_.linear.x = 0.0;
            current_cmd_.linear.y = linear_speed;
            current_cmd_.angular.z = 0.0;
            break;
    }
}

void SimVisualizationMove::calculateMovementFromType(const std::string& move_type, double linear_speed) {
    if (move_type == "attack" || move_type == "进攻") {
        current_cmd_.linear.x = linear_speed;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = 0.0;
    } else if (move_type == "defense" || move_type == "防御") {
        current_cmd_.linear.x = -linear_speed;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = 0.0;
    } else if (move_type == "patrol" || move_type == "巡逻") {
        current_cmd_.linear.x = 0.0;
        current_cmd_.linear.y = linear_speed;
        current_cmd_.angular.z = 0.0;
    } else if (move_type == "rotate_left" || move_type == "左转") {
        current_cmd_.linear.x = 0.0;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = linear_speed;
    } else if (move_type == "rotate_right" || move_type == "右转") {
        current_cmd_.linear.x = 0.0;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = -linear_speed;
    } else if (move_type == "stop" || move_type == "停止") {
        current_cmd_.linear.x = 0.0;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = 0.0;
    } else {
        current_cmd_.linear.x = linear_speed;
        current_cmd_.linear.y = 0.0;
        current_cmd_.angular.z = 0.0;
    }
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::SimVisualizationMove>("SimVisualizationMove");
}

