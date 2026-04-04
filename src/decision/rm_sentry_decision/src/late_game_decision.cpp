#include "late_game_decision.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

LateGameDecision::LateGameDecision(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config)
{
    // Get ROS node from blackboard
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");

    // Subscribe game status to get remaining time
    game_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            stage_remain_time_.store(msg->stage_remain_time);
        });

    // Subscribe robot status for HP
    robot_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
        "/robot_status", 10,
        [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
            current_hp_.store(msg->current_hp);
            is_blue_team_.store(msg->team_color);
        });

    // Subscribe bullet allowance (real ammo info)
    bullet_allowance_sub_ = node_->create_subscription<rm_decision_interfaces::msg::BulletAllowance>(
        "/bullet_allowance", 10,
        [this](const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg) {
            // 以 17mm 弹量作为决策参考，可根据实际弹种调整
            ammo_.store(msg->bullet_allowance_17mm);
        });

    // Subscribe all robot HP to know base HPs
    all_hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
        "/all_robot_hp", 10,
        [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
            bool blue = is_blue_team_.load();
            own_base_hp_.store(blue ? msg->blue_base_hp : msg->red_base_hp);
            enemy_base_hp_.store(blue ? msg->red_base_hp : msg->blue_base_hp);

            // --- store own robots 1/3/4 HP for buff point decision ---
            if (blue) {
                robot1_hp_.store(msg->blue_1_robot_hp);
                robot3_hp_.store(msg->blue_3_robot_hp);
                robot4_hp_.store(msg->blue_4_robot_hp);
            } else {
                robot1_hp_.store(msg->red_1_robot_hp);
                robot3_hp_.store(msg->red_3_robot_hp);
                robot4_hp_.store(msg->red_4_robot_hp);
            }
        });
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

    const uint16_t remain = stage_remain_time_.load();
    const uint16_t hp = current_hp_.load();
    const uint16_t ammo = ammo_.load();

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
    bool need_buff_guard = (robot1_hp_.load() == 0 && robot3_hp_.load() == 0 && robot4_hp_.load() == 0);
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
        int diff = static_cast<int>(enemy_base_hp_.load()) - static_cast<int>(own_base_hp_.load());
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