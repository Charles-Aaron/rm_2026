#include <algorithm>
#include <chrono>
#include <exception>
#include <memory>

#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/create_timer.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/transform_listener.h"

class InitialPose : public rclcpp::Node
{
public:
  InitialPose() : Node("initial_pose")
  {
    map_frame_ = declare_parameter<std::string>("map_frame", "map");
    odom_frame_ = declare_parameter<std::string>("odom_frame", "odom");
    base_frame_ = declare_parameter<std::string>("base_frame", "chassis_link");
    const auto pose_topic = declare_parameter<std::string>("pose_topic", "initialpose");
    const double frequency = declare_parameter<double>("publish_frequency", 20.0);
    const double x = declare_parameter<double>("initial_x", 0.0);
    const double y = declare_parameter<double>("initial_y", 0.0);
    const double z = declare_parameter<double>("initial_z", 0.0);
    const double yaw = declare_parameter<double>("initial_yaw", 0.0);
    tf2::Quaternion rotation;
    rotation.setRPY(0.0, 0.0, yaw);
    map_to_odom_ = tf2::Transform(rotation, tf2::Vector3(x, y, z));

    buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
    // Use this node's interfaces so launch TF remappings and clock apply to the listener.
    listener_ = std::make_unique<tf2_ros::TransformListener>(*buffer_, this, true);
    broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    subscription_ = create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
      pose_topic, 10,
      [this](geometry_msgs::msg::PoseWithCovarianceStamped::ConstSharedPtr message) {
        apply_pose(*message);
      });
    // Match rclpy's ROS-clock timer, including use_sim_time and /clock pauses.
    timer_ = rclcpp::create_timer(
      this, get_clock(), std::chrono::duration<double>(1.0 / std::max(frequency, 1.0)),
      [this]() {publish_transform();});
    RCLCPP_INFO(
      get_logger(), "Publishing %s->%s; initial alignment: [%g, %g, %g], yaw %g",
      map_frame_.c_str(), odom_frame_.c_str(), x, y, z, yaw);
  }

private:
  void apply_pose(const geometry_msgs::msg::PoseWithCovarianceStamped & message)
  {
    const auto & frame = message.header.frame_id.empty() ? map_frame_ : message.header.frame_id;
    if (frame != map_frame_) {
      RCLCPP_WARN(
        get_logger(), "Ignoring initial pose in %s; expected %s", frame.c_str(), map_frame_.c_str());
      return;
    }
    try {
      // Preserve the original latest-TF behavior rather than using the RViz timestamp.
      const auto transform = buffer_->lookupTransform(odom_frame_, base_frame_, tf2::TimePointZero);
      tf2::Transform odom_to_base;
      tf2::Transform map_to_base;
      tf2::fromMsg(transform.transform, odom_to_base);
      tf2::fromMsg(message.pose.pose, map_to_base);
      map_to_odom_ = map_to_base * odom_to_base.inverse();
      RCLCPP_INFO(get_logger(), "Updated map->odom from RViz initial pose");
    } catch (const tf2::TransformException & error) {
      RCLCPP_WARN(get_logger(), "Cannot apply initial pose yet: %s", error.what());
    }
  }

  void publish_transform()
  {
    geometry_msgs::msg::TransformStamped message;
    message.header.stamp = now();
    message.header.frame_id = map_frame_;
    message.child_frame_id = odom_frame_;
    message.transform = tf2::toMsg(map_to_odom_);
    broadcaster_->sendTransform(message);
  }

  std::string map_frame_, odom_frame_, base_frame_;
  tf2::Transform map_to_odom_;
  std::unique_ptr<tf2_ros::Buffer> buffer_;
  std::unique_ptr<tf2_ros::TransformListener> listener_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
  rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int result = 0;
  try {
    // Pose updates and TF publication share the default mutually exclusive callback group.
    rclcpp::spin(std::make_shared<InitialPose>());
  } catch (const std::exception & error) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("initial_pose"), "%s", error.what());
      result = 1;
    }
  }
  rclcpp::shutdown();
  return result;
}
