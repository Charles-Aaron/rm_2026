#include "allies_down.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

AlliesDown::AlliesDown(const std::string &name, const BT::NodeConfiguration &config)
: BT::ConditionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("AlliesDown: failed to get ROS node from blackboard");
  }
  data_manager_ = &DataManager::getInstance(node_);
}

BT::NodeStatus AlliesDown::tick()
{
  int max_data_age_ms = 1000;
  getInput("max_data_age_ms", max_data_age_ms);
  if (!data_manager_->isDataFresh(static_cast<uint32_t>(max_data_age_ms))) {
    return BT::NodeStatus::FAILURE;
  }

  int min_stage_remain_time = 0;
  getInput("min_stage_remain_time", min_stage_remain_time);
  if (min_stage_remain_time > 0 &&
      static_cast<int>(data_manager_->stage_remain_time_.load()) <= min_stage_remain_time) {
    RCLCPP_DEBUG_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "AlliesDown: stage_remain_time=%u <= %d, skip buff guard",
      static_cast<unsigned int>(data_manager_->stage_remain_time_.load()),
      min_stage_remain_time);
    return BT::NodeStatus::FAILURE;
  }

  const bool is_blue = data_manager_->team_color_.load();
  const int robot1_hp = static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 1));
  const int robot3_hp = static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 3));
  const int robot4_hp = static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 4));
  const bool all_down = (robot1_hp == 0) && (robot3_hp == 0) && (robot4_hp == 0);

  RCLCPP_DEBUG_THROTTLE(
    node_->get_logger(), *node_->get_clock(), 1000,
    "AlliesDown: team=%s r1=%d r3=%d r4=%d all_down=%d",
    is_blue ? "Blue" : "Red", robot1_hp, robot3_hp, robot4_hp, all_down);

  return all_down ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::AlliesDown>("AlliesDown");
}
