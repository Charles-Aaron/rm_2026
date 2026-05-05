#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_IS_DETECT_ENEMY_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_IS_DETECT_ENEMY_HPP_

#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "std_msgs/msg/u_int8.hpp"
namespace rm_behavior_tree
{
        /**
     * @brief 该 action 订阅自瞄检测敌人状态
     * @param[in] topic_name 输入端 订阅比赛状态的话题名
     * @param[in] detect_enemy_status 输出端 输出比赛状态 {0, 未检测到敌人} {1, 检测到敌人}
     */
    class SubIsDetectEnemyAction : public BT::RosTopicSubNode<std_msgs::msg::UInt8>
    {
        public:
            SubIsDetectEnemyAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<std_msgs::msg::UInt8>("detect_enemy_status")};
            }
        BT::NodeStatus onTick(
            const std::shared_ptr<std_msgs::msg::UInt8> & last_msg
        ) override;
    };
}
#endif  //RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_IS_DETECT_ENEMY_HPP_