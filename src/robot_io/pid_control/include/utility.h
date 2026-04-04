#ifndef ROBOMASTER_UTILITY_H
#define ROBOMASTER_UTILITY_H

#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include <cmath>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>  // 必须包含这个头文件
double GetYawFromOrientation(const geometry_msgs::msg::Quaternion& orientation) {
  tf2::Quaternion q(orientation.x, orientation.y, orientation.z, orientation.w);
  tf2::Matrix3x3 m(q);
  double roll, pitch, yaw;
  m.getRPY(roll, pitch, yaw);
  return yaw;
}

double GetEuclideanDistance(const geometry_msgs::msg::PoseStamped& pose_1,
                            const geometry_msgs::msg::PoseStamped& pose_2) {
  return std::hypot(pose_1.pose.position.x - pose_2.pose.position.x,
                    pose_1.pose.position.y - pose_2.pose.position.y);
}

bool GetGlobalRobotPose(const std::shared_ptr<tf2_ros::Buffer>& tf_buffer,
                        const std::string& target_frame,
                        geometry_msgs::msg::PoseStamped& robot_global_pose) {
    // 创建一个带有当前时间的时间戳
    rclcpp::Time current_time = rclcpp::Clock(RCL_ROS_TIME).now();

    geometry_msgs::msg::PoseStamped robot_pose;
    robot_pose.header.frame_id = "base_link";
    robot_pose.header.stamp = current_time; // 使用当前时间
    robot_pose.pose.orientation.w = 1.0; // 设置为单位四元数

    try {
        // 查询目标坐标系到源坐标系的变换
        geometry_msgs::msg::TransformStamped transform_stamped;
        transform_stamped = tf_buffer->lookupTransform(
    target_frame,
    robot_pose.header.frame_id,
    tf2::TimePointZero,  // 使用最新变换
    tf2::durationFromSec(1.0)  // 缩短超时时间
);
        // 手动应用变换
        tf2::doTransform(robot_pose, robot_global_pose, transform_stamped);
    } catch (const tf2::TransformException& ex) {
        RCLCPP_ERROR(rclcpp::get_logger("utility"), "Failed to transform robot pose: %s", ex.what());
        return false;
    }
    return true;
}

void TransformPose(const tf2::Stamped<tf2::Transform>& transform,
                   const geometry_msgs::msg::PoseStamped& input_pose,
                   geometry_msgs::msg::PoseStamped& output_pose) {
    // 将 geometry_msgs::msg::PoseStamped 转换为 tf2::Transform
    tf2::Transform input_pose_tf;
    tf2::fromMsg(input_pose.pose, input_pose_tf);
    // 应用变换
    tf2::Transform output_pose_tf = transform * input_pose_tf;
    // 将 tf2::Transform 转换回 geometry_msgs::msg::PoseStamped
    output_pose.header.frame_id = transform.frame_id_; // 设置目标坐标系
    // 转换时间戳
    auto time_point = transform.stamp_.time_since_epoch();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(time_point).count();
    auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point).count() % 1000000000;
    output_pose.header.stamp = rclcpp::Time(sec, nanosec);
    // 分别处理位置和姿态
    output_pose.pose.position.x = output_pose_tf.getOrigin().getX();
    output_pose.pose.position.y = output_pose_tf.getOrigin().getY();
    output_pose.pose.position.z = output_pose_tf.getOrigin().getZ();
    tf2::Quaternion q = output_pose_tf.getRotation();
    output_pose.pose.orientation.x = q.x();
    output_pose.pose.orientation.y = q.y();
    output_pose.pose.orientation.z = q.z();
    output_pose.pose.orientation.w = q.w();
}

bool UpdateTransform(const std::shared_ptr<tf2_ros::Buffer>& tf_buffer,
                     const std::string& target_frame,
                     const std::string& source_frame,
                     const rclcpp::Time& source_time,
                     tf2::Stamped<tf2::Transform>& target_to_source_transform,
                     const rclcpp::Time& target_time = rclcpp::Clock().now(),
                     const rclcpp::Duration& timeout = std::chrono::milliseconds(500)) {
    try {
        // 正确调用 lookupTransform 函数
        geometry_msgs::msg::TransformStamped transform_stamped = tf_buffer->lookupTransform(
            target_frame, target_time,
            source_frame, source_time,
            target_frame, timeout);
        // 将 geometry_msgs::msg::TransformStamped 转换为 tf2::Stamped<tf2::Transform>
        tf2::fromMsg(transform_stamped, target_to_source_transform);
    } catch (tf2::TransformException& ex) {
        RCLCPP_ERROR(rclcpp::get_logger("utility"), "Failed to update transform: %s", ex.what());
        return false;
    }
    return true;
}


#endif // ROBOMASTER_UTILITY_H