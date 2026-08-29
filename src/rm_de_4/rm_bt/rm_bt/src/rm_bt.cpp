// #include "behaviortree_cpp/bt_factory.h"

// // file that contains the custom nodes definitions
// #include "rm_bt/dummy_nodes.h"
// using namespace DummyNodes;

// int main()
// {  
//   BT::BehaviorTreeFactory factory;
//   factory.registerNodeType<CalculateGoal>("CalculateGoal");
//   factory.registerNodeType<PrintTarget>("PrintTarget");

//   auto tree = factory.createTreeFromFile("./rm_bt_tree.xml");
//   tree.tickWhileRunning();
//   return 0;
// }

#include "behaviortree_cpp/bt_factory.h"

#include <behaviortree_ros2/bt_action_node.hpp>
#include "behaviortree_ros2/plugins.hpp"
#include "behaviortree_cpp/loggers/groot2_publisher.h"

#include "rm_behavior_tree/bt_conversions.hpp"

// file that contains the custom nodes definitions

int main(int argc, char ** argv)
{  
  rclcpp::init(argc, argv);
  BT::BehaviorTreeFactory factory;

  BT::RosNodeParams params_update_msg;
  params_update_msg.nh = std::make_shared<rclcpp::Node>("update_msg");

  BT::RosNodeParams params_send_goal;
  params_send_goal.nh = std::make_shared<rclcpp::Node>("send_goal");
  params_send_goal.default_port_value = "goal_pose";

  // clang-format off
  const std::vector<std::string> msg_update_plugin_libs = {
    "sub_game_status",
    "sub_game_time",
    "sub_BothCampHp",
    "sub_is_detect_enemy",
    "sub_decision_status",
    "pub_float",
    "pub_current_area",
    "pub_current_location",
    "sub_projectile_number",
    "sub_enemy_outpost_status"
  };

  const std::vector<std::string> bt_plugin_libs = {
    "check_game_start",
    "check_detect_enemy",
    "restores_health",
    "check_enemy_outpost_die",
    "restores_hp_or_pill",
    "check_time_back"
  };
  // clang-format on

  for (const auto & p : msg_update_plugin_libs) {
    RegisterRosNode(factory, BT::SharedLibrary::getOSName(p), params_update_msg);
  }

  for (const auto & p : bt_plugin_libs) {
    factory.registerFromPlugin(BT::SharedLibrary::getOSName(p));
  }

  RegisterRosNode(factory, BT::SharedLibrary::getOSName("send_goal"), params_send_goal);
  auto tree = factory.createTreeFromFile("./rm_bt_RMUC2.xml");

  // Connect the Groot2Publisher. This will allow Groot2 to get the tree and poll status updates.
  //const unsigned port = 1667;
  //BT::Groot2Publisher publisher(tree, port);

  while (rclcpp::ok()) {
    tree.tickWhileRunning(std::chrono::milliseconds(10));
  }
  
  rclcpp::shutdown();
  return 0;
}
