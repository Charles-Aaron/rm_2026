#ifndef RM_SENTRY_DECISION__GAME_STATUS_SUB_HPP_
#define RM_SENTRY_DECISION___GAME_STATUS_SUB_HPP_
#include "behaviortree_cpp_v3/condition_node.h"  // v3专用头文件路径
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"

namespace rm_sentry_decision {
class GameStatusSub : public BT::ConditionNode {
public:
    GameStatusSub(const std::string& name, const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return { BT::InputPort<int>("start_status") };
    }
    
    BT::NodeStatus tick() override;

private:
    std::atomic<int> current_status_{0};
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr sub_;
};
} // namespace
#endif