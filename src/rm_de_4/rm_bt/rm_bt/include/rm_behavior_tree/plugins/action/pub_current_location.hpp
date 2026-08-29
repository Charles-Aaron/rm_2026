#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_CURRENT_LOCATION_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_CURRENT_LOCATION_HPP_

#include "behaviortree_cpp/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "behaviortree_ros2/ros_node_params.hpp"
#include <vector>
#include <cmath>

namespace rm_behavior_tree
{

class PubCurrentLocationAction : public BT::SyncActionNode
{
public:
  PubCurrentLocationAction(const std::string &name, const BT::NodeConfig &config);

  // 添加符合 BT::BehaviorTreeFactory 要求的构造函数
  PubCurrentLocationAction(const std::string &name, const BT::NodeConfig &config, const BT::RosNodeParams &params)
    : BT::SyncActionNode(name, config), node_(params.nh), tf_buffer_(std::make_shared<tf2_ros::Buffer>(node_->get_clock())), tf_listener_(*tf_buffer_) {}

  /**
   * @brief 行为树节点的 tick 方法
   * @return BT::NodeStatus
   */
  BT::NodeStatus tick() override;

  static BT::PortsList providedPorts()
  {
    return {
      BT::OutputPort<std::string>("current_location") // 仅输出位置信息
    };
  }

private:
  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  tf2_ros::TransformListener tf_listener_;
};

}  // namespace rm_behavior_tree

#endif  // RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_CURRENT_LOCATION_HPP_
