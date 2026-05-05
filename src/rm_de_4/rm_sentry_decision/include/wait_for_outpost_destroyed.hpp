#ifndef WAIT_FOR_OUTPOST_DESTROYED_HPP
#define WAIT_FOR_OUTPOST_DESTROYED_HPP

#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"
#include <atomic>

namespace rm_sentry_decision {

class WaitForOutpostDestroyed : public BT::StatefulActionNode {
public:
    WaitForOutpostDestroyed(
        const std::string& name,
        const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {};
    }
    
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

private:
  
    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr outpost_hp_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr status_sub_;
    
    std::atomic<int> enemy_outpost_hp_;
    std::atomic<bool> is_blue_team_;
    bool initial_hp_received_;
};

} // namespace rm_sentry_decision

#endif // WAIT_FOR_OUTPOST_DESTROYED_HPP