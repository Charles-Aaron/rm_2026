#ifndef RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_GAME_START_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_GAME_START_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "std_msgs/msg/u_int8.hpp"

namespace rm_behavior_tree
{
    /**
     * @brief condition节点，用于判断比赛是否开始
     * @param[in] status 比赛状态 {0, 未开始比赛} {1, 准备阶段} {2, 15 s 裁判系统自检阶段} {3, 5s 倒计时} {4, 比赛中} {5, 比赛结算中}
     */
    class ISGameStartcondition : public BT::SimpleConditionNode
    {
        public:
            ISGameStartcondition(const std::string & name, const BT::NodeConfig & config);

            BT::NodeStatus checkGameStart();

            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std_msgs::msg::UInt8>("message")
                };
            }
    };
}   // namespace rm_behavior_tree

#endif //RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_GAME_START_HPP_