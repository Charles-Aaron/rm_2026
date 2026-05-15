#include "action_follow.h"
using namespace std::chrono_literals;

action_follow::action_follow(const std::string& name, const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name, config) {
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    client_ = rclcpp_action::create_client<NavigateToPose>(node_, "navigate_to_pose");
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(node_->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
        "/remote_data", 10, [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
            enemy_distance_ = msg->enemy_distance;
            enemy_angle_ = msg->enemy_angle;
            interrupt_navigation_ = msg->operator_control ? 1 : 0;
        });

}

BT::PortsList action_follow::providedPorts() {
    return {};  // 不再使用端口
}

BT::NodeStatus action_follow::onStart() {
    is_goal_done_ = false;
    is_goal_success_ = false;
    if (interrupt_navigation_) {
        RCLCPP_INFO(node_->get_logger(), "Interrupted navigation due to remote data flag");
        return BT::NodeStatus::FAILURE;
     }

    geometry_msgs::msg::TransformStamped chassis_tf;
    try {
        chassis_tf = tf_buffer_->lookupTransform(
            "map", "chassis", rclcpp::Time(0), rclcpp::Duration(100ms));
    } catch (const tf2::TransformException& ex) {
        RCLCPP_WARN(node_->get_logger(), "TF lookup failed in onStart: %s", ex.what());
        return BT::NodeStatus::FAILURE;
    }

    tf2::Quaternion q(
        chassis_tf.transform.rotation.x,
        chassis_tf.transform.rotation.y,
        chassis_tf.transform.rotation.z,
        chassis_tf.transform.rotation.w
    );
    double yaw = tf2::getYaw(q);

    double enemy_angle_rad = enemy_angle_ * M_PI / 180.0;
    double adjusted_distance = std::max(0.0, enemy_distance_ - 1.5);

    double goal_yaw = enemy_angle_rad + yaw;
    tf2::Quaternion goal_q;
    goal_q.setRPY(0.0, 0.0, goal_yaw);

    current_goal_.header.frame_id = "map";
    current_goal_.header.stamp = node_->get_clock()->now();
    current_goal_.pose.position.x = chassis_tf.transform.translation.x + 0.5 * adjusted_distance * cos(goal_yaw);
    current_goal_.pose.position.y = chassis_tf.transform.translation.y + 0.5 * adjusted_distance * sin(goal_yaw);
    current_goal_.pose.orientation.x = goal_q.x();
    current_goal_.pose.orientation.y = goal_q.y();
    current_goal_.pose.orientation.z = goal_q.z();
    current_goal_.pose.orientation.w = goal_q.w();

    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose = current_goal_;
    // 检查动作服务器是否可用
    if (!client_->wait_for_action_server(std::chrono::seconds(10))) {
        RCLCPP_ERROR(node_->get_logger(), "NavigateToPose action server not available after waiting");
        return BT::NodeStatus::FAILURE;
    }

    auto send_goal_options = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();
    send_goal_options.feedback_callback = [this](rclcpp_action::ClientGoalHandle<NavigateToPose>::SharedPtr,
                                                 const std::shared_ptr<const NavigateToPose::Feedback>& feedback) {
        // 处理反馈信息
        RCLCPP_INFO(node_->get_logger(), "Feedback: Distance remaining: %.2f", feedback->distance_remaining);
    };
    send_goal_options.result_callback = [this](const rclcpp_action::ClientGoalHandle<NavigateToPose>::WrappedResult& result) {
        // 处理结果信息
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(node_->get_logger(), "Navigation succeeded!");
            is_goal_done_ = true;
            is_goal_success_ = true;
        } else if (result.code == rclcpp_action::ResultCode::CANCELED) {
            RCLCPP_INFO(node_->get_logger(), "Navigation canceled!");
            is_goal_done_ = true;
        } else {
            RCLCPP_INFO(node_->get_logger(), "Navigation failed!");
            is_goal_done_ = true;
        }
    };

    try {
        future_goal_handle_ = client_->async_send_goal(goal_msg, send_goal_options);
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "Failed to send goal: %s", e.what());
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus action_follow::onRunning() {
    if (interrupt_navigation_) {
        if (client_ && goal_handle_) {
            try {
                client_->async_cancel_goal(goal_handle_);
            } catch (const rclcpp_action::exceptions::UnknownGoalHandleError& e) {
                RCLCPP_WARN(node_->get_logger(), "Cancel: unknown goal handle: %s", e.what());
            }
        }
        RCLCPP_INFO(node_->get_logger(), "Interrupted navigation due to remote data flag");
        return BT::NodeStatus::FAILURE;
     }
    // 检查 future_goal_handle_ 是否准备好
    if (future_goal_handle_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        // 动作仍在进行中，或者 future 还未准备好
        return BT::NodeStatus::RUNNING;
    }

    // 获取 goal_handle
    goal_handle_ = future_goal_handle_.get();
    if (!goal_handle_) {
        // 如果 goal_handle_ 为空，说明发送目标失败或动作服务器未响应
        RCLCPP_ERROR(node_->get_logger(), "Failed to get goal handle or action server not responding");
        return BT::NodeStatus::FAILURE;
    }

    // 检查动作是否完成
    if (is_goal_done_) {
        // 动作已完成，根据 is_goal_success_ 返回状态
        if (is_goal_success_) {
            return BT::NodeStatus::SUCCESS;
        } else {
            return BT::NodeStatus::FAILURE;
        }
    }

    // 动作仍在进行中
    return BT::NodeStatus::RUNNING;
}
    
void action_follow::onHalted() {
    if (client_ && goal_handle_) {
        client_->async_cancel_goal(goal_handle_);  // 通过客户端对象取消目标‌:ml-citation{ref="2,3" data="citationList"}
        RCLCPP_INFO(node_->get_logger(), "Navigation halted");
    }
}
