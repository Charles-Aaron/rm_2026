#include "behaviortree_cpp/bt_factory.h"
#include "std_msgs/msg/u_int8.hpp"
#include "rm_behavior_tree/plugins/condition/check_game_start.hpp"

namespace rm_behavior_tree
{
    ISGameStartcondition::ISGameStartcondition(const std::string & name, const BT::NodeConfig & config
    ): BT::SimpleConditionNode(name, std::bind(&ISGameStartcondition::checkGameStart, this), config)
    {
    }
    BT::NodeStatus ISGameStartcondition::checkGameStart()
    {
        //int message;
        auto msg = getInput<std_msgs::msg::UInt8>("message");
        if (!msg)
        {
            return BT::NodeStatus::FAILURE;
        }
        //u_int8 与 int 不能直接比较，需要显示比较
        if (msg.value().data == static_cast<uint8_t>(1))
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
    factory.registerNodeType<rm_behavior_tree::ISGameStartcondition>("ISGameStart");
}