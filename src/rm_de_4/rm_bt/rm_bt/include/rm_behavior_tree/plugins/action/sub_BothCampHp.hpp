#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_BOTHCAMPHP_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_BOTHCAMPHP_HPP_

#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "rm_sentry_decision_interfaces/msg/both_camp_hp.hpp"



namespace rm_behavior_tree
{
    /**
     * @brief 该 action 订阅己方阵营机器人的血量
     * @param[in] topic_name 订阅己方阵营机器人的血量的话题名称，在groot2出输入
     * @param[in] both_camp_hp 订阅己方阵营机器人的血量后的输出端口
     */
    class SubBothCampHpAction : public BT::RosTopicSubNode<rm_sentry_decision_interfaces::msg::BothCampHp>
    {
        public:
            SubBothCampHpAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<rm_sentry_decision_interfaces::msg::BothCampHp>("both_camp_hp")};
            }
        BT::NodeStatus onTick(
            const std::shared_ptr<rm_sentry_decision_interfaces::msg::BothCampHp> & last_msg
        ) override;
    };
}
#endif  // RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_BOTHCAMPHP_HPP_