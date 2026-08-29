#ifndef RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_FLOAT_HPP_
#define RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_FLOAT_HPP_

#include "behaviortree_ros2/bt_topic_pub_node.hpp"
#include "std_msgs/msg/float32.hpp"

namespace rm_behavior_tree
{
    /**
     * @brief 该 action 发布浮点数数据
     * @param[in] topic_name 输入端 话题名称
     * @param[in] float_data 输入端 发布的数据
     */
    class PubFloatAction : public BT::RosTopicPubNode<std_msgs::msg::Float32>
    {
        public:
            PubFloatAction(
                const std::string & name, const BT::NodeConfig & config, const BT::RosNodeParams & params);
            static BT::PortsList providedPorts()
            {
                return{
                    BT::InputPort<std::string>("topic_name"),
                    BT::InputPort<float>("float_data")
                };
            }
            bool setMessage(std_msgs::msg::Float32 & msg) override;
    };
}   // namespace rm_behavior_tree

#endif  //RM_BEHAVIOR_TREE__PLUGINS__ACTION__PUB_FLOAT_HPP_
