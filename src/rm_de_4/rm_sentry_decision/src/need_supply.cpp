#include "need_supply.hpp"

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

NeedSupply::NeedSupply(const std::string &name, const BT::NodeConfiguration &config)
: BT::ConditionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("补给判断：无法从行为树黑板获取 ROS 节点");
  }
  data_manager_ = &DataManager::getInstance(node_);
}

BT::NodeStatus NeedSupply::tick()
{
  int enter_hp = 150;
  int enter_ammo = 40;
  int exit_hp = 400;
  int exit_ammo = 50;
  bool require_game_start = true;
  int max_data_age_ms = 1000;

  getInput("enter_hp", enter_hp);
  getInput("enter_ammo", enter_ammo);
  getInput("exit_hp", exit_hp);
  getInput("exit_ammo", exit_ammo);
  getInput("require_game_start", require_game_start);
  getInput("max_data_age_ms", max_data_age_ms);

  if (!data_manager_->isGameStatusFresh(static_cast<uint32_t>(max_data_age_ms))) {
    RCLCPP_WARN_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "补给判断：比赛状态数据过期，数据年龄=%ldms",
      static_cast<long>(data_manager_->lastGameStatusAgeMs()));
    return BT::NodeStatus::FAILURE;
  }

  if (!data_manager_->isSelfStatusFresh(static_cast<uint32_t>(max_data_age_ms))) {
    RCLCPP_WARN_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "补给判断：本车血量/弹量数据过期，数据年龄=%ldms",
      static_cast<long>(data_manager_->lastSelfStatusAgeMs()));
    return BT::NodeStatus::FAILURE;
  }

  const int game_progress = static_cast<int>(data_manager_->game_progress_.load());
  if (require_game_start && game_progress != 4) {
    in_supply_.store(false);
    return BT::NodeStatus::FAILURE;
  }

  const int hp = static_cast<int>(data_manager_->current_hp_.load());
  const int ammo = static_cast<int>(data_manager_->bullets_remaining_.load());
  const std::string current_pose = currentPoseText(node_);

  if (!in_supply_.load()) {
    if (hp <= enter_hp || ammo <= enter_ammo) {
      in_supply_.store(true);
      RCLCPP_INFO(
        node_->get_logger(),
        "补给判断：进入补给模式，此时姿态=%s，当前HP=%d 触发阈值=%d 当前弹量=%d 触发阈值=%d",
        current_pose.c_str(), hp, enter_hp, ammo, enter_ammo);
      return BT::NodeStatus::SUCCESS;
    }
    RCLCPP_INFO_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "补给判断：当前无需补给，此时姿态=%s，当前HP=%d 触发阈值=%d 当前弹量=%d 触发阈值=%d",
      current_pose.c_str(), hp, enter_hp, ammo, enter_ammo);
    return BT::NodeStatus::FAILURE;
  }

  if (hp >= exit_hp && ammo >= exit_ammo) {
    in_supply_.store(false);
    RCLCPP_INFO(
      node_->get_logger(),
      "补给判断：退出补给模式，此时姿态=%s，当前HP=%d 恢复阈值=%d 当前弹量=%d 恢复阈值=%d",
      current_pose.c_str(), hp, exit_hp, ammo, exit_ammo);
    return BT::NodeStatus::FAILURE;
  }

  RCLCPP_INFO_THROTTLE(
    node_->get_logger(), *node_->get_clock(), 1000,
    "补给判断：保持补给模式，此时姿态=%s，当前HP=%d 恢复阈值=%d 当前弹量=%d 恢复阈值=%d",
    current_pose.c_str(), hp, exit_hp, ammo, exit_ammo);
  return BT::NodeStatus::SUCCESS;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::NeedSupply>("NeedSupply");
}
