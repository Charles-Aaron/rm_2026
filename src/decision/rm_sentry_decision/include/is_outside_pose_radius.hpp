#ifndef RM_SENTRY_DECISION__IS_OUTSIDE_POSE_RADIUS_HPP_
#define RM_SENTRY_DECISION__IS_OUTSIDE_POSE_RADIUS_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

#include <memory>
#include <string>

namespace rm_sentry_decision {

class IsOutsidePoseRadius : public BT::ConditionNode {
public:
  IsOutsidePoseRadius(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<std::string>("target_pose", "x; y; z; qx; qy; qz; qw"),
      BT::InputPort<double>("radius", 0.9, "distance threshold in meters"),
      BT::InputPort<std::string>("global_frame", "map", "global frame"),
      BT::InputPort<std::string>("robot_frame", "gimbal_yaw_fake", "robot base frame")
    };
  }

  BT::NodeStatus tick() override;

private:
  bool parseTargetPoseXY(const std::string &pose_str, double &x, double &y) const;

  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
};

}  // namespace rm_sentry_decision

#endif  // RM_SENTRY_DECISION__IS_OUTSIDE_POSE_RADIUS_HPP_
