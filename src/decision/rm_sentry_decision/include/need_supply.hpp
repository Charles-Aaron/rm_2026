#ifndef RM_SENTRY_DECISION__NEED_SUPPLY_HPP_
#define RM_SENTRY_DECISION__NEED_SUPPLY_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include <atomic>
#include <chrono>
#include <memory>

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class NeedSupply : public BT::ConditionNode {
public:
  NeedSupply(const std::string &name, const BT::NodeConfiguration &config);

  static BT::PortsList providedPorts() { return {}; }

  BT::NodeStatus tick() override;

private:
  // 缓存最新数据
  std::atomic<int32_t> remaining_time_{9999};     // 毫秒或秒，取决于 topic
  std::atomic<int32_t> current_hp_{0};
  // 剩余弹丸量
  std::atomic<int32_t> remaining_ammo_{9999};
  
  // 补给冷却时间
  // std::chrono::steady_clock::time_point last_supply_time_{};

  rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_sub_;
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<ParameterManager> param_manager_;
  std::atomic<bool> suppressed_{false}; 
  std::atomic<uint16_t> outpost_hp_{0};
  std::atomic<bool> is_blue_{false};
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__NEED_SUPPLY_HPP_ 