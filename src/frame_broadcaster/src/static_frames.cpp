#include <exception>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

#include "frame_broadcaster/level_alignment.hpp"

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
    if (declare_parameter<bool>("body_level_initialization", false)) {
      const auto alignment = sentry_frames::levelAlignment(sentry_frames::imuFromChassis(
        Eigen::Vector3d(translation[0], translation[1], translation[2]),
        Eigen::Vector3d(rpy[0], rpy[1], rpy[2])));
      const Eigen::Quaterniond q(alignment.odom_from_lio.linear());
      const auto & p = alignment.odom_from_lio.translation();
      // 与前端非单位初始姿态配套；机械安装 TF 本身不变。
      odom_transform.transform = tf2::toMsg(tf2::Transform(
        tf2::Quaternion(q.x(), q.y(), q.z(), q.w()), tf2::Vector3(p.x(), p.y(), p.z())));
    }

    std::vector<geometry_msgs::msg::TransformStamped> transforms{odom_transform, extrinsic};
    const auto lidar = declare_parameter<std::string>("lidar_frame", "");
    if (!lidar.empty()) {
      const auto lidar_translation = declare_parameter<std::vector<double>>(
        "imu_to_lidar_translation", {0.0, 0.0, 0.0});
      const auto lidar_rpy = declare_parameter<std::vector<double>>(
        "imu_to_lidar_rpy", {0.0, 0.0, 0.0});
      if (lidar == imu || lidar == chassis || lidar == odom || lidar == imu_odom ||
          lidar_translation.size() != 3 || lidar_rpy.size() != 3) {
        throw std::invalid_argument("Invalid lidar frame or extrinsic dimensions");
      }
      for (double value : lidar_translation) if (!std::isfinite(value)) {
        throw std::invalid_argument("Lidar translation must be finite");
      }
      for (double value : lidar_rpy) if (!std::isfinite(value)) {
        throw std::invalid_argument("Lidar rotation must be finite");
      }
      tf2::Quaternion lidar_rotation;
      lidar_rotation.setRPY(lidar_rpy[0], lidar_rpy[1], lidar_rpy[2]);
      geometry_msgs::msg::TransformStamped lidar_transform;
      lidar_transform.header = extrinsic.header;
      lidar_transform.child_frame_id = lidar;
      lidar_transform.transform = tf2::toMsg(tf2::Transform(lidar_rotation,
        tf2::Vector3(lidar_translation[0], lidar_translation[1], lidar_translation[2])));
      transforms.push_back(lidar_transform);
    }
    broadcaster_ = std::make_unique<tf2_ros::StaticTransformBroadcaster>(*this);
    broadcaster_->sendTransform(transforms);
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
