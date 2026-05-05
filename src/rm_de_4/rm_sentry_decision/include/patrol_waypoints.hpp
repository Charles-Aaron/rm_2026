#ifndef RM_SENTRY_DECISION__PATROL_WAYPOINTS_HPP_
#define RM_SENTRY_DECISION__PATROL_WAYPOINTS_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace rm_sentry_decision {

class LoadWaypoints : public BT::SyncActionNode {
public:
  LoadWaypoints(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

class GetCurrentWaypoint : public BT::SyncActionNode {
public:
  GetCurrentWaypoint(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

class WaitUntilReached : public BT::StatefulActionNode {
public:
  WaitUntilReached(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  geometry_msgs::msg::PoseStamped goal_;
  double tolerance_{0.5};
  std::string global_frame_{"map"};
  std::string robot_frame_{"base_link"};
};

class WaitDuration : public BT::StatefulActionNode {
public:
  WaitDuration(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  double duration_sec_{0.0};
  std::chrono::steady_clock::time_point start_time_;
};

class NextWaypoint : public BT::SyncActionNode {
public:
  NextWaypoint(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__PATROL_WAYPOINTS_HPP_
