#pragma once

#include <rclcpp/rclcpp.hpp>
#include <nav2_core/global_planner.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <queue>
#include <limits>

namespace far_planner_ros2
{

struct Point2D {
    double x, y;
    Point2D(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    bool operator==(const Point2D& other) const {
        return std::abs(x - other.x) < 1e-6 && std::abs(y - other.y) < 1e-6;
    }
};

struct Polygon {
    std::vector<Point2D> vertices;
    bool is_obstacle;
    
    Polygon() : is_obstacle(true) {}
};

struct VisibilityEdge {
    Point2D start;
    Point2D end;
    double length;
    
    VisibilityEdge(const Point2D& s, const Point2D& e) 
        : start(s), end(e), length(std::sqrt(std::pow(e.x - s.x, 2) + std::pow(e.y - s.y, 2))) {}
};

class VisibilityGraph
{
public:
    VisibilityGraph();
    std::vector<Point2D> nodes;
    std::vector<VisibilityEdge> edges;
    std::vector<Polygon> obstacles;
    
    void buildFromPolygons(const std::vector<Polygon>& polygons);
    bool isVisible(const Point2D& p1, const Point2D& p2) const;
    std::vector<Point2D> shortestPath(const Point2D& start, const Point2D& goal) const;
    void clear();
    
private:
    bool lineIntersectsPolygon(const Point2D& p1, const Point2D& p2, const Polygon& poly) const;
    bool segmentsIntersect(const Point2D& p1, const Point2D& p2, const Point2D& p3, const Point2D& p4) const;
    bool pointInPolygon(const Point2D& point, const Polygon& poly) const;
    double pointToLineDistance(const Point2D& point, const Point2D& line_start, const Point2D& line_end) const;
};

class FARPlanner
{
public:
    FARPlanner();
    ~FARPlanner() = default;
    
    void initialize(const rclcpp::Node::SharedPtr& node);
    void setObstacles(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg);
    void setStart(const geometry_msgs::msg::Pose& pose);
    void setGoal(const geometry_msgs::msg::Pose& pose);
    bool makePlan(nav_msgs::msg::Path& path);
    
    void setMaxPlanningTime(double time) { max_planning_time_ = time; }
    void setRobotRadius(double radius) { robot_radius_ = radius; }
    void setVisibilityRange(double range) { visibility_range_ = range; }
    void setObstacleThreshold(double threshold) { obstacle_threshold_ = threshold; }
    
private:
    rclcpp::Node::SharedPtr node_;
    
    Point2D current_pos_;
    Point2D goal_pos_;
    double current_theta_;
    
    VisibilityGraph vis_graph_;
    
    double max_planning_time_;
    double robot_radius_;
    double visibility_range_;
    double obstacle_threshold_;
    double map_resolution_;
    double map_origin_x_;
    double map_origin_y_;
    int map_width_;
    int map_height_;
    
    std::vector<Polygon> current_obstacles_;
    bool obstacles_updated_;
    
    void processPointCloud(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg);
    std::vector<Polygon> extractPolygons(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg);
    bool isInsideRobot(const Point2D& point) const;
    geometry_msgs::msg::PoseStamped poseToPoseStamped(const geometry_msgs::msg::Pose& pose, const std::string& frame_id);
};

class FARPlannerGlobalPlanner : public nav2_core::GlobalPlanner
{
public:
    FARPlannerGlobalPlanner() = default;
    ~FARPlannerGlobalPlanner() = default;
    
    void configure(
        const rclcpp_lifecycle::LifecycleNode::WeakPtr& parent,
        std::string name,
        std::shared_ptr<tf2_ros::Buffer> tf,
        std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros) override;
    
    nav_msgs::msg::Path createPlan(
        const geometry_msgs::msg::PoseStamped& start,
        const geometry_msgs::msg::PoseStamped& goal) override;
    
    void cleanup() override;
    void activate() override;
    void deactivate() override;
    
private:
    std::shared_ptr<tf2_ros::Buffer> tf_;
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_;
    
    std::unique_ptr<FARPlanner> planner_;
    
    std::string name_;
    std::string frame_id_;
    double tolerance_;
    double max_planning_time_;
    double robot_radius_;
    double visibility_range_;
    double obstacle_threshold_;
    bool allow_unknown_;
    bool use_pointcloud_;
    std::string pointcloud_topic_;
    
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pointcloud_sub_;
    
    void pointcloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg);
};

}  // namespace far_planner_ros2
