#include "rm_behavior_tree/plugins/action/sub_projectile_number.hpp"
#include "std_msgs/msg/u_int16.hpp"

namespace rm_behavior_tree
{
    SubProjectileNumberAction::SubProjectileNumberAction(
        const std::string & name, const BT::NodeConfig & conf, const BT::RosNodeParams & params
    ):BT::RosTopicSubNode<std_msgs::msg::UInt16>(name, conf, params)
    {
    }

    BT::NodeStatus SubProjectileNumberAction::onTick(
        const std::shared_ptr<std_msgs::msg::UInt16> & last_msg
    )
    {
        if (last_msg) // empty if no new message received, since the last tick
        {
            setOutput("projectile_number", *last_msg); 
        }
        return BT::NodeStatus::SUCCESS;
    }
}   // namespace rm_behavior_tree

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(rm_behavior_tree::SubProjectileNumberAction, "SubProjectileNumber");