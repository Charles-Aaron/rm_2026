#include "behaviortree_cpp/bt_factory.h"
#include "rm_behavior_tree/plugins/condition/restores_hp_or_pill.hpp"
#include "rm_behavior_tree/bt_conversions_StringToInt.hpp"

namespace rm_behavior_tree
{
    IfRestoresHpOrPillcondition::IfRestoresHpOrPillcondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&IfRestoresHpOrPillcondition::IfRestorespOrPill, this), config)
    {
    }
    BT::NodeStatus IfRestoresHpOrPillcondition::IfRestorespOrPill()
    {
        auto both_camp_hp = getInput<rm_sentry_decision_interfaces::msg::BothCampHp>("both_camp_hp");
        auto sentry_hp_threshold = getInput<std_msgs::msg::Int16>("sentry_blood_threshold");
        auto sentry_full_hp = getInput<std_msgs::msg::Int16>("sentry_full_blood");
        auto projectile_number = getInput<std_msgs::msg::UInt16>("projectile_number");
        auto temp_minutes = getInput<std_msgs::msg::UInt8>("game_time");
        
        if (!both_camp_hp || !sentry_hp_threshold || !sentry_full_hp || !projectile_number || !temp_minutes)
        {
            return BT::NodeStatus::FAILURE;
        }

        IfRestoresHpOrPillcondition::minutes = static_cast<int>(temp_minutes.value().data);
        //用于判断哨兵是否已经补充子弹，如果已经补充，pill_flag_ready置0 
        if (projectile_number.value().data > IfRestoresHpOrPillcondition::pill_past_number)
        {
            IfRestoresHpOrPillcondition::pill_flag_ready = 0;
        }
        IfRestoresHpOrPillcondition::pill_past_number = projectile_number.value().data;
        
        //用于判断免费领取的弹丸数量是否刷新
        if (IfRestoresHpOrPillcondition::minutes != IfRestoresHpOrPillcondition::minutes_flag)
        {
            IfRestoresHpOrPillcondition::pill_flag_ready = 1;
        }
        IfRestoresHpOrPillcondition::minutes_flag = IfRestoresHpOrPillcondition::minutes;

        //用于判断当前哨兵是否需要去补充血量或者弹丸
        if (both_camp_hp.value().own_sentry_hp <= sentry_hp_threshold.value().data)
        {
            IfRestoresHpOrPillcondition::hp_flag = 1;
            return BT::NodeStatus::SUCCESS;
        }
        else if (IfRestoresHpOrPillcondition::hp_flag == 1 && both_camp_hp.value().own_sentry_hp < sentry_full_hp.value().data)
        {
            IfRestoresHpOrPillcondition::pill_flag_ready = 0;
            return BT::NodeStatus::SUCCESS;
        }
        else if (IfRestoresHpOrPillcondition::pill_flag_ready == 1 && projectile_number.value().data == 0)
        {
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            IfRestoresHpOrPillcondition::hp_flag = 0;
            return BT::NodeStatus::FAILURE;
        }
    }
}   //namespace rm_behavior_tree


BT_REGISTER_NODES(factory)
{
    factory.registerNodeType<rm_behavior_tree::IfRestoresHpOrPillcondition>("IfRestoresHpOrPill");
}