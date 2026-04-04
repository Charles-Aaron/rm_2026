#include "need_supply.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <chrono>

namespace rm_sentry_decision {

NeedSupply::NeedSupply(const std::string &name, const BT::NodeConfiguration &config)
    : BT::ConditionNode(name, config) {
  // Get node from blackboard
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("Failed to get ROS node from blackboard");
  }

  // 初始化参数管理器
  param_manager_ = ParameterManager::getInstance(node_);

  // Subscribe GameStatus for remaining match time (assume field remaining_time)
  game_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
      "/game_detection_status", 10,
      [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        // 假设 GameStatus 有字段 remaining_time (单位: 秒)
        remaining_time_.store(msg->stage_remain_time);
        current_hp_.store(msg->current_hp);
        remaining_ammo_.store(msg->bullet_allowance_17mm);
        is_blue_.store(msg->team_color);
         if (is_blue_.load()) {
                outpost_hp_.store(msg->blue_outpost_hp);
            } else {
                outpost_hp_.store(msg->red_outpost_hp);
            }
      });
}

BT::NodeStatus NeedSupply::tick() {
  const int trigger_hp   = 150;  // 触发补给
  const int trigger_ammo = 40;
  const int recover_hp   = 380;  // 退出补给
  const int recover_ammo = 50;

    int hp   = current_hp_.load();
    int ammo = remaining_ammo_.load();

    // 如果已触发且尚未恢复，直接保持 SUCCESS
    
  // 从参数管理器获取阈值参数
  int endgame_time = param_manager_->getTimeThreshold("endgame");
  int low_hp = param_manager_->getHpThreshold("medium");
  int low_ammo = param_manager_->getAmmoThreshold("medium");
  int critical_ammo = param_manager_->getAmmoThreshold("critical");
  // int cooldown_time = param_manager_->getTimeThreshold("supply_cooldown");
  // int log_interval = param_manager_->getLoggingParam("info_interval");
  // int warn_interval = param_manager_->getLoggingParam("warn_interval");

  // 修改条件逻辑：需要补给的情况
  // 1. 剩余时间 <= endgame_time 且 (hp<low_hp 或 ammo<low_ammo)
  // 2. 或者 弹量严重不足 (< critical_ammo)
  bool time_critical = (remaining_time_.load() <= endgame_time);
  bool low_resources = (current_hp_.load() < 150) || (remaining_ammo_.load() < 40);
  // bool ammo_critical = (remaining_ammo_.load() < critical_ammo);
  
  // auto now = std::chrono::steady_clock::now();
  
  // if ((time_critical && low_resources) || ammo_critical) {
  //   // 检查冷却时间：如果最近cooldown_time秒内已经触发过补给，则不再触发
  //   if (last_supply_time_.time_since_epoch().count() == 0 || 
  //       std::chrono::duration_cast<std::chrono::seconds>(now - last_supply_time_).count() >= cooldown_time) {
      
  //     last_supply_time_ = now; // 更新最后补给时间
  //     RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), log_interval,
  //                          "Need supply! time:%d hp:%d ammo:%d (thresholds: time<=%d, hp<%d, ammo<%d)", 
  //                          remaining_time_.load(), current_hp_.load(), remaining_ammo_.load(),
  //                          endgame_time, low_hp, low_ammo);
  //     return BT::NodeStatus::SUCCESS;
  //   } else {
  //     // 在冷却期内 - 但仍然返回SUCCESS保持补给状态
  //     auto cooldown_remaining = cooldown_time - std::chrono::duration_cast<std::chrono::seconds>(now - last_supply_time_).count();
  //     RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), warn_interval,
  //                          "Supply cooldown: %ld seconds remaining (maintaining supply state)", cooldown_remaining);
  //     return BT::NodeStatus::SUCCESS;  // 关键修改：保持补给状态
  //   }
  // }time_critical && 
  if (!suppressed_.load()) {
    // 还未进入补给状态，判断是否触发
    bool need_supply = (hp < trigger_hp || ammo < trigger_ammo);
    if (need_supply) {
      suppressed_.store(true);
      RCLCPP_INFO(node_->get_logger(), "Enter supply mode: HP=%d, Ammo=%d", hp, ammo);
      return BT::NodeStatus::SUCCESS;
    } else {
      return BT::NodeStatus::FAILURE;
    }
  } else {
    // 已进入补给状态，判断是否退出
    bool fully_recovered = (hp >= recover_hp && ammo >= recover_ammo);
    if (fully_recovered) {
      suppressed_.store(false);
      RCLCPP_INFO(node_->get_logger(), "Exit supply mode: HP=%d, Ammo=%d", hp, ammo);
      return BT::NodeStatus::FAILURE;
    } else {
      RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                           "Still supplying... HP=%d, Ammo=%d", hp, ammo);
      return BT::NodeStatus::SUCCESS;  // 保持补给状态
    }
  }
}

} // namespace rm_sentry_decision

// 注册节点
BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::NeedSupply>("NeedSupply");
} 