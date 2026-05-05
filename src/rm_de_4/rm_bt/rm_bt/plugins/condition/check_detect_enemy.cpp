#include "behaviortree_cpp/bt_factory.h"
#include "std_msgs/msg/u_int8.hpp"
#include "rm_behavior_tree/plugins/condition/check_detect_enemy.hpp"

namespace rm_behavior_tree
{
    IsDetectEnemycondition::IsDetectEnemycondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&IsDetectEnemycondition::checkDetectEnemy, this), config)
    {
    }
    BT::NodeStatus IsDetectEnemycondition::checkDetectEnemy()
    {
        //int message;
        auto msg = getInput<std_msgs::msg::UInt8>("message");
        if (!msg)
        {
            return BT::NodeStatus::FAILURE;
        }
        //需要显示比较
        if (msg.value().data == static_cast<uint8_t>(1) || msg.value().data == static_cast<uint8_t>(2))
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
    factory.registerNodeType<rm_behavior_tree::IsDetectEnemycondition>("IsDetectEnemy");
}