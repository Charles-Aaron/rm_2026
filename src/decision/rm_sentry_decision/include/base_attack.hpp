#ifndef RM_SENTRY_DECISION__BASE_ATTACK_HPP_
#define RM_SENTRY_DECISION__BASE_ATTACK_HPP_

#include "behaviortree_cpp_v3/condition_node.h"  // 使用v3专用头文件
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"

namespace rm_sentry_decision {

class BaseAttack : public BT::ConditionNode {  // 使用基础ConditionNode
public:
   BaseAttack(const std::string& name, 
             const BT::NodeConfiguration& config);  // 使用v3的配置类型

   static BT::PortsList providedPorts() {
     return {};
   }

   BT::NodeStatus tick() override;
  
private:
  std::atomic<uint16_t> red_outpost_hp_{0};
  std::atomic<uint16_t> blue_outpost_hp_{0};
  std::atomic<uint16_t> red_base_hp_{0};
  std::atomic<uint16_t> blue_base_hp_{0};
  std::atomic<bool> is_blue_team_{false};
  rclcpp::Node::SharedPtr node_;  // 需要手动管理节点
  rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr hp_sub_;
  rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr status_sub_;
};

} // namespace rm_sentry_decision

#endif