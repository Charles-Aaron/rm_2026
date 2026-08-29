#ifndef RM_SENTRY_DECISION__GAME_STATUS_SUB_HPP_
#define RM_SENTRY_DECISION__GAME_STATUS_SUB_HPP_
#include "behaviortree_cpp_v3/condition_node.h"  // v3专用头文件路径
#include "rclcpp/rclcpp.hpp"
#include "rm_sentry_decision/data_manager.hpp"

namespace rm_sentry_decision {
class GameStatusSub : public BT::ConditionNode {
public:
    GameStatusSub(const std::string& name, const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<int>("start_status"),
            BT::InputPort<int>("max_data_age_ms", 1000, "最大允许状态数据年龄(ms)")
        };
    }
    
    BT::NodeStatus tick() override;

private:
    rclcpp::Node::SharedPtr node_;
    DataManager *data_manager_{nullptr};
};
} // namespace
#endif
