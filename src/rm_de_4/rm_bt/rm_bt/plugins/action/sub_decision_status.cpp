#include "rm_behavior_tree/plugins/action/sub_decision_status.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include <string>

namespace rm_behavior_tree
{
    SubDecisionStatusAction::SubDecisionStatusAction(
        const std::string & name, const BT::NodeConfig & conf, const BT::RosNodeParams & params
    ):BT::RosTopicSubNode<std_msgs::msg::UInt8>(name, conf, params)
    {
    }

    BT::NodeStatus SubDecisionStatusAction::onTick(
        const std::shared_ptr<std_msgs::msg::UInt8> & last_msg
    )
    {
        if (last_msg) // empty if no new message received, since the last tick
        {
            setOutput("game_decision_status", std::to_string(last_msg->data));
        }
        return BT::NodeStatus::SUCCESS;
    }
}   // namespace rm_behavior_tree

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(rm_behavior_tree::SubDecisionStatusAction, "SubDecisionStatus");