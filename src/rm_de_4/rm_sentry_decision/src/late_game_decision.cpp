#include "late_game_decision.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

LateGameDecision::LateGameDecision(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("LateGameDecision: failed to get ROS node from blackboard");
    }
    data_manager_ = &DataManager::getInstance(node_);
}

BT::PortsList LateGameDecision::providedPorts()
{
    return {
        BT::InputPort<int>("critical_time", 150, "关键剩余时间阈值 (s)"),
        BT::InputPort<int>("low_hp_threshold", 200, "低血量阈值"),
        BT::InputPort<int>("low_ammo_threshold", 200, "低弹量阈值"),
        BT::InputPort<int>("recovery_hp_threshold", 300, "补给后血量阈值"),
        BT::InputPort<int>("recovery_ammo_threshold", 300, "补给后弹量阈值"),
        BT::InputPort<int>("hp_diff_threshold", 500, "基地血量差阈值"),
        BT::InputPort<std::string>("supply_pose", "0;0;0;0;0;0;1", "前往补给点"),
        BT::InputPort<std::string>("attack_pose", "0;0;0;0;0;0;1", "冲家点"),
        BT::InputPort<std::string>("defend_pose", "0;0;0;0;0;0;1", "保家点"),
        // 堡垒增益点和蹲守时长
        BT::InputPort<std::string>("buff_pose", "0;0;0;0;0;0;1", "堡垒增益点"),
        BT::InputPort<int>("guard_duration", 60, "蹲守时长 (s)"),
        // 输出
        BT::OutputPort<std::string>("goal_pose", "决定后的目标点")
    };
}

BT::NodeStatus LateGameDecision::tick()
{
    int critical_time, low_hp_thr, low_ammo_thr, recovery_hp_thr, recovery_ammo_thr, hp_diff_thr;
    int guard_duration;
    std::string supply_pose, attack_pose, defend_pose, buff_pose;

    getInput("critical_time", critical_time);
    getInput("low_hp_threshold", low_hp_thr);
    getInput("low_ammo_threshold", low_ammo_thr);
    getInput("recovery_hp_threshold", recovery_hp_thr);
    getInput("recovery_ammo_threshold", recovery_ammo_thr);
    getInput("hp_diff_threshold", hp_diff_thr);
    getInput("supply_pose", supply_pose);
    getInput("attack_pose", attack_pose);
    getInput("defend_pose", defend_pose);
    getInput("buff_pose", buff_pose);
    getInput("guard_duration", guard_duration);

    if (!data_manager_->isDataFresh(1000)) {
        return BT::NodeStatus::FAILURE;
    }

    const bool is_blue = data_manager_->team_color_.load();
    const uint16_t remain = data_manager_->stage_remain_time_.load();
    const uint16_t hp = data_manager_->current_hp_.load();
    const uint16_t ammo = data_manager_->bullets_remaining_.load();

    auto now = std::chrono::steady_clock::now();

    // Step 0: 若处于蹲守状态，则持续 SUCCESS 直到时间结束
    if (guard_active_.load()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - guard_start_time_).count();
        if (elapsed < guard_duration) {
            setOutput("goal_pose", buff_pose);
            return BT::NodeStatus::SUCCESS;
        } else {
            guard_active_.store(false);
        }
    }

    // Step 0.5: 判断是否需要前往堡垒增益点
    bool need_buff_guard =
        (data_manager_->getOwnRobotHp(is_blue, 1) == 0 &&
         data_manager_->getOwnRobotHp(is_blue, 3) == 0 &&
         data_manager_->getOwnRobotHp(is_blue, 4) == 0);
    if (need_buff_guard) {
        guard_active_.store(true);
        guard_start_time_ = now;
        setOutput("goal_pose", buff_pose);
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                             "[LateGameDecision] 我方1/3/4号机器人全部阵亡，前往堡垒增益点并蹲守(%s)", buff_pose.c_str());
        return BT::NodeStatus::SUCCESS;
    }

    // Step 1: 时间检查
    if (remain > critical_time) {
        return BT::NodeStatus::FAILURE;
    }

    // Step 2: 血量/弹药不足 -> 补给
    if (hp < low_hp_thr || ammo < low_ammo_thr) {
        setOutput("goal_pose", supply_pose);
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                             "[LateGameDecision] 低血量/弹量不足，前往补给点(%s)", supply_pose.c_str());
        return BT::NodeStatus::SUCCESS;
    }

    // Step 3: 补给完成后决策
    if (hp >= recovery_hp_thr || ammo >= recovery_ammo_thr) {
        int diff = static_cast<int>(data_manager_->getEnemyBaseHp(is_blue)) -
                   static_cast<int>(data_manager_->getOwnBaseHp(is_blue));
        if (diff >= hp_diff_thr) {
            setOutput("goal_pose", attack_pose);
            RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                                 "[LateGameDecision] 冲家! diff=%d 目标:(%s)", diff, attack_pose.c_str());
        } else {
            setOutput("goal_pose", defend_pose);
            RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                                 "[LateGameDecision] 保家! diff=%d 目标:(%s)", diff, defend_pose.c_str());
        }
        return BT::NodeStatus::SUCCESS;
    }

    // 其他情况
    return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

// 注册节点
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::LateGameDecision>("LateGameDecision");
} 
