#include "behaviortree_cpp/bt_factory.h"
#include "rm_behavior_tree/plugins/condition/check_time_back.hpp"

namespace rm_behavior_tree
{
    IsTimeBackcondition::IsTimeBackcondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&IsTimeBackcondition::checkTimeBack, this), config)
    {
    }
    BT::NodeStatus IsTimeBackcondition::checkTimeBack()
    {
        auto game_time = getInput<std_msgs::msg::UInt8>("game_time");
        auto set_minutes = getInput<std::string>("set_minutes");
        if (!game_time || !set_minutes)
        {
            return BT::NodeStatus::FAILURE;
        }
        //u_int8 与 int 不能直接比较，需要显示比较
        if (game_time.value().data >= static_cast<uint8_t>(std::stoi(set_minutes.value())))
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
    factory.registerNodeType<rm_behavior_tree::IsTimeBackcondition>("IsTimeBack");  
}