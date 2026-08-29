#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_GAME_TIME_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_GAME_TIME_HPP_

#include "behaviortree_ros2/bt_topic_sub_node.hpp"
#include "std_msgs/msg/u_int8.hpp"

namespace rm_behavior_tree
{
        /**
     * @brief 该 action 订阅比赛时间
     * @param[in] topic_name 输入端 订阅比赛时间的话题名
     * @param[in] game_time 输出端口，整型，表示当前比赛过了多少分钟，用于给哨兵补充弹丸判断
     */
    class SubGameTimeAction : public BT::RosTopicSubNode<std_msgs::msg::UInt8>
    {
        public:
            SubGameTimeAction(
                const std::string & name,const BT::NodeConfig & config,const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::OutputPort<std_msgs::msg::UInt8>("game_time")};
            }
        BT::NodeStatus onTick(
            const std::shared_ptr<std_msgs::msg::UInt8> & last_msg
        ) override;
    };
}
#endif  // RM_BEHAVIOR_TREE__PLUGINS__ACTION__SUB_GAME_TIME_HPP_