#include "rush_enemy_buff.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

RushEnemyBuff::RushEnemyBuff(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config) {
    // Get ROS2 node from blackboard
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }

    // Subscribe to game status to obtain remaining time
    game_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            remain_time_.store(msg->stage_remain_time);
        });

    // Subscribe to robot status to know team color
    status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
        "/robot_status", 10,
        [this](const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
            is_blue_team_.store(msg->team_color);
        });

    // Subscribe to all robot hp for HP of robots 1/3/4
    hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
        "/all_robot_hp", 10,
        [this](const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
            bool blue = is_blue_team_.load();
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

BT::PortsList RushEnemyBuff::providedPorts() {
    return {
        BT::InputPort<int>("min_remain", 180, "最小剩余时间 (s)"),
        BT::InputPort<int>("max_remain", 240, "最大剩余时间 (s)"),
        BT::InputPort<int>("hp_threshold", 200, "血量阈值"),
        BT::InputPort<std::string>("enemy_buff_pose", "0;0;0;0;0;0;1", "对方堡垒点目标位姿"),
        BT::OutputPort<std::string>("goal_pose", "目标点")
    };
}

BT::NodeStatus RushEnemyBuff::tick() {
    int min_r, max_r, hp_thr;
    std::string pose;

    getInput("min_remain", min_r);
    getInput("max_remain", max_r);
    getInput("hp_threshold", hp_thr);
    getInput("enemy_buff_pose", pose);

    uint16_t remain = remain_time_.load();
    uint16_t hp1 = robot1_hp_.load();
    uint16_t hp3 = robot3_hp_.load();
    uint16_t hp4 = robot4_hp_.load();

    bool time_ok = (remain >= static_cast<uint16_t>(min_r)) && (remain <= static_cast<uint16_t>(max_r));
    bool hp_ok = (hp1 > hp_thr) && (hp3 > hp_thr) && (hp4 > hp_thr);

    if (time_ok && hp_ok) {
        setOutput("goal_pose", pose);
        RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 1000,
                             "[RushEnemyBuff] 条件满足，冲击对方增益点(%s)", pose.c_str());
        return BT::NodeStatus::SUCCESS;
    }
    return BT::NodeStatus::FAILURE;
}

} // namespace rm_sentry_decision

// Register node
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::RushEnemyBuff>("RushEnemyBuff");
} 