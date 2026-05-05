#ifndef RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HEALTH_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HEALTH_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "rm_sentry_decision_interfaces/msg/both_camp_hp.hpp"
#include "std_msgs/msg/int16.hpp"


namespace rm_behavior_tree
{
    /**
     * @brief 该 condition 行为树节点用于判断己方哨兵机器人是否回补给区补血
     * @param[in] both_camp_hp 包含双方阵营血量信息
     * @param[in] sentry_blood_threshold 己方哨兵机器人血量阈值，低于该血量哨兵回基地恢复血量
     * @param[in] sentry_full_blood 己方哨兵满血值
     */
    class IfRestoresHealthcondition : public BT::SimpleConditionNode
    {
        public:
            IfRestoresHealthcondition(const std::string & name, const BT::NodeConfig & config);
            int flag=0;//用于判断哨兵血量是否回满,1表示未回满血量
            BT::NodeStatus IfRestoresHealth();

            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<rm_sentry_decision_interfaces::msg::BothCampHp>("both_camp_hp"),
                    BT::InputPort<std_msgs::msg::Int16>("sentry_blood_threshold"),
                    BT::InputPort<std_msgs::msg::Int16>("sentry_full_blood")
                };
            }
    };
}   // namespace rm_behavior_tree

#endif //RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HEALTH_HPP_