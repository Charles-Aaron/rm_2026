#include "action_home.h"
using namespace std::chrono_literals;
action_home::action_home(const std::string& name, const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name, config) {
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    client_ = rclcpp_action::create_client<NavigateToPose>(node_, "navigate_to_pose");
    sub_ = node_->create_subscription<customize_messages::msg::Remotedata>(
        "/remote_data", 10, [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
            // 检查标志位
            if ( msg->supply_flag || msg->operator_control||(msg->home_flag==0)) {
                interrupt_navigation_ = 1;
            }
            else{interrupt_navigation_ = 0;}

            // //计算和隧道的距离
            // geometry_msgs::msg::TransformStamped transformStamped;
            // try {
            //     transformStamped = tf_buffer_->lookupTransform(
            //         "map", "chassis", rclcpp::Time(0), rclcpp::Duration(1s));
            // } catch (const tf2::TransformException& ex) {
            //     RCLCPP_ERROR(node_->get_logger(), "TF lookup failed: %s", ex.what());
            // }
            // double goal_x = 0.0;
            // double goal_y = 0.0;
            // double goal_x_1 = 1.0;
            // double goal_y_1 = 1.0;
            // double distance = std::sqrt(
            //     std::pow(transformStamped.transform.translation.x - goal_x, 2) + std::pow(transformStamped.transform.translation.y - goal_y, 2)
            // );
            // double distance1 = std::sqrt(
            //     std::pow(transformStamped.transform.translation.x- goal_x_1, 2) + std::pow(transformStamped.transform.translation.y - goal_y_1, 2)
            // );
            // tf2::Quaternion q(
            //     transformStamped.transform.rotation.x,
            //     transformStamped.transform.rotation.y,
            //     transformStamped.transform.rotation.z,
            //     transformStamped.transform.rotation.w
            // );
            // double yaw = tf2::getYaw(q);
            // std_msgs::msg::Float32 yaw_msg;
            // yaw_msg.data = yaw;
            // if (distance < 1.2||distance1 < 1.2)
            // {
            //     chassis_yaw_pub_->publish(yaw_msg);
            //     RCLCPP_INFO(node_->get_logger(), "Reached tunnel_goal!");
            // }
        });
    goals_.resize(1);

    // 目标点1
    goals_[0].pose.position.x = -0.624;
    goals_[0].pose.position.y = -5.26;
    goals_[0].pose.orientation.w = 1.0;

    // // 目标点2
    // goals_[1].pose.position.x = -1.0;
    // goals_[1].pose.position.y = -2.0;
    // goals_[1].pose.orientation.w = 1.0;

    // 设置目标点的header
    for (auto& goal : goals_) {
        goal.header.frame_id = "map";
        goal.header.stamp = node_->get_clock()->now();
    }
}

BT::PortsList action_home::providedPorts() {
    return {};  // 不再使用端口
}

BT::NodeStatus action_home::onStart() {
    is_goal_done_ = false;
    is_goal_success_ = false;
    if (interrupt_navigation_) {
        RCLCPP_INFO(node_->get_logger(), "Interrupted navigation due to remote data flag");
        return BT::NodeStatus::FAILURE;
     }
    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose = goals_[current_goal_index_];  // 直接使用定义好的目标点

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

BT::NodeStatus action_home::onRunning() {
    if (interrupt_navigation_) {
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
            // if (current_goal_index_ < 1) { //当一个点导航成功时进入下一个点进行导航点切换
            //     current_goal_index_++;
            // } else {
            //     current_goal_index_ = 0;
            // }
            return BT::NodeStatus::SUCCESS;
        } else {
            // if (current_goal_index_ < 1) {
            //     current_goal_index_++;
            // } else {
            //     current_goal_index_ = 0;
            // }
            return BT::NodeStatus::FAILURE;
        }
    }

    // 动作仍在进行中
    return BT::NodeStatus::RUNNING;
}
    
void action_home::onHalted() {
    if (client_ && goal_handle_) {
        client_->async_cancel_goal(goal_handle_);  // 通过客户端对象取消目标‌:ml-citation{ref="2,3" data="citationList"}
        RCLCPP_INFO(node_->get_logger(), "Navigation halted");
    }
}