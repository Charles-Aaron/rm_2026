#ifndef RM_SENTRY_DECISION__OUTPOST_ATTACK_HPP_
#define RM_SENTRY_DECISION__OUTPOST_ATTACK_HPP_

#include "behaviortree_cpp_v3/condition_node.h"  // 使用v3头文件
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

class OutpostAttack : public BT::ConditionNode {
public:
  OutpostAttack(const std::string& name, 
               const BT::NodeConfiguration& config);  // 使用v3的配置类型

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override;

private:
  // 分别缓存红蓝方前哨站血量，避免回调里依赖 team_color 决策
  std::atomic<int> red_outpost_hp_{0};
  std::atomic<int> blue_outpost_hp_{0};
  std::atomic<bool> is_blue_team_{false};
  
  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr outpost_status_sub_;
  rclcpp::Node::SharedPtr node_;  // 需要手动管理节点指针
  std::shared_ptr<rm_sentry_decision::ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__OUTPOST_ATTACK_HPP_