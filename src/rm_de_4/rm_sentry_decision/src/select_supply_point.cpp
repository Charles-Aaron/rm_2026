#include "select_supply_point.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "rm_sentry_decision/parameter_manager.hpp"

namespace rm_sentry_decision {

SelectSupplyPoint::SelectSupplyPoint(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("SelectSupplyPoint: Failed to get ROS node from blackboard");
    }

    // 初始化参数管理器
    param_manager_ = ParameterManager::getInstance(node_);

    // 自己血量与队伍颜色
    status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        [this](const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
            self_hp_.store(msg->current_hp);
            remaining_ammo_.store(msg->bullet_allowance_17mm);
            is_blue_.store(msg->team_color);
            status_ready_.store(true);
            if (is_blue_.load()) {
                enemy3_hp_.store(msg->red_3_robot_hp);
                enemy4_hp_.store(msg->red_4_robot_hp);
                outpost_hp_.store(msg->blue_outpost_hp);
            } else {
                enemy3_hp_.store(msg->blue_3_robot_hp);
                enemy4_hp_.store(msg->blue_4_robot_hp);
                outpost_hp_.store(msg->red_outpost_hp);
            }
            hp_ready_.store(true);
        });

    // 全局血量
}

BT::PortsList SelectSupplyPoint::providedPorts() {
    return {
        BT::InputPort<std::string>("outpost_pose"),
        BT::InputPort<std::string>("base_pose"),
        BT::OutputPort<std::string>("goal_pose")//goal_pose smart_supply_goal
    };
}

BT::NodeStatus SelectSupplyPoint::tick() {
    // if (outpost_hp_.load() ==0) {
    //     suppressed_.store(false); 
    // }
    std::string outpost_pose, base_pose;
    getInput("outpost_pose", outpost_pose);
    getInput("base_pose", base_pose);
    if (!(status_ready_.load() && hp_ready_.load())) {
        return BT::NodeStatus::FAILURE; // 数据尚未准备
    }

    const int recover_hp   =550;   // 复位阈值
    const int recover_ammo = 50;

    int hp   = self_hp_.load();
    int ammo = remaining_ammo_.load();

    if (suppressed_.load()) {
      if (hp >= recover_hp && ammo >= recover_ammo) {
        suppressed_.store(false);  // 恢复可再次触发
        RCLCPP_INFO(node_->get_logger(), "NeedSupply reset: HP≥%d, Ammo≥%d", recover_hp, recover_ammo);
        return BT::NodeStatus::FAILURE;
      }
    else{
    //      if (outpost_hp_.load()==0) {
    //     setOutput("goal_pose", base_pose);
    //     return BT::NodeStatus::SUCCESS;
    // }
        // else{return BT::NodeStatus::SUCCESS;}
        return BT::NodeStatus::SUCCESS;
        }
    }

    //  if (outpost_hp_.load() < 400) {
    //     setOutput("goal_pose", base_pose);
    //     return BT::NodeStatus::SUCCESS;
    // }
    

    // 从参数管理器获取阈值参数
    // int hp_critical = param_manager_->getSupplyDecisionParam<int>("hp_critical");
    // int hp_low = param_manager_->getSupplyDecisionParam<int>("hp_low");
    // int hp_medium = param_manager_->getSupplyDecisionParam<int>("hp_medium");
    // int diff_small = param_manager_->getSupplyDecisionParam<int>("diff_small");
    // int diff_medium = param_manager_->getSupplyDecisionParam<int>("diff_medium");
    // int diff_large = param_manager_->getSupplyDecisionParam<int>("diff_large");
    // int enemy_dead = param_manager_->getHpDiffThreshold("enemy_dead");

    // uint16_t self_hp = self_hp_.load();
    // uint16_t e3 = enemy3_hp_.load();
    // uint16_t e4 = enemy4_hp_.load();

    // auto diff = [&](uint16_t enemy){
    //     return static_cast<int>(enemy) - static_cast<int>(self_hp);
    // };

    // int diff3 = (e3 > 0) ? diff(e3) : enemy_dead; // 敌人阵亡视作差值足够小
    // int diff4 = (e4 > 0) ? diff(e4) : enemy_dead;

    // // 读取坐标

    // auto choose = [&](const std::string &pose){
    //     setOutput("goal_pose", pose);
    //     suppressed_.store(true);
    //     return BT::NodeStatus::SUCCESS;
    // };

    // // Rule 1: HP <= hp_critical → 基地补给
    // if (self_hp <= hp_critical) {
    //     RCLCPP_DEBUG(node_->get_logger(), "SelectSupplyPoint: HP <= %d, choosing base supply", hp_critical);
    //     return choose(base_pose);
    // }

    // // Helper flags
    // bool both_ge_small = ( (e3==0) || diff3 >= diff_small) && ( (e4==0) || diff4 >= diff_small);
    // bool any_between_medium_large = ( (e3>0) && diff3 >= diff_medium && diff3 <= diff_large) || 
    //                                 ( (e4>0) && diff4 >= diff_medium && diff4 <= diff_large);
    // bool both_ge_large = ( (e3==0) || diff3 >= diff_large) && ( (e4==0) || diff4 >= diff_large);

    // // HP between hp_low and hp_medium inclusive
    // if (self_hp >= hp_low && self_hp <= hp_medium) {
    //     if (both_ge_small || any_between_medium_large) {
    //         RCLCPP_DEBUG(node_->get_logger(), "SelectSupplyPoint: HP %d-%d, choosing outpost supply", hp_low, hp_medium);
    //         return choose(outpost_pose);
    //     }
    //     if (both_ge_large) {
    //         RCLCPP_DEBUG(node_->get_logger(), "SelectSupplyPoint: HP %d-%d, large diff, choosing base supply", hp_low, hp_medium);
    //         return choose(base_pose);
    //     }
    // }

    // // HP between hp_critical and hp_low
    // if (self_hp > hp_critical && self_hp < hp_low) {
    //     if (both_ge_large) {
    //         RCLCPP_DEBUG(node_->get_logger(), "SelectSupplyPoint: HP %d-%d, large diff, choosing base supply", hp_critical, hp_low);
    //         return choose(base_pose);
    //     }
    // }

    // // No rule matched
    // RCLCPP_DEBUG(node_->get_logger(), "SelectSupplyPoint: No rule matched, defaulting to base supply");
    // setOutput("goal_pose", base_pose);  // 默认返回基地补给点而不是空字符串
    // suppressed_.store(true);
    // return BT::NodeStatus::SUCCESS;  // 改为SUCCESS，确保不会因为无匹配规则而失败
    // std::string outpost_pose, base_pose;
    // getInput("outpost_pose", outpost_pose);
    // getInput("base_pose", base_pose);

    /* 新逻辑：只看前哨血量 */
    // if (outpost_hp_.load() > 0) {
    //     setOutput("goal_pose", outpost_pose);
    // } else {
    //     setOutput("goal_pose", base_pose);
    // }
    // suppressed_.store(true);
    // return BT::NodeStatus::SUCCESS;
setOutput("goal_pose", base_pose);
suppressed_.store(true);
 return BT::NodeStatus::SUCCESS;
}
}

 // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::SelectSupplyPoint>("SelectSupplyPoint");
} 
