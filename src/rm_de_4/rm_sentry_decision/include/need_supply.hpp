#ifndef RM_SENTRY_DECISION__NEED_SUPPLY_HPP_
#define RM_SENTRY_DECISION__NEED_SUPPLY_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"
#include <atomic>

namespace rm_sentry_decision {

class NeedSupply : public BT::ConditionNode {
public:
  NeedSupply(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<int>("enter_hp", 150, "进入补给的血量阈值"),
      BT::InputPort<int>("enter_ammo", 40, "进入补给的17mm弹量阈值"),
      BT::InputPort<int>("exit_hp", 400, "退出补给的血量阈值"),
      BT::InputPort<int>("exit_ammo", 50, "退出补给的17mm弹量阈值"),
      BT::InputPort<bool>("require_game_start", true, "是否要求game_progress==4"),
      BT::InputPort<int>("max_data_age_ms", 1000, "最大允许状态数据年龄(ms)")
    };
  }

  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  DataManager *data_manager_{nullptr};
  std::atomic<bool> in_supply_{false};
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__NEED_SUPPLY_HPP_ 
