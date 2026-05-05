#include "behaviortree_cpp/bt_factory.h"
#include "rm_behavior_tree/plugins/condition/restores_health.hpp"
#include "rm_behavior_tree/bt_conversions_StringToInt.hpp"

namespace rm_behavior_tree
{
    IfRestoresHealthcondition::IfRestoresHealthcondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&IfRestoresHealthcondition::IfRestoresHealth, this), config)
    {
    }
    BT::NodeStatus IfRestoresHealthcondition::IfRestoresHealth()
    {
        auto both_camp_hp = getInput<rm_sentry_decision_interfaces::msg::BothCampHp>("both_camp_hp");
        auto sentry_hp_threshold = getInput<std_msgs::msg::Int16>("sentry_blood_threshold");
        auto sentry_full_hp = getInput<std_msgs::msg::Int16>("sentry_full_blood");
        if (!both_camp_hp || !sentry_hp_threshold)
        {
            return BT::NodeStatus::FAILURE;
        }
        if (both_camp_hp.value().own_sentry_hp <= sentry_hp_threshold.value().data)
        {
            IfRestoresHealthcondition::flag = 1;
            return BT::NodeStatus::SUCCESS;
        }
        else if (IfRestoresHealthcondition::flag == 1 && both_camp_hp.value().own_sentry_hp < sentry_full_hp.value().data)
        {
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            IfRestoresHealthcondition::flag = 0;
            return BT::NodeStatus::FAILURE;
        }
    }
}   //namespace rm_behavior_tree


BT_REGISTER_NODES(factory)
{
    factory.registerNodeType<rm_behavior_tree::IfRestoresHealthcondition>("IfRestoresHealth");
}