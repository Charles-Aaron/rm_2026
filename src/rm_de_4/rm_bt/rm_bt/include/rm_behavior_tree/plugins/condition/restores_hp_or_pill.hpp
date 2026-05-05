#ifndef RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HP_OR_PILL_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HP_OR_PILL_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "rm_sentry_decision_interfaces/msg/both_camp_hp.hpp"
#include "std_msgs/msg/int16.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "std_msgs/msg/u_int16.hpp"


namespace rm_behavior_tree
{
    /**
     * @brief 该 condition 行为树节点用于判断己方哨兵机器人是否回补给区补血
     * @param[in] both_camp_hp 包含双方阵营血量信息
     * @param[in] sentry_blood_threshold 己方哨兵机器人血量阈值，低于该血量哨兵回基地恢复血量
     * @param[in] sentry_full_blood 己方哨兵满血值
     * @param[in] game_time 比赛已经开始的时间，单位：分钟，整型，用于判断哨兵是否补充弹药
     * @param[in] projectile_number 己方哨兵机器人允许发弹量
     */
    class IfRestoresHpOrPillcondition : public BT::SimpleConditionNode
    {
        public:
            //判断条件标志位
            int hp_flag=0;//用于判断哨兵血量是否回满,1表示未回满血量
            int pill_flag_ready=0;//用于判断哨兵是否补充弹药,1表示哨兵当前可以占领补给区获取允许发弹量
            int minutes = 0; //比赛时间，单位：分钟
            int minutes_flag = 0; //用于辅助判断是否能领取弹丸
            int pill_past_number=0; //上一次弹丸数量，用于辅助判断哨兵是否已经补充完毕子弹

            IfRestoresHpOrPillcondition(const std::string & name, const BT::NodeConfig & config);
            BT::NodeStatus IfRestorespOrPill();

            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<rm_sentry_decision_interfaces::msg::BothCampHp>("both_camp_hp"),
                    BT::InputPort<std_msgs::msg::Int16>("sentry_blood_threshold"),
                    BT::InputPort<std_msgs::msg::Int16>("sentry_full_blood"),
                    BT::InputPort<std_msgs::msg::UInt8>("game_time"),
                    BT::InputPort<std_msgs::msg::UInt16>("projectile_number")
                };
            }
    };
}   // namespace rm_behavior_tree

#endif //RM_BEHAVIOR_TREE__PLUGINS__CONDITION__RESTORES_HP_OR_PILL_HPP_