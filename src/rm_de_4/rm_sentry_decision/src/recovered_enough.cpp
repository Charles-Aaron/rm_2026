#include "recovered_enough.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"
#include "pose_behavior.hpp"

namespace rm_sentry_decision {
namespace {

std::string currentPoseText(const rclcpp::Node::SharedPtr &node)
{
  auto pose_manager = PoseManager::getInstance(node);
  if (!pose_manager) {
    return "未知姿态";
  }
  return pose_manager->poseToString(pose_manager->getCurrentPose());
}

}  // namespace

RecoveredEnough::RecoveredEnough(const std::string &name, const BT::NodeConfiguration &config)
: BT::ConditionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("补给完成判断：无法从行为树黑板获取 ROS 节点");
  }
  data_manager_ = &DataManager::getInstance(node_);
}

BT::NodeStatus RecoveredEnough::tick()
{
  int min_hp = 400;
  int min_ammo = 50;
  bool require_game_start = true;
  int max_data_age_ms = 1000;

  getInput("min_hp", min_hp);
  getInput("min_ammo", min_ammo);
  getInput("require_game_start", require_game_start);
  getInput("max_data_age_ms", max_data_age_ms);

  if (!data_manager_->isGameStatusFresh(static_cast<uint32_t>(max_data_age_ms))) {
    RCLCPP_WARN_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "补给完成判断：比赛状态数据过期，数据年龄=%ldms",
      static_cast<long>(data_manager_->lastGameStatusAgeMs()));
    return BT::NodeStatus::FAILURE;
  }

  if (!data_manager_->isSelfStatusFresh(static_cast<uint32_t>(max_data_age_ms))) {
    RCLCPP_WARN_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "补给完成判断：本车血量/弹量数据过期，数据年龄=%ldms",
      static_cast<long>(data_manager_->lastSelfStatusAgeMs()));
    return BT::NodeStatus::FAILURE;
  }

  if (require_game_start && data_manager_->game_progress_.load() != 4) {
    return BT::NodeStatus::FAILURE;
  }

  const int hp = static_cast<int>(data_manager_->current_hp_.load());
  const int ammo = static_cast<int>(data_manager_->bullets_remaining_.load());
  const std::string current_pose = currentPoseText(node_);
  if (hp >= min_hp && ammo >= min_ammo) {
    RCLCPP_INFO(
      node_->get_logger(),
      "补给完成判断：补给已完成，此时姿态=%s，当前HP=%d 完成阈值=%d 当前弹量=%d 完成阈值=%d",
      current_pose.c_str(), hp, min_hp, ammo, min_ammo);
    return BT::NodeStatus::SUCCESS;
  }

  RCLCPP_INFO_THROTTLE(
    node_->get_logger(), *node_->get_clock(), 1000,
    "补给完成判断：等待补给完成，此时姿态=%s，当前HP=%d 完成阈值=%d 当前弹量=%d 完成阈值=%d",
    current_pose.c_str(), hp, min_hp, ammo, min_ammo);
  return BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::RecoveredEnough>("RecoveredEnough");
}
