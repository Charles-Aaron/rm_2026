#pragma once

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <string>

namespace ego_planner_ros2
{

struct Point {
    double x, y, z;
    Point(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}
};

struct TrajectoryPoint {
    Eigen::Vector3d position;
    Eigen::Vector3d velocity;
    double yaw;
    double time_from_start;
};

class GridMap
{
public:
    GridMap();
    ~GridMap() = default;
    
    void initialize(const rclcpp::Node::SharedPtr& node);
    void updateFromPointCloud(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg);
    bool isOccupied(double x, double y) const;
    bool isInInflatedOccupied(double x, double y) const;
    void clear();
    
private:
    int posToGridIndex(double x, double y) const;
    void inflateGrid();
    
    rclcpp::Node::SharedPtr node_;
    double resolution_;
    double x_size_, y_size_;
    double origin_x_, origin_y_;
    std::vector<uint8_t> grid_data_;
    std::vector<uint8_t> inflated_data_;
    int grid_width_, grid_height_;
    double inflation_radius_;
};

class BsplineOptimizer
{
public:
    BsplineOptimizer();
    ~BsplineOptimizer() = default;
    
    void setParameters(double max_vel, double max_acc, double max_jerk);
    std::vector<Eigen::Vector3d> optimize(
        const std::vector<Eigen::Vector3d>& init_path,
        const GridMap& grid_map,
        double start_yaw,
        double goal_yaw);
    bool checkCollision(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const GridMap& grid_map);
        
private:
    double max_vel_;
    double max_acc_;
    double max_jerk_;
    int bspline_order_;
};

class EgoPlanner
{
public:
    EgoPlanner();
    ~EgoPlanner() = default;
    
    void initialize(const rclcpp::Node::SharedPtr& node);
    void setMap(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg);
    void setStart(const geometry_msgs::msg::Pose& pose);
    void setGoal(const geometry_msgs::msg::Pose& pose);
    void setGlobalPath(const nav_msgs::msg::Path::SharedPtr& path_msg);
    bool makePlan(nav_msgs::msg::Path& path);
    bool checkCollision();
    
private:
    rclcpp::Node::SharedPtr node_;
    
    GridMap grid_map_;
    BsplineOptimizer optimizer_;
    
    Eigen::Vector3d current_pos_;
    double current_yaw_;
    Eigen::Vector3d goal_pos_;
    double goal_yaw_;
    
    std::vector<Eigen::Vector3d> global_path_;
    
    double max_vel_;
    double max_acc_;
    double bspline_order_;
    double control_points_interval_;
    double optimization_time_;
    
    bool map_received_;
    bool goal_received_;
};

}  // namespace ego_planner_ros2
