#pragma once

#include "behaviortree_cpp_v3/behavior_tree.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include <atomic>

namespace rm_sentry_decision {

class RushEnemyBuff : public BT::SyncActionNode {
public:
    RushEnemyBuff(const std::string &name, const BT::NodeConfiguration &config);

    static BT::PortsList providedPorts();

    BT::NodeStatus tick() override;

private:
    rclcpp::Node::SharedPtr node_;

    // Subscriptions
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr status_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr hp_sub_;

    // Cached data
    std::atomic<uint16_t> remain_time_{9999};
    std::atomic<uint16_t> robot1_hp_{0};
    std::atomic<uint16_t> robot3_hp_{0};
    std::atomic<uint16_t> robot4_hp_{0};
    std::atomic<bool> is_blue_team_{false};
};

} // namespace rm_sentry_decision 