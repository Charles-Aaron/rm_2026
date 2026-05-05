#include "topics_to_blackboard.hpp"

#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

TopicsToBlackboard::TopicsToBlackboard(
  const std::string &name,
  const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("裁判数据写入黑板：无法从行为树黑板获取 ROS 节点");
  }
  data_manager_ = &DataManager::getInstance(node_);
}

BT::NodeStatus TopicsToBlackboard::tick()
{
  auto bb = config().blackboard;
  const bool data_fresh = data_manager_->isDataFresh(1000);
  const bool primary_fresh = data_manager_->hasPrimaryStatus(500);
  const bool is_blue = data_manager_->team_color_.load();

  bb->set("data_fresh", data_fresh);
  bb->set("primary_status_fresh", primary_fresh);
  bb->set("game_progress", static_cast<int>(data_manager_->game_progress_.load()));
  bb->set("stage_remain_time", static_cast<int>(data_manager_->stage_remain_time_.load()));
  bb->set("current_hp", static_cast<int>(data_manager_->current_hp_.load()));
  bb->set("bullet_allowance_17mm", static_cast<int>(data_manager_->bullets_remaining_.load()));
  bb->set("team_color", is_blue);
  bb->set("is_blue", is_blue);

  bb->set("own_base_hp", static_cast<int>(data_manager_->getOwnBaseHp(is_blue)));
  bb->set("enemy_base_hp", static_cast<int>(data_manager_->getEnemyBaseHp(is_blue)));
  bb->set("own_outpost_hp", static_cast<int>(data_manager_->getOwnOutpostHp(is_blue)));
  bb->set("enemy_outpost_hp", static_cast<int>(data_manager_->getEnemyOutpostHp(is_blue)));
  bb->set("own_robot1_hp", static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 1)));
  bb->set("own_robot3_hp", static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 3)));
  bb->set("own_robot4_hp", static_cast<int>(data_manager_->getOwnRobotHp(is_blue, 4)));
  bb->set("enemy_robot1_hp", static_cast<int>(data_manager_->getEnemyRobotHp(is_blue, 1)));
  bb->set("enemy_robot3_hp", static_cast<int>(data_manager_->getEnemyRobotHp(is_blue, 3)));
  bb->set("enemy_robot4_hp", static_cast<int>(data_manager_->getEnemyRobotHp(is_blue, 4)));

  RCLCPP_DEBUG_THROTTLE(
    node_->get_logger(), *node_->get_clock(), 1000,
    "裁判数据写入黑板：数据新鲜=%d 主状态新鲜=%d 比赛阶段=%d HP=%d 弹量=%d",
    data_fresh, primary_fresh,
    static_cast<int>(data_manager_->game_progress_.load()),
    static_cast<int>(data_manager_->current_hp_.load()),
    static_cast<int>(data_manager_->bullets_remaining_.load()));

  return BT::NodeStatus::SUCCESS;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::TopicsToBlackboard>("TopicsToBlackboard");
}
