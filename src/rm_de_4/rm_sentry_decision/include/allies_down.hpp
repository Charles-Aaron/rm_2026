#ifndef RM_SENTRY_DECISION__ALLIES_DOWN_HPP_
#define RM_SENTRY_DECISION__ALLIES_DOWN_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"

namespace rm_sentry_decision {

// 条件节点：当本方 1/3/4 号机器人全部阵亡(HP==0)时返回 SUCCESS
class AlliesDown : public BT::ConditionNode {
public:
  AlliesDown(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<int>("max_data_age_ms", 1000, "最大允许状态数据年龄(ms)"),
      BT::InputPort<int>(
        "min_stage_remain_time", 0,
        "仅在剩余时间大于该值时允许触发；0表示不限制")
    };
  }

  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  DataManager *data_manager_{nullptr};
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__ALLIES_DOWN_HPP_ 
