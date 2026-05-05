#ifndef RM_SENTRY_DECISION__ROBOT_STATUS_SUB_HPP_
#define RM_SENTRY_DECISION__ROBOT_STATUS_SUB_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <atomic>
#include <memory>

namespace rm_sentry_decision {

class RobotStatusSub : public BT::ConditionNode {
public:
    RobotStatusSub(const std::string& name, 
                 const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::OutputPort<bool>("is_status_ok")
        };
    }

    BT::NodeStatus tick() override;

private:
    std::atomic<uint16_t> current_hp_{0};
    // 剩余弹丸量（来自 BulletAllowance 消息，17mm 枪口）
    std::atomic<uint16_t> bullets_remaining_{0};
    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr status_sub_;
    // 订阅允许发弹量 (0x0208) 消息
    std::shared_ptr<rm_sentry_decision::ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision

#endif