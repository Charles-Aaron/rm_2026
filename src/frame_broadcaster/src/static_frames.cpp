#include <exception>
#include <memory>
#include <stdexcept>
#include <vector>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/static_transform_broadcaster.h"

class FrameBroadcaster : public rclcpp::Node
{
public:
  FrameBroadcaster() : Node("frame_broadcaster")
  {
    const auto imu = declare_parameter<std::string>("imu_frame", "imu_link");
    const auto chassis = declare_parameter<std::string>("chassis_frame", "chassis_link");
    const auto odom = declare_parameter<std::string>("odom_frame", "odom");
    const auto imu_odom = declare_parameter<std::string>("imu_odom_frame", "imu_odom");
    const auto translation = declare_parameter<std::vector<double>>(
      "imu_to_chassis_translation", {0.0, 0.0, 0.0});
    const auto rpy = declare_parameter<std::vector<double>>(
      "imu_to_chassis_rpy", {0.0, 0.0, 0.0});
    if (translation.size() != 3 || rpy.size() != 3) {
      throw std::invalid_argument("IMU extrinsic translation and RPY must each contain three values");
    }

    tf2::Quaternion rotation;
    rotation.setRPY(rpy[0], rpy[1], rpy[2]);
    const tf2::Transform imu_to_chassis(
      rotation, tf2::Vector3(translation[0], translation[1], translation[2]));
    geometry_msgs::msg::TransformStamped extrinsic;
    extrinsic.header.stamp = now();
    extrinsic.header.frame_id = imu;
    extrinsic.child_frame_id = chassis;
    extrinsic.transform = tf2::toMsg(imu_to_chassis);

    geometry_msgs::msg::TransformStamped odom_transform;
    odom_transform.header.stamp = extrinsic.header.stamp;
    odom_transform.header.frame_id = odom;
    odom_transform.child_frame_id = imu_odom;
    odom_transform.transform = tf2::toMsg(imu_to_chassis.inverse());

    broadcaster_ = std::make_unique<tf2_ros::StaticTransformBroadcaster>(*this);
    broadcaster_->sendTransform(
      std::vector<geometry_msgs::msg::TransformStamped>{odom_transform, extrinsic});
    RCLCPP_INFO(
      get_logger(), "Published %s->%s and %s->%s static transforms",
      odom.c_str(), imu_odom.c_str(), imu.c_str(), chassis.c_str());
  }

private:
  std::unique_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int result = 0;
  try {
    rclcpp::spin(std::make_shared<FrameBroadcaster>());
  } catch (const std::exception & error) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("frame_broadcaster"), "%s", error.what());
      result = 1;
    }
  }
  rclcpp::shutdown();
  return result;
}
