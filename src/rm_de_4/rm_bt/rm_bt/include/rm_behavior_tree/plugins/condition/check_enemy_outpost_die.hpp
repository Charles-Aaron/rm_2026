#ifndef RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_ENEMY_OUTPOST_DIE_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_ENEMY_OUTPOST_DIE_HPP_
#include "behaviortree_cpp/condition_node.h"
#include "std_msgs/msg/u_int8.hpp"
#include "rm_sentry_decision_interfaces/msg/both_camp_hp.hpp"

namespace rm_behavior_tree
{
    /**
     * @brief condition节点，用于判断比赛过程中，敌方阵营前哨站是否死亡
     * @param[in] both_camp_hp 包含双方阵营血量信息，包含敌方前哨站血量
     */
    class IsEnemyOutpostDiecondition : public BT::SimpleConditionNode
    {
        public:
            IsEnemyOutpostDiecondition(const std::string & name, const BT::NodeConfig & config);

            BT::NodeStatus CheckEnemyOutpostDie();

            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std_msgs::msg::UInt8>("enemy_outpost_status", "敌方前哨站状态"),
                };
            }
    };
}   // namespace rm_behavior_tree

#endif //RM_BEHAVIOR_TREE__PLUGINS__CONDITION__CHECK_ENEMY_OUTPOST_DIE_HPP_