#include "far_planner_ros2/far_planner.h"

namespace far_planner_ros2
{

void FARPlannerGlobalPlanner::configure(
    const rclcpp_lifecycle::LifecycleNode::WeakPtr& parent,
    std::string name,
    std::shared_ptr<tf2_ros::Buffer> tf,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros)
{
    auto node = parent.lock();
    name_ = name;
    tf_ = tf;
    costmap_ros_ = costmap_ros;
    
    node->declare_parameter(name_ + ".tolerance", 0.5);
    node->declare_parameter(name_ + ".max_planning_time", 1.0);
    node->declare_parameter(name_ + ".robot_radius", 0.2);
    node->declare_parameter(name_ + ".visibility_range", 10.0);
    node->declare_parameter(name_ + ".obstacle_threshold", 0.5);
    node->declare_parameter(name_ + ".use_pointcloud", false);
    node->declare_parameter(name_ + ".pointcloud_topic", std::string("/terrain_map"));
    
    node->get_parameter(name_ + ".tolerance", tolerance_);
    node->get_parameter(name_ + ".max_planning_time", max_planning_time_);
    node->get_parameter(name_ + ".robot_radius", robot_radius_);
    node->get_parameter(name_ + ".visibility_range", visibility_range_);
    node->get_parameter(name_ + ".obstacle_threshold", obstacle_threshold_);
    node->get_parameter(name_ + ".use_pointcloud", use_pointcloud_);
    node->get_parameter(name_ + ".pointcloud_topic", pointcloud_topic_);
    
    frame_id_ = costmap_ros->getGlobalFrameID();
    
    planner_ = std::make_unique<FARPlanner>();
    planner_->setMaxPlanningTime(max_planning_time_);
    planner_->setRobotRadius(robot_radius_);
    planner_->setVisibilityRange(visibility_range_);
    planner_->setObstacleThreshold(obstacle_threshold_);
}

nav_msgs::msg::Path FARPlannerGlobalPlanner::createPlan(
    const geometry_msgs::msg::PoseStamped& start,
    const geometry_msgs::msg::PoseStamped& goal)
{
    nav_msgs::msg::Path path;
    path.header.stamp = rclcpp::Time(0);
    path.header.frame_id = frame_id_;
    
    if (!planner_) {
        return path;
    }
    
    planner_->setStart(start.pose);
    planner_->setGoal(goal.pose);
    planner_->makePlan(path);
    
    return path;
}

void FARPlannerGlobalPlanner::cleanup() {}
void FARPlannerGlobalPlanner::activate() {}
void FARPlannerGlobalPlanner::deactivate() {}

}  // namespace far_planner_ros2

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(far_planner_ros2::FARPlannerGlobalPlanner, nav2_core::GlobalPlanner)
