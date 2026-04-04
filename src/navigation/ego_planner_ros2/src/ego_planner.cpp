#include "ego_planner_ros2/ego_planner.h"
#include <cmath>
#include <algorithm>

namespace ego_planner_ros2
{

GridMap::GridMap()
    : resolution_(0.1)
    , x_size_(60.0)
    , y_size_(60.0)
    , origin_x_(-30.0)
    , origin_y_(-30.0)
    , grid_width_(600)
    , grid_height_(600)
    , inflation_radius_(0.5)
{
}

void GridMap::initialize(const rclcpp::Node::SharedPtr& node)
{
    node_ = node;
    
    node_->declare_parameter("grid_map.resolution", 0.1);
    node_->declare_parameter("grid_map.x_size", 60.0);
    node_->declare_parameter("grid_map.y_size", 60.0);
    node_->declare_parameter("grid_map.inflation_radius", 0.5);
    
    node_->get_parameter("grid_map.resolution", resolution_);
    node_->get_parameter("grid_map.x_size", x_size_);
    node_->get_parameter("grid_map.y_size", y_size_);
    node_->get_parameter("grid_map.inflation_radius", inflation_radius_);
    
    grid_width_ = static_cast<int>(x_size_ / resolution_);
    grid_height_ = static_cast<int>(y_size_ / resolution_);
    
    grid_data_.resize(grid_width_ * grid_height_, 0);
    inflated_data_.resize(grid_width_ * grid_height_, 0);
    
    origin_x_ = -x_size_ / 2.0;
    origin_y_ = -y_size_ / 2.0;
    
    RCLCPP_INFO(node_->get_logger(), "GridMap initialized: %d x %d", grid_width_, grid_height_);
}

void GridMap::updateFromPointCloud(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg)
{
    clear();
    
    const int point_step = cloud_msg->point_step;
    const size_t data_size = cloud_msg->data.size();
    
    int x_idx = -1, y_idx = -1, z_idx = -1;
    for (size_t i = 0; i < cloud_msg->fields.size(); i++) {
        if (cloud_msg->fields[i].name == "x") x_idx = i;
        if (cloud_msg->fields[i].name == "y") y_idx = i;
        if (cloud_msg->fields[i].name == "z") z_idx = i;
    }
    
    for (size_t i = 0; i < data_size; i += point_step) {
        float x, y, z;
        memcpy(&x, &cloud_msg->data[i + cloud_msg->fields[x_idx].offset], sizeof(float));
        memcpy(&y, &cloud_msg->data[i + cloud_msg->fields[y_idx].offset], sizeof(float));
        memcpy(&z, &cloud_msg->data[i + cloud_msg->fields[z_idx].offset], sizeof(float));
        
        if (z < 0.1 || z > 2.0) continue;
        
        int gx = static_cast<int>((x - origin_x_) / resolution_);
        int gy = static_cast<int>((y - origin_y_) / resolution_);
        
        if (gx >= 0 && gx < grid_width_ && gy >= 0 && gy < grid_height_) {
            grid_data_[gy * grid_width_ + gx] = 1;
        }
    }
    
    inflateGrid();
}

bool GridMap::isOccupied(double x, double y) const
{
    int gx = static_cast<int>((x - origin_x_) / resolution_);
    int gy = static_cast<int>((y - origin_y_) / resolution_);
    
    if (gx >= 0 && gx < grid_width_ && gy >= 0 && gy < grid_height_) {
        return grid_data_[gy * grid_width_ + gx] == 1;
    }
    return false;
}

bool GridMap::isInInflatedOccupied(double x, double y) const
{
    int gx = static_cast<int>((x - origin_x_) / resolution_);
    int gy = static_cast<int>((y - origin_y_) / resolution_);
    
    if (gx >= 0 && gx < grid_width_ && gy >= 0 && gy < grid_height_) {
        return inflated_data_[gy * grid_width_ + gx] == 1;
    }
    return false;
}

void GridMap::clear()
{
    std::fill(grid_data_.begin(), grid_data_.end(), 0);
    std::fill(inflated_data_.begin(), inflated_data_.end(), 0);
}

int GridMap::posToGridIndex(double x, double y) const
{
    int gx = static_cast<int>((x - origin_x_) / resolution_);
    int gy = static_cast<int>((y - origin_y_) / resolution_);
    return gy * grid_width_ + gx;
}

void GridMap::inflateGrid()
{
    int inflation_cells = static_cast<int>(inflation_radius_ / resolution_);
    
    for (int gy = 0; gy < grid_height_; gy++) {
        for (int gx = 0; gx < grid_width_; gx++) {
            if (grid_data_[gy * grid_width_ + gx] == 1) {
                for (int oy = -inflation_cells; oy <= inflation_cells; oy++) {
                    for (int ox = -inflation_cells; ox <= inflation_cells; ox++) {
                        int nx = gx + ox;
                        int ny = gy + oy;
                        if (nx >= 0 && nx < grid_width_ && ny >= 0 && ny < grid_height_) {
                            double dist = std::sqrt(ox * ox + oy * oy) * resolution_;
                            if (dist <= inflation_radius_) {
                                inflated_data_[ny * grid_width_ + nx] = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

BsplineOptimizer::BsplineOptimizer()
    : max_vel_(1.0)
    , max_acc_(1.0)
    , max_jerk_(1.0)
    , bspline_order_(3)
{
}

void BsplineOptimizer::setParameters(double max_vel, double max_acc, double max_jerk)
{
    max_vel_ = max_vel;
    max_acc_ = max_acc;
    max_jerk_ = max_jerk;
}

bool BsplineOptimizer::checkCollision(
    const Eigen::Vector3d& p1, 
    const Eigen::Vector3d& p2, 
    const GridMap& grid_map)
{
    double dist = (p2 - p1).norm();
    int steps = static_cast<int>(dist / 0.05) + 1;
    
    for (int i = 0; i <= steps; i++) {
        double t = static_cast<double>(i) / steps;
        double x = p1.x() + t * (p2.x() - p1.x());
        double y = p1.y() + t * (p2.y() - p1.y());
        
        if (grid_map.isInInflatedOccupied(x, y)) {
            return true;
        }
    }
    return false;
}

std::vector<Eigen::Vector3d> BsplineOptimizer::optimize(
    const std::vector<Eigen::Vector3d>& init_path,
    const GridMap& grid_map,
    double start_yaw,
    double goal_yaw)
{
    if (init_path.empty()) return {};
    
    std::vector<Eigen::Vector3d> optimized_path = init_path;
    
    const int max_iterations = 10;
    for (int iter = 0; iter < max_iterations; iter++) {
        bool has_collision = false;
        
        for (size_t i = 1; i < optimized_path.size(); i++) {
            if (checkCollision(optimized_path[i-1], optimized_path[i], grid_map)) {
                has_collision = true;
                
                Eigen::Vector3d mid = (optimized_path[i-1] + optimized_path[i]) / 2.0;
                
                double offset_range = 0.5;
                double best_dist = std::numeric_limits<double>::max();
                Eigen::Vector3d best_pos = optimized_path[i];
                
                for (double ox = -offset_range; ox <= offset_range; ox += 0.1) {
                    for (double oy = -offset_range; oy <= offset_range; oy += 0.1) {
                        Eigen::Vector3d test_pos = mid + Eigen::Vector3d(ox, oy, 0);
                        if (!grid_map.isInInflatedOccupied(test_pos.x(), test_pos.y())) {
                            double dist = (test_pos - optimized_path[i-1]).norm() + 
                                        (test_pos - optimized_path[i]).norm();
                            if (dist < best_dist) {
                                best_dist = dist;
                                best_pos = test_pos;
                            }
                        }
                    }
                }
                
                optimized_path.insert(optimized_path.begin() + i, best_pos);
            }
        }
        
        if (!has_collision) break;
    }
    
    return optimized_path;
}

EgoPlanner::EgoPlanner()
    : max_vel_(1.0)
    , max_acc_(1.0)
    , bspline_order_(3)
    , control_points_interval_(0.4)
    , optimization_time_(0.5)
    , map_received_(false)
    , goal_received_(false)
{
}

void EgoPlanner::initialize(const rclcpp::Node::SharedPtr& node)
{
    node_ = node;
    
    node_->declare_parameter("max_vel", 1.0);
    node_->declare_parameter("max_acc", 1.0);
    node_->declare_parameter("bspline_order", 3);
    node_->declare_parameter("control_points_interval", 0.4);
    
    node_->get_parameter("max_vel", max_vel_);
    node_->get_parameter("max_acc", max_acc_);
    node_->get_parameter("bspline_order", bspline_order_);
    node_->get_parameter("control_points_interval", control_points_interval_);
    
    grid_map_.initialize(node_);
    optimizer_.setParameters(max_vel_, max_acc_, 1.0);
    
    RCLCPP_INFO(node_->get_logger(), "EgoPlanner initialized");
}

void EgoPlanner::setMap(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg)
{
    grid_map_.updateFromPointCloud(cloud_msg);
    map_received_ = true;
}

void EgoPlanner::setStart(const geometry_msgs::msg::Pose& pose)
{
    current_pos_ = Eigen::Vector3d(pose.position.x, pose.position.y, pose.position.z);
    current_yaw_ = 0.0;
}

void EgoPlanner::setGoal(const geometry_msgs::msg::Pose& pose)
{
    goal_pos_ = Eigen::Vector3d(pose.position.x, pose.position.y, pose.position.z);
    goal_yaw_ = 0.0;
    goal_received_ = true;
}

void EgoPlanner::setGlobalPath(const nav_msgs::msg::Path::SharedPtr& path_msg)
{
    global_path_.clear();
    for (const auto& pose : path_msg->poses) {
        global_path_.push_back(Eigen::Vector3d(
            pose.pose.position.x,
            pose.pose.position.y,
            pose.pose.position.z
        ));
    }
}

bool EgoPlanner::checkCollision()
{
    if (!map_received_ || global_path_.empty()) return false;
    
    for (size_t i = 1; i < global_path_.size(); i++) {
        if (optimizer_.checkCollision(global_path_[i-1], global_path_[i], grid_map_)) {
            return true;
        }
    }
    return false;
}

bool EgoPlanner::makePlan(nav_msgs::msg::Path& path)
{
    path.header.stamp = node_->now();
    path.header.frame_id = "map";
    path.poses.clear();
    
    if (!map_received_) {
        RCLCPP_WARN_ONCE(node_->get_logger(), "Map not received yet");
        return false;
    }
    
    if (!goal_received_) {
        RCLCPP_WARN_ONCE(node_->get_logger(), "Goal not received yet");
        return false;
    }
    
    std::vector<Eigen::Vector3d> path_points;
    path_points.push_back(current_pos_);
    
    if (!global_path_.empty()) {
        for (size_t i = 1; i < global_path_.size(); i++) {
            path_points.push_back(global_path_[i]);
        }
    } else {
        path_points.push_back(goal_pos_);
    }
    
    std::vector<Eigen::Vector3d> optimized_path = optimizer_.optimize(
        path_points, grid_map_, current_yaw_, goal_yaw_);
    
    if (optimized_path.empty()) {
        RCLCPP_WARN(node_->get_logger(), "Optimization failed");
        
        for (const auto& pt : path_points) {
            geometry_msgs::msg::PoseStamped pose;
            pose.header.stamp = node_->now();
            pose.header.frame_id = "map";
            pose.pose.position.x = pt.x();
            pose.pose.position.y = pt.y();
            pose.pose.position.z = pt.z();
            pose.pose.orientation.w = 1.0;
            path.poses.push_back(pose);
        }
        return true;
    }
    
    for (const auto& pt : optimized_path) {
        geometry_msgs::msg::PoseStamped pose;
        pose.header.stamp = node_->now();
        pose.header.frame_id = "map";
        pose.pose.position.x = pt.x();
        pose.pose.position.y = pt.y();
        pose.pose.position.z = pt.z();
        pose.pose.orientation.w = 1.0;
        path.poses.push_back(pose);
    }
    
    RCLCPP_INFO(node_->get_logger(), "EgoPlanner: generated path with %zu points", path.poses.size());
    return true;
}

}  // namespace ego_planner_ros2
