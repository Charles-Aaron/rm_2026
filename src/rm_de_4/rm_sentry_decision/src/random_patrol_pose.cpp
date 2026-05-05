#include "random_patrol_pose.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"
#include <chrono>

namespace rm_sentry_decision {

RandomPatrolPose::RandomPatrolPose(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config)
{
    // seed RNG with steady_clock time
     node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) throw std::runtime_error("RandomPatrolPose: no ROS node");
    
    // 暂时注释掉参数管理器初始化，使用硬编码值测试
    // param_manager_ = ParameterManager::getInstance(node_);
    RCLCPP_INFO(node_->get_logger(), "RandomPatrolPose initialized with hardcoded values");
    
    sub_base_= node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
    "/game_detection_status", 10,
    [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
        // 根据队伍颜色缓存己方基地血量
        own_base_hp_.store(msg->team_color ? msg->blue_base_hp
                                           : msg->red_base_hp);
    });
}

BT::PortsList RandomPatrolPose::providedPorts() {
    return {
        BT::InputPort<std::string>("pose1"),
        BT::InputPort<std::string>("pose2"),
        BT::InputPort<std::string>("pose3"),
        BT::InputPort<int>("interval", 8, "切换间隔 (s)"),
        BT::OutputPort<std::string>("goal_pose")
    };
}

BT::NodeStatus RandomPatrolPose::tick() {
    // 暂时使用硬编码值，因为参数管理器被注释掉了
    int base_low = 1500;  // 硬编码基地低血量阈值
    
    if (own_base_hp_.load() < base_low) {
        RCLCPP_DEBUG(node_->get_logger(), "RandomPatrolPose: Base HP %d < %d, returning FAILURE", 
                     own_base_hp_.load(), base_low);
        return BT::NodeStatus::FAILURE;
    }

    // load interval only first time
    if (interval_sec_ == 8) {
        int iv;
        if (getInput("interval", iv)) {
            interval_sec_ = iv;
        }
    }

    std::vector<std::string> poses;
    std::string p1, p2, p3;
    if (getInput("pose1", p1) && !p1.empty()) poses.push_back(p1);
    if (getInput("pose2", p2) && !p2.empty()) poses.push_back(p2);
    if (getInput("pose3", p3) && !p3.empty()) poses.push_back(p3);

    if (poses.empty()) {
        RCLCPP_WARN(node_->get_logger(), "RandomPatrolPose: No poses provided, returning FAILURE");
        return BT::NodeStatus::FAILURE;
    }

    auto now = std::chrono::steady_clock::now();
    bool need_switch = cached_pose_.empty() ||
        std::chrono::duration_cast<std::chrono::seconds>(now - last_switch_).count() >= interval_sec_;

    if (need_switch) {
        std::uniform_int_distribution<size_t> dist(0, poses.size()-1);
        cached_pose_ = poses[dist(rng_)];
        last_switch_ = now;
        // 返回 SUCCESS 让 ReactiveSequence 继续执行 SendGoalAction
        setOutput("goal_pose", cached_pose_);
        return BT::NodeStatus::SUCCESS;  // 修复：返回SUCCESS而不是FAILURE
    }

    setOutput("goal_pose", cached_pose_);
    return BT::NodeStatus::SUCCESS;  // 改为 SUCCESS，只有在没有有效pose时才返回FAILURE
}

} // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::RandomPatrolPose>("RandomPatrolPose");
} 
