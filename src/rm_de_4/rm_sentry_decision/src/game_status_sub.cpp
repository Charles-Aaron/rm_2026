#include "game_status_sub.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

GameStatusSub::GameStatusSub(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    data_manager_ = &DataManager::getInstance(node_);
}

BT::NodeStatus GameStatusSub::tick() {
    int required_status = 0;
    getInput("start_status", required_status);
    int max_data_age_ms = 1000;
    getInput("max_data_age_ms", max_data_age_ms);
    
    if (!data_manager_->isGameStatusFresh(static_cast<uint32_t>(max_data_age_ms))) {
        RCLCPP_WARN_THROTTLE(
            node_->get_logger(), *node_->get_clock(), 1000,
            "GameStatusSub: no fresh GameStatus, age=%ldms",
            static_cast<long>(data_manager_->lastGameStatusAgeMs()));
        return BT::NodeStatus::FAILURE;
    }

    int current = static_cast<int>(data_manager_->game_progress_.load());
    bool result = (current == required_status);

    RCLCPP_DEBUG_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
        "GameStatusSub check: current=%d, required=%d, result=%s",
        current, required_status, result ? "SUCCESS" : "FAILURE");
    
    return result ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::GameStatusSub>("GameStatusSub");
}
