#include "rm_behavior_tree/plugins/action/pub_float.hpp"
#include "std_msgs/msg/float32.hpp"

namespace rm_behavior_tree
{
    PubFloatAction::PubFloatAction(
        const std::string & name, const BT::NodeConfig & conf, const BT::RosNodeParams & params
    ):BT::RosTopicPubNode<std_msgs::msg::Float32>(name, conf, params)
    {
    }

    bool PubFloatAction::setMessage(std_msgs::msg::Float32 & msg)
    {
        auto input = getInput<float>("float_data");
        if (!input)
        {
            RCLCPP_ERROR(node_->get_logger(), "Failed to get input data: %s", input.error().c_str());
            return false;
        }
        msg.data = input.value();
        return true;
    }
}   // namespace rm_behavior_tree

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(rm_behavior_tree::PubFloatAction, "PubFloat");
