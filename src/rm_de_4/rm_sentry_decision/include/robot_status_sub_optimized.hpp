#ifndef RM_SENTRY_DECISION__ROBOT_STATUS_SUB_OPTIMIZED_HPP_
#define RM_SENTRY_DECISION__ROBOT_STATUS_SUB_OPTIMIZED_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <memory>

namespace rm_sentry_decision {

/**
 * @brief 优化版本的机器人状态订阅节点
 * 
 * 使用共享的DataManager而不是直接创建订阅，减少内存使用
 */
class RobotStatusSubOptimized : public BT::ConditionNode {
public:
    RobotStatusSubOptimized(const std::string& name, 
                           const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<int>("min_hp"),
            BT::InputPort<int>("min_bullets"),
            BT::OutputPort<bool>("is_status_ok")
        };
    }

    BT::NodeStatus tick() override;

private:
    rclcpp::Node::SharedPtr node_;
    DataManager* data_manager_{nullptr};
    std::shared_ptr<ParameterManager> param_manager_;
};

} // namespace rm_sentry_decision

#endif 