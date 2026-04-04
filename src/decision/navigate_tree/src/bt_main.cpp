#include "rclcpp/rclcpp.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "condition_start.h"
#include "patrol.h"
#include "condition_protect.h"
#include "action_start35.h"
#include "condition_operator.h"
#include "action_operator.h"
#include "condition_enemy.h"
#include "action_follow.h"
#include "condition_blosupeng.h"
#include "action_blosupeng.h"
#include "action_tower.h"
#include "condition_tower.h"
#include "action_home.h"
#include "condition_home.h"
#include "action_fort.h"
#include "condition_fort.h"
#include "action_free.h"
#include "condition_free.h"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("bt_navigation_node");

    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<condition_start>("condition_start");
    factory.registerNodeType<condition_protect>("condition_protect");
    factory.registerNodeType<condition_operator>("condition_operator");
    factory.registerNodeType<condition_enemy>("condition_enemy");
    factory.registerNodeType<condition_blosupeng>("condition_blosupeng");
    factory.registerNodeType<condition_tower>("condition_tower");
    factory.registerNodeType<condition_home>("condition_home");
    factory.registerNodeType<condition_free>("condition_free");
    factory.registerNodeType<condition_fort>("condition_fort");
    factory.registerNodeType<action_home>("action_home");
    factory.registerNodeType<action_tower>("action_tower");
    factory.registerNodeType<action_start35>("action_start35");
    factory.registerNodeType<action_blosupeng>("action_blosupeng");
    factory.registerNodeType<action_operator>("action_operator");
    factory.registerNodeType<action_follow>("action_follow");
    factory.registerNodeType<action_free>("action_free");
    factory.registerNodeType<action_fort>("action_fort");


    auto blackboard = BT::Blackboard::create();
    blackboard->set("node", node);

    const auto behavior_tree_path =
        ament_index_cpp::get_package_share_directory("navigate_tree") + "/config/behavior_tree.xml";
    auto tree = factory.createTreeFromFile(behavior_tree_path, blackboard);

    while (rclcpp::ok()) {
        tree.tickRoot();
         BT::printTreeRecursively(tree.rootNode());
        rclcpp::spin_some(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    rclcpp::shutdown();
    return 0;
}
