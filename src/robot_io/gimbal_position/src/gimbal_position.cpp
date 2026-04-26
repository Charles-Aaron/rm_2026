#include <cmath>
#include <chrono>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float32.hpp"

using namespace std::chrono_literals;

class JointStatePublisher : public rclcpp::Node {
 public:
  JointStatePublisher() : Node("jointstate_publisher") {
    joint_state_pub_ =
        this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
    legacy_joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
        "serial/gimbal_joint_state", 10);

    joint_state_msg_.name = {
        "gimbal_pitch_joint",
        "gimbal_yaw_joint",
        "gimbal_pitch_odom_joint",
        "gimbal_yaw_odom_joint",
        "front_left_wheel_joint",
        "front_right_wheel_joint",
        "rear_left_wheel_joint",
        "rear_right_wheel_joint",
    };
    joint_state_msg_.position = {
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
    };
    joint_state_msg_.velocity.resize(joint_state_msg_.name.size(), 0.0);
    joint_state_msg_.effort.resize(joint_state_msg_.name.size(), 0.0);

    gimbal_angle_sub_ = this->create_subscription<std_msgs::msg::Float32>(
        "/gimbal_controller/gimbal_relative_angle", 10,
        std::bind(&JointStatePublisher::gimbalAngleCallback, this,
                  std::placeholders::_1));

    // Keep publishing even when no external gimbal data arrives so the TF tree
    // remains connected during bringup.
    timer_ = this->create_wall_timer(
        20ms, std::bind(&JointStatePublisher::publishJointStates, this));
  }

 private:
  static double NormalizeAngle(double angle) {
    double normalized = std::fmod(angle, 2.0 * M_PI);
    if (normalized < 0.0) {
      normalized += 2.0 * M_PI;
    }
    return normalized;
  }

  void gimbalAngleCallback(const std_msgs::msg::Float32::SharedPtr msg) {
    joint_state_msg_.position[1] = NormalizeAngle(msg->data);
  }

  void publishJointStates() {
    joint_state_msg_.header.stamp = this->now();
    joint_state_pub_->publish(joint_state_msg_);
    legacy_joint_state_pub_->publish(joint_state_msg_);
  }

  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr
      legacy_joint_state_pub_;
  rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr gimbal_angle_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  sensor_msgs::msg::JointState joint_state_msg_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JointStatePublisher>());
  rclcpp::shutdown();
  return 0;
}
