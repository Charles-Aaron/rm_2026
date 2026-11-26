#ifndef ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_
#define ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

#include <Eigen/Core>
#include "nav_msgs/msg/odometry.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

namespace odom_frame_adapter
{
using Matrix6 = Eigen::Matrix<double, 6, 6>;
using Covariance = nav_msgs::msg::Odometry::_pose_type::_covariance_type;
using CovarianceMatrix = Eigen::Matrix<double, 6, 6, Eigen::RowMajor>;

inline tf2::Quaternion normalized(const geometry_msgs::msg::Quaternion & message)
{
  tf2::Quaternion rotation(message.x, message.y, message.z, message.w);
  if (!std::isfinite(rotation.length2())) {
    throw std::invalid_argument("nonfinite quaternion");
  }
  // 保留旧接口对未设置四元数的单位旋转回退。
  if (rotation.length2() < 1.0e-24) {
    return tf2::Quaternion(0.0, 0.0, 0.0, 1.0);
  }
  return rotation.normalized();
}

inline Eigen::Matrix3d matrix(const tf2::Matrix3x3 & rotation)
{
  Eigen::Matrix3d result;
  for (int row = 0; row < 3; ++row) {
    for (int column = 0; column < 3; ++column) {
      result(row, column) = rotation[row][column];
    }
  }
  return result;
}

inline Eigen::Matrix3d skew(const tf2::Vector3 & value)
{
  Eigen::Matrix3d result;
  result << 0, -value.z(), value.y(), value.z(), 0, -value.x(),
    -value.y(), value.x(), 0;
  return result;
}

inline Covariance propagate(const Covariance & covariance, const Matrix6 & jacobian)
{
  Covariance result;
  Eigen::Map<CovarianceMatrix> output(result.data());
  output.noalias() = jacobian * Eigen::Map<const CovarianceMatrix>(covariance.data()) *
    jacobian.transpose();
  return result;
}

// 两段外参均为固定变换；缓存变换及速度雅可比，运行中只更新位姿相关项。
class OdometryConverter
{
public:
  OdometryConverter(
    const geometry_msgs::msg::Transform & output_from_input,
    const geometry_msgs::msg::Transform & source_from_target,
    std::string output_frame, std::string target_frame)
  : output_from_input_(transform(output_from_input)),
    source_from_target_(transform(source_from_target)),
    rotation_(matrix(output_from_input_.getBasis())),
    output_frame_(std::move(output_frame)), target_frame_(std::move(target_frame))
  {
    const Eigen::Matrix3d body_rotation = matrix(source_from_target_.getBasis().transpose());
    twist_jacobian_.topLeftCorner<3, 3>() = body_rotation;
    twist_jacobian_.topRightCorner<3, 3>() =
      -body_rotation * skew(source_from_target_.getOrigin());
    twist_jacobian_.bottomRightCorner<3, 3>() = body_rotation;
  }

  nav_msgs::msg::Odometry convert(const nav_msgs::msg::Odometry & message) const
  {
    const auto & pose = message.pose.pose;
    const auto & linear = message.twist.twist.linear;
    const auto & angular = message.twist.twist.angular;
    for (double value : {pose.position.x, pose.position.y, pose.position.z,
        linear.x, linear.y, linear.z, angular.x, angular.y, angular.z})
    {
      if (!std::isfinite(value)) {throw std::invalid_argument("nonfinite odometry");}
    }
    for (const auto * covariance : {&message.pose.covariance, &message.twist.covariance}) {
      if (!std::all_of(covariance->begin(), covariance->end(),
          [](double value) {return std::isfinite(value);}))
      {
        throw std::invalid_argument("nonfinite covariance");
      }
    }
    const tf2::Transform input_from_source(normalized(pose.orientation),
      tf2::Vector3(pose.position.x, pose.position.y, pose.position.z));

    nav_msgs::msg::Odometry converted;
    converted.header = message.header;
    converted.header.frame_id = output_frame_;
    converted.child_frame_id = target_frame_;
    tf2::toMsg(output_from_input_ * input_from_source * source_from_target_, converted.pose.pose);

    const tf2::Vector3 source_linear(linear.x, linear.y, linear.z);
    const tf2::Vector3 source_angular(angular.x, angular.y, angular.z);
    const auto target_from_source = source_from_target_.getBasis().transpose();
    // 先补偿底盘原点的旋转杠杆臂，再转到底盘轴向；不转到 odom 系。
    converted.twist.twist.linear = tf2::toMsg(target_from_source *
      (source_linear + source_angular.cross(source_from_target_.getOrigin())));
    converted.twist.twist.angular = tf2::toMsg(target_from_source * source_angular);

    // 输入姿态误差采用父系左旋转增量，位置误差与姿态误差可有交叉相关。
    Matrix6 pose_jacobian = Matrix6::Zero();
    pose_jacobian.topLeftCorner<3, 3>() = rotation_;
    pose_jacobian.topRightCorner<3, 3>() = -rotation_ *
      skew(input_from_source.getBasis() * source_from_target_.getOrigin());
    pose_jacobian.bottomRightCorner<3, 3>() = rotation_;
    converted.pose.covariance = propagate(message.pose.covariance, pose_jacobian);
    converted.twist.covariance = propagate(message.twist.covariance, twist_jacobian_);
    return converted;
  }

private:
  static tf2::Transform transform(const geometry_msgs::msg::Transform & message)
  {
    const auto & p = message.translation;
    if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) {
      throw std::invalid_argument("nonfinite static transform");
    }
    return tf2::Transform(normalized(message.rotation), tf2::Vector3(p.x, p.y, p.z));
  }

  tf2::Transform output_from_input_, source_from_target_;
  Eigen::Matrix3d rotation_;
  Matrix6 twist_jacobian_ = Matrix6::Zero();
  std::string output_frame_, target_frame_;
};
}  // namespace odom_frame_adapter

#endif  // ODOM_FRAME_ADAPTER__ODOMETRY_CONVERSION_HPP_
