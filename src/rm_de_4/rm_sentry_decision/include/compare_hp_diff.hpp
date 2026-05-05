#ifndef RM_SENTRY_DECISION__COMPARE_HP_DIFF_HPP_
#define RM_SENTRY_DECISION__COMPARE_HP_DIFF_HPP_

#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include <atomic>
#include <memory>

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class CompareHpDiff : public BT::ConditionNode {
public:
    CompareHpDiff(const std::string &name, const BT::NodeConfiguration &config);

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<int>("min_diff", 0, "差值下限，包含"),
            BT::InputPort<int>("max_diff", -1, "差值上限，包含 (若给-1则表示无上限)")
        };
    }

    BT::NodeStatus tick() override;

private:
    std::atomic<uint16_t> self_hp_{0};
    std::atomic<uint16_t> red_3_hp_{0};
    std::atomic<uint16_t> blue_3_hp_{0};
    std::atomic<bool> is_blue_team_{false};

    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr self_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr all_hp_sub_;
    std::shared_ptr<ParameterManager> param_manager_;
};

}

#endif 