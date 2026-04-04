#include "is_outside_pose_radius.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"
#include "tf2/time.h"
#include "tf2/exceptions.h"

#include <cmath>
#include <sstream>
#include <vector>

namespace rm_sentry_decision {

IsOutsidePoseRadius::IsOutsidePoseRadius(const std::string &name, const BT::NodeConfiguration &config)
: BT::ConditionNode(name, config) {
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("IsOutsidePoseRadius: failed to get ROS node from blackboard");
  }

  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(node_->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_, node_, false);
}

bool IsOutsidePoseRadius::parseTargetPoseXY(const std::string &pose_str, double &x, double &y) const {
  std::stringstream ss(pose_str);
  std::string token;
  std::vector<std::string> tokens;
  while (std::getline(ss, token, ';')) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }

  if (tokens.size() != 7) {
    return false;
  }

  try {
    x = std::stod(tokens[0]);
    y = std::stod(tokens[1]);
  } catch (const std::exception &) {
    return false;
  }
  return true;
}

BT::NodeStatus IsOutsidePoseRadius::tick() {
  std::string target_pose;
  if (!getInput("target_pose", target_pose) || target_pose.empty()) {
    RCLCPP_ERROR(node_->get_logger(), "IsOutsidePoseRadius: missing required input [target_pose]");
    return BT::NodeStatus::FAILURE;
  }

  double radius = 0.9;
  getInput("radius", radius);
  if (radius < 0.0) {
    RCLCPP_WARN(node_->get_logger(), "IsOutsidePoseRadius: invalid radius %.3f, clamp to 0.0", radius);
    radius = 0.0;
  }

  std::string global_frame = "map";
  std::string robot_frame = "gimbal_yaw_fake";
  getInput("global_frame", global_frame);
  getInput("robot_frame", robot_frame);

  double target_x = 0.0;
  double target_y = 0.0;
  if (!parseTargetPoseXY(target_pose, target_x, target_y)) {
    RCLCPP_ERROR(node_->get_logger(),
                 "IsOutsidePoseRadius: target_pose format invalid, expected 7 fields");
    return BT::NodeStatus::FAILURE;
  }

  geometry_msgs::msg::TransformStamped tf_msg;
  try {
    tf_msg = tf_buffer_->lookupTransform(
      global_frame, robot_frame, tf2::TimePointZero, tf2::durationFromSec(0.05));
  } catch (const tf2::TransformException &ex) {
    RCLCPP_WARN(node_->get_logger(),
                "IsOutsidePoseRadius: lookup %s->%s failed: %s",
                global_frame.c_str(), robot_frame.c_str(), ex.what());
    return BT::NodeStatus::FAILURE;
  }

  const double robot_x = tf_msg.transform.translation.x;
  const double robot_y = tf_msg.transform.translation.y;
  const double dx = robot_x - target_x;
  const double dy = robot_y - target_y;
  const double dist = std::sqrt(dx * dx + dy * dy);

  if (dist > radius) {
    RCLCPP_INFO(node_->get_logger(),
                "IsOutsidePoseRadius: outside target radius (dist=%.2f > %.2f), need re-enter",
                dist, radius);
    return BT::NodeStatus::SUCCESS;
  }

  RCLCPP_DEBUG(node_->get_logger(),
               "IsOutsidePoseRadius: within target radius (dist=%.2f <= %.2f)",
               dist, radius);
  return BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::IsOutsidePoseRadius>("IsOutsidePoseRadius");
}
