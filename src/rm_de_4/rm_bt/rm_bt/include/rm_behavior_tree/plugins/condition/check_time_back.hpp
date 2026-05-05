#ifndef RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_TIME_BACK_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_TIME_BACK_HPP_
#include "behaviortree_cpp/condition_node.h"
#include "std_msgs/msg/u_int8.hpp"
#include "std_msgs/msg/string.hpp"


namespace rm_behavior_tree
{
    /**
     * @brief condition节点，用于判断比赛是否检测到敌人
     * @param[in] set_minutes 消息类型，包含敌人信息
     * @param[in] game_time 比赛时间，单位：分钟。当比赛时间小于分钟时，哨兵回到基地进行防守 
     */
    class IsTimeBackcondition : public BT::SimpleConditionNode
    {
        public:
            IsTimeBackcondition(const std::string & name, const BT::NodeConfig & config);

            BT::NodeStatus checkTimeBack();

            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("set_minutes"),
                    BT::InputPort<std_msgs::msg::UInt8>("game_time"),
                };
            }
    };
}   // namespace rm_behavior_tree

#endif //RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_TIME_BACK_HPP_