#ifndef RM_SENTRY_DECISION__NEED_SUPPLY_OPTIMIZED_HPP_
#define RM_SENTRY_DECISION__NEED_SUPPLY_OPTIMIZED_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <memory>

namespace rm_sentry_decision {

/**
 * @brief 优化版本的补给需求判断节点
 * 
 * 使用共享的DataManager而不是直接创建订阅，减少内存使用
 */
class NeedSupplyOptimized : public BT::ConditionNode {
public:
    NeedSupplyOptimized(const std::string &name, const BT::NodeConfiguration &config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<int>("critical_time", 150, "关键剩余时间阈值 (s)"),
            BT::InputPort<int>("low_hp_threshold", 200, "低血量阈值"),
            BT::InputPort<int>("low_ammo_threshold", 200, "低弹量阈值")
        };
    }

    BT::NodeStatus tick() override;

private:
    rclcpp::Node::SharedPtr node_;
    DataManager* data_manager_{nullptr};
    ParameterManager* param_manager_{nullptr};
    std::atomic<bool> suppressed_{false}; 
};

} // namespace rm_sentry_decision

#endif 