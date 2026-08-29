#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_PROJECTILE_NUMBER_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_PROJECTILE_NUMBER_HPP_

#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "std_msgs/msg/u_int16.hpp"

namespace rm_behavior_tree
{
        /**
     * @brief 该 action 订阅比赛时间
     * @param[in] topic_name 输入端 订阅比赛时间的话题名
     * @param[in] projectile_number 输出端口 表示但前允许发弹量
     */
    class SubProjectileNumberAction : public BT::RosTopicSubNode<std_msgs::msg::UInt16>
    {
        public:
            SubProjectileNumberAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<std_msgs::msg::UInt16>("projectile_number")};
            }
            BT::NodeStatus onTick(
                const std::shared_ptr<std_msgs::msg::UInt16> & last_msg
            ) override;
    };
}
#endif  //RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_PROJECTILE_NUMBER_HPP_