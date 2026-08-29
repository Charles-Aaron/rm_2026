#ifndef RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_
#define RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"

namespace rm_sentry_decision {

class RecoveredEnough : public BT::ConditionNode {
public:
  RecoveredEnough(const std::string &name, const BT::NodeConfiguration &config);
  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<int>("min_hp", 400, "补给完成血量阈值"),
      BT::InputPort<int>("min_ammo", 50, "补给完成17mm弹量阈值"),
      BT::InputPort<bool>("require_game_start", true, "是否要求game_progress==4"),
      BT::InputPort<int>("max_data_age_ms", 1000, "最大允许状态数据年龄(ms)")
    };
  }
  BT::NodeStatus tick() override;
private:
  rclcpp::Node::SharedPtr node_;
  DataManager *data_manager_{nullptr};
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__RECOVERED_ENOUGH_HPP_ 
