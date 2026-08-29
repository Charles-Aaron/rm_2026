#include "behaviortree_cpp/bt_factory.h"
#include "rm_behavior_tree/plugins/condition/check_enemy_outpost_die.hpp"

namespace rm_behavior_tree
{
    IsEnemyOutpostDiecondition::IsEnemyOutpostDiecondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&IsEnemyOutpostDiecondition::CheckEnemyOutpostDie, this), config)
    {
    }
    BT::NodeStatus IsEnemyOutpostDiecondition::CheckEnemyOutpostDie()
    {
        auto both_camp_hp = getInput<std_msgs::msg::UInt8>("enemy_outpost_status");
        if (both_camp_hp.value().data == 0 )
        {
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            return BT::NodeStatus::FAILURE;
        }
    }
}   //namespace rm_behavior_tree


BT_REGISTER_NODES(factory)
{
    factory.registerNodeType<rm_behavior_tree::IsEnemyOutpostDiecondition>("IfEnemyOutpostDie");
}   