#include <rclcpp/rclcpp.hpp>
#include <memory>
#include "ego_planner_ros2/ego_planner.h"
#include "nav_msgs/msg/odometry.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  
  auto node = std::make_shared<rclcpp::Node>("ego_planner_node");
  
  RCLCPP_INFO(node->get_logger(), "Ego Planner Node Starting...");
  
  node->declare_parameter("use_sim_time", false);
  node->declare_parameter("pointcloud_topic", std::string("/terrain_map"));
  node->declare_parameter("robot_pose_topic", std::string("/odom"));
  node->declare_parameter("output_path_topic", std::string("/plan"));
  node->declare_parameter("global_path_topic", std::string("/global_path"));
  
  std::string pointcloud_topic, robot_pose_topic, output_path_topic, global_path_topic;
  node->get_parameter("pointcloud_topic", pointcloud_topic);
  node->get_parameter("robot_pose_topic", robot_pose_topic);
  node->get_parameter("output_path_topic", output_path_topic);
  node->get_parameter("global_path_topic", global_path_topic);
  
  auto planner = std::make_unique<ego_planner_ros2::EgoPlanner>();
  planner->initialize(node);
  
  auto path_pub = node->create_publisher<nav_msgs::msg::Path>(output_path_topic, 10);
  
  geometry_msgs::msg::Pose current_pose;
  geometry_msgs::msg::Pose goal_pose;
  bool goal_received = false;
  
  auto pose_sub = node->create_subscription<nav_msgs::msg::Odometry>(
    robot_pose_topic, 10,
    [&planner, &current_pose](const nav_msgs::msg::Odometry::SharedPtr msg) {
      current_pose = msg->pose.pose;
      planner->setStart(current_pose);
    });
  
  auto goal_sub = node->create_subscription<geometry_msgs::msg::PoseStamped>(
    "/goal_pose", 10,
    [&planner, &goal_pose, &goal_received](const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
      goal_pose = msg->pose;
      goal_received = true;
      planner->setGoal(goal_pose);
    });
  
  auto pointcloud_sub = node->create_subscription<sensor_msgs::msg::PointCloud2>(
    pointcloud_topic, 5,
    [&planner](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
      planner->setMap(msg);
    });
  
  auto global_path_sub = node->create_subscription<nav_msgs::msg::Path>(
    global_path_topic, 5,
    [&planner](const nav_msgs::msg::Path::SharedPtr msg) {
      planner->setGlobalPath(msg);
    });
  
  rclcpp::Rate rate(10);
  
  RCLCPP_INFO(node->get_logger(), "Ego Planner Node Started.");
  
  while (rclcpp::ok()) {
    rclcpp::spin_some(node);
    
    if (goal_received) {
      if (planner->checkCollision()) {
        RCLCPP_INFO(node->get_logger(), "Collision detected, replanning...");
      }
      
      nav_msgs::msg::Path path;
      if (planner->makePlan(path)) {
        path_pub->publish(path);
      }
    }
    
    rate.sleep();
  }
  
  rclcpp::shutdown();
  return 0;
}
