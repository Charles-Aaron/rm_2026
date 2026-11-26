#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <pcl/common/transforms.h>
#include <pcl_conversions/pcl_conversions.h>
#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <backend/keyframe_map.hpp>

// Display adapter only: all geometry is already expressed in the map frame.
// No TF, feedback into search, or extrapolation of accepted poses.
class ProbeVisualization {
public:
  ProbeVisualization(rclcpp::Node &node, const backend::KeyframeMap &prior,
                     const std::string &frame, double voxel) : frame_(frame) {
    const auto qos = rclcpp::QoS(1).reliable().transient_local();
    map_ = node.create_publisher<sensor_msgs::msg::PointCloud2>("~/prior_cloud", qos);
    aligned_ = node.create_publisher<sensor_msgs::msg::PointCloud2>("~/aligned_cloud", qos);
    markers_ = node.create_publisher<visualization_msgs::msg::MarkerArray>("~/visualization", qos);
    // Reuse upstream bundle projection, including optimized keyframe poses.
    const auto cloud = prior.buildOptimizedMap(voxel);
    sensor_msgs::msg::PointCloud2 message;
    pcl::toROSMsg(*cloud, message);
    message.header.frame_id = frame_;
    map_->publish(message);  // Static map, stamp zero; retained for late RViz subscribers.
    reset(message.header);
  }

  void reset(const std_msgs::msg::Header &header) {
    history_.clear();
    show(header, nullptr, nullptr, "WAITING: ZLIO frame", false);
  }

  void show(const std_msgs::msg::Header &source_header, const backend::Cloud *local,
            const Eigen::Matrix4d *pose, const std::string &status, bool success) {
    auto header = source_header;
    header.frame_id = frame_;
    backend::Cloud cloud;
    if (success && local && pose) pcl::transformPointCloud(*local, cloud, *pose);
    sensor_msgs::msg::PointCloud2 message;
    pcl::toROSMsg(cloud, message);
    message.header = header;
    aligned_->publish(message);  // Empty cloud clears a previous successful attempt.

    using Marker = visualization_msgs::msg::Marker;
    Marker arrow;
    arrow.header = header;
    arrow.ns = "relocalization";
    arrow.id = 0;
    arrow.type = Marker::ARROW;
    arrow.action = success ? Marker::ADD : Marker::DELETE;
    arrow.pose.orientation.w = 1;
    arrow.scale.x = 1.0; arrow.scale.y = .12; arrow.scale.z = .12;
    arrow.color.r = 1; arrow.color.g = .7; arrow.color.a = 1;
    if (success && pose) {
      const Eigen::Quaterniond q(pose->block<3, 3>(0, 0));
      arrow.pose.position.x = (*pose)(0, 3);
      arrow.pose.position.y = (*pose)(1, 3);
      arrow.pose.position.z = (*pose)(2, 3);
      arrow.pose.orientation.x = q.x(); arrow.pose.orientation.y = q.y();
      arrow.pose.orientation.z = q.z(); arrow.pose.orientation.w = q.w();
      history_.push_back(arrow.pose.position);
      if (history_.size() > 1000) history_.erase(history_.begin());
    }
    Marker text = arrow;
    text.id = 1;
    text.type = Marker::TEXT_VIEW_FACING;
    text.action = Marker::ADD;
    text.pose = geometry_msgs::msg::Pose();
    text.pose.orientation.w = 1;
    text.pose.position.z = 2;
    text.scale.z = .5;
    text.color.r = success ? .2 : 1;
    text.color.g = success ? 1 : .3;
    text.text = status;

    Marker history = text;
    history.id = 2;
    history.type = Marker::SPHERE_LIST;
    history.action = history_.empty() ? Marker::DELETE : Marker::ADD;
    history.pose.position.z = 0;
    history.scale.x = history.scale.y = history.scale.z = .15;
    history.color.r = .2; history.color.g = 1; history.color.b = .4;
    history.points = history_;
    visualization_msgs::msg::MarkerArray markers;
    markers.markers = {arrow, text, history};
    markers_->publish(markers);
  }

private:
  std::string frame_;
  std::vector<geometry_msgs::msg::Point> history_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr map_, aligned_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr markers_;
};
