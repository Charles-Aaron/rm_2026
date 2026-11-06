#ifndef ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_
#define ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_

#include <string>

#include "nav_msgs/msg/odometry.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

namespace odom_frame_adapter
{
inline tf2::Quaternion normalized(const geometry_msgs::msg::Quaternion & message)
{
  tf2::Quaternion rotation(message.x, message.y, message.z, message.w);
  // Match the original fallback for an unset quaternion.
  if (rotation.length2() < 1.0e-24) {
    return tf2::Quaternion(0.0, 0.0, 0.0, 1.0);
  }
  return rotation.normalized();
}

inline nav_msgs::msg::Odometry convert_odometry(
  const nav_msgs::msg::Odometry & message,
  const geometry_msgs::msg::Transform & transform,
  const std::string & target_frame)
{
  const auto & pose = message.pose.pose;
  const auto & offset = transform.translation;
  const tf2::Transform parent_source(
    normalized(pose.orientation), tf2::Vector3(pose.position.x, pose.position.y, pose.position.z));
  const tf2::Quaternion source_target_q = normalized(transform.rotation);
  const tf2::Vector3 source_target_t(offset.x, offset.y, offset.z);
  const tf2::Transform source_target(source_target_q, source_target_t);

  nav_msgs::msg::Odometry converted;
  converted.header = message.header;
  converted.child_frame_id = target_frame;
  // Uncertainty propagation is a separate change from this language migration.
  converted.pose.covariance = message.pose.covariance;
  converted.twist.covariance = message.twist.covariance;
  tf2::toMsg(parent_source * source_target, converted.pose.pose);

  const auto & linear = message.twist.twist.linear;
  const auto & angular = message.twist.twist.angular;
  const tf2::Vector3 source_linear(linear.x, linear.y, linear.z);
  const tf2::Vector3 source_angular(angular.x, angular.y, angular.z);
  const tf2::Quaternion target_source_q = source_target_q.inverse();
  // Shift velocity to the chassis origin before rotating into the chassis axes.
  const auto target_linear = tf2::quatRotate(
    target_source_q, source_linear + source_angular.cross(source_target_t));
  const auto target_angular = tf2::quatRotate(target_source_q, source_angular);
  converted.twist.twist.linear = tf2::toMsg(target_linear);
  converted.twist.twist.angular = tf2::toMsg(target_angular);
  return converted;
}
}  // namespace odom_frame_adapter

#endif  // ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_
