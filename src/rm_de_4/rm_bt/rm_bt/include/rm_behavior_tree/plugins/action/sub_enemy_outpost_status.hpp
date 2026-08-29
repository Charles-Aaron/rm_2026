#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_ENEMY_OUTPOST_STATUS_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_ENEMY_OUTPOST_STATUS_HPP_


#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "std_msgs/msg/u_int8.hpp"

namespace rm_behavior_tree
{
        /**
     * @brief 该 action 订阅敌方前哨站状态
     * @param[in] topic_name 输入端 订阅敌方前哨站状态的话题名
     * @param[in] enemy_outpost_status 输出端口，整型，表示敌方前哨站的状态
     */
    class SubEnemyOutpostStatusAction : public BT::RosTopicSubNode<std_msgs::msg::UInt8>
    {
        public:
            SubEnemyOutpostStatusAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<std_msgs::msg::UInt8>("enemy_outpost_status")};
            }
        BT::NodeStatus onTick(
            const std::shared_ptr<std_msgs::msg::UInt8> & last_msg
        ) override;
    };
}
#endif  //  RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_ENEMY_OUTPOST_STATUS_HPP_