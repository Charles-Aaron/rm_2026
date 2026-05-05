#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_DECISION_STATUS_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_DECISION_STATUS_HPP_

#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "std_msgs/msg/u_int8.hpp"

namespace rm_behavior_tree
{
        /**
     * @brief 该 action 订阅比赛状态
     * @param[in] topic_name 输入端 订阅决策状态的话题名
     * @param[in] decision_status 输出端 输出决策状态 {"1"：决策1，表示进攻} {"2"：决策2，表示防守} {"3"：决策3，表示进攻兼防守} 
     */
    class SubDecisionStatusAction : public BT::RosTopicSubNode<std_msgs::msg::UInt8>
    {
        public:
            SubDecisionStatusAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<std::string>("game_decision_status")};
            }
        BT::NodeStatus onTick(
            const std::shared_ptr<std_msgs::msg::UInt8> & last_msg
        ) override;
    };
}
#endif  // RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_DECISION_STATUS_HPP_