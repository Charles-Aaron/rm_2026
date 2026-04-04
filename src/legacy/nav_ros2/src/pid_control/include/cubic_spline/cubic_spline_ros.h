#ifndef ROBOMASTER_CUBIC_SPLINE_ROS_H
#define ROBOMASTER_CUBIC_SPLINE_ROS_H

#include "cubic_spline.h"
#include "cpprobotics_types.h"
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>


struct TrajInfo {
  std::vector<float> smoothed_path_x,
      smoothed_path_y,
      smoothed_path_yaw,
      smoothed_path_curvature,
      smoothed_path_s;
};

TrajInfo GenTraj(const nav_msgs::msg::Path& path, nav_msgs::msg::Path& smoothed_path, const float interval = 0.1) {
  using namespace cpprobotics;
  TrajInfo traj_info;
  std::vector<float> path_x, path_y;

  for (size_t j = 0; j < path.poses.size(); ++j) {
    path_x.push_back(path.poses[j].pose.position.x);
    path_y.push_back(path.poses[j].pose.position.y);
  }

  // create a cubic spline interpolator
  Spline2D cubic_spline(path_x, path_y);
  // calculate the new smoothed trajectory
  geometry_msgs::msg::PoseStamped tmp_pose;
  tmp_pose.header.frame_id = path.header.frame_id;
  tmp_pose.header.stamp = rclcpp::Clock().now(); // Use ROS 2 clock
  for (float i = 0; i < cubic_spline.s.back(); i += interval) {
    std::array<float, 2> point_ = cubic_spline.calc_position(i);
    traj_info.smoothed_path_x.push_back(point_[0]);
    traj_info.smoothed_path_y.push_back(point_[1]);
    float yaw = cubic_spline.calc_yaw(i);
    traj_info.smoothed_path_yaw.push_back(yaw);
    traj_info.smoothed_path_curvature.push_back(cubic_spline.calc_curvature(i));
    traj_info.smoothed_path_s.push_back(i);

    tmp_pose.pose.position.x = point_[0];
    tmp_pose.pose.position.y = point_[1];
    tmp_pose.pose.position.z = 0.0;

    // Create quaternion from yaw using tf2
    tf2::Quaternion q;
    q.setRPY(0, 0, yaw);
    tmp_pose.pose.orientation = tf2::toMsg(q);

    smoothed_path.poses.push_back(tmp_pose);
  }

  return traj_info;
}

#endif // ROBOMASTER_CUBIC_SPLINE_ROS_H