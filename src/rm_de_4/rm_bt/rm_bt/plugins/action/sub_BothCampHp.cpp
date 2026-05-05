#include "rm_behavior_tree/plugins/action/sub_BothCampHp.hpp"

namespace rm_behavior_tree
{
    SubBothCampHpAction::SubBothCampHpAction(
        const std::string & name, const BT::NodeConfig & conf, const BT::RosNodeParams & params
    ):BT::RosTopicSubNode<rm_sentry_decision_interfaces::msg::BothCampHp>(name, conf, params)
    {
    }

    BT::NodeStatus SubBothCampHpAction::onTick(
        const std::shared_ptr<rm_sentry_decision_interfaces::msg::BothCampHp> & last_msg
    )
    {
        if (last_msg) // empty if no new message received, since the last tick
        {
            RCLCPP_DEBUG(
                logger(),
                "接收到最新血量信息\n"
                "我方基地血量为 [%d]\n"
                "我方前哨战血量为 [%d]\n"
                "我方一号步兵机器人机器人血量为 [%d]\n"
                "我方二号步兵机器人机器人血量为 [%d]\n"
                "我方英雄机器人机器人血量为 [%d]\n"
                "我方哨兵机器人血量为 [%d]\n\n"
                "敌方基地血量为 [%d]\n"
                "敌方前哨战血量为 [%d]\n"
                "敌方一号步兵机器人机器人血量为 [%d]\n"
                "敌方二号步兵机器人机器人血量为 [%d]\n"
                "敌方英雄机器人机器人血量为 [%d]\n"
                "敌方哨兵机器人血量为 [%d]\n",
                last_msg->own_base_hp,
                last_msg->own_outpost_hp,
                last_msg->own_infantry1_hp,
                last_msg->own_infantry2_hp,
                last_msg->own_hero_hp,
                last_msg->own_sentry_hp,
                last_msg->enemy_base_hp,
                last_msg->enemy_outpost_hp,
                last_msg->enemy_infantry1_hp,
                last_msg->enemy_infantry2_hp,
                last_msg->enemy_hero_hp,
                last_msg->enemy_sentry_hp
            );
            setOutput("both_camp_hp", *last_msg);
        }
        return BT::NodeStatus::SUCCESS;
    }
}   // namespace rm_behavior_tree

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(rm_behavior_tree::SubBothCampHpAction, "SubBothCampHp");