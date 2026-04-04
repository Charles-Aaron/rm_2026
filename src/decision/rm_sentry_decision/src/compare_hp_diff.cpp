#include "compare_hp_diff.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

CompareHpDiff::CompareHpDiff(const std::string &name, const BT::NodeConfiguration &config)
: BT::ConditionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("CompareHpDiff: Failed to get ROS node from blackboard");
    }

    // 初始化参数管理器
    param_manager_ = ParameterManager::getInstance(node_);

    self_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
        "/robot_status", 10,
        [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg){
            self_hp_.store(msg->current_hp);
            is_blue_team_.store(msg->team_color);
        });

    all_hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
        "/all_robot_hp", 10,
        [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg){
            red_3_hp_.store(msg->red_3_robot_hp);
            blue_3_hp_.store(msg->blue_3_robot_hp);
        });
}

BT::NodeStatus CompareHpDiff::tick()
{
    int min_diff=0, max_diff=100000;
    getInput("min_diff", min_diff);
    getInput("max_diff", max_diff);

    // 从参数管理器获取阈值参数
    int max_self_hp = param_manager_->getHpThreshold("max");
    int max_diff_threshold = param_manager_->getHpDiffThreshold("max");

    uint16_t selfHP = self_hp_.load();
    bool blue = is_blue_team_.load();
    uint16_t enemyHP = blue ? red_3_hp_.load() : blue_3_hp_.load();

    if(selfHP >= max_self_hp) {
        RCLCPP_DEBUG(node_->get_logger(), "CompareHpDiff: Self HP %d >= %d, returning FAILURE", 
                     selfHP, max_self_hp);
        return BT::NodeStatus::FAILURE; // 自身血量未低于最大阈值
    }

    int diff = static_cast<int>(enemyHP) - static_cast<int>(selfHP);

    // 使用参数化的最大差值阈值
    bool upper_ok = (max_diff_threshold < 0) ? true : (diff <= max_diff_threshold);
    bool cond = (diff >= min_diff) && upper_ok;

    RCLCPP_DEBUG(node_->get_logger(), "CompareHpDiff: Self=%d, Enemy=%d, Diff=%d, Min=%d, Max=%d, Result=%s", 
                 selfHP, enemyHP, diff, min_diff, max_diff_threshold, 
                 cond ? "SUCCESS" : "FAILURE");

    return cond ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory){
    factory.registerNodeType<rm_sentry_decision::CompareHpDiff>("CompareHpDiff");
} 