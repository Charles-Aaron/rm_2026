#include "far_planner_ros2/far_planner.h"
#include <algorithm>
#include <queue>

namespace far_planner_ros2
{

VisibilityGraph::VisibilityGraph() {}

void VisibilityGraph::clear()
{
    nodes.clear();
    edges.clear();
    obstacles.clear();
}

void VisibilityGraph::buildFromPolygons(const std::vector<Polygon>& polygons)
{
    clear();
    obstacles = polygons;
    
    for (const auto& poly : polygons) {
        for (const auto& vertex : poly.vertices) {
            bool found = false;
            for (const auto& node : nodes) {
                if (std::abs(node.x - vertex.x) < 0.1 && std::abs(node.y - vertex.y) < 0.1) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                nodes.push_back(vertex);
            }
        }
    }
    
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = i + 1; j < nodes.size(); j++) {
            if (isVisible(nodes[i], nodes[j])) {
                edges.emplace_back(nodes[i], nodes[j]);
            }
        }
    }
}

bool VisibilityGraph::isVisible(const Point2D& p1, const Point2D& p2) const
{
    for (const auto& poly : obstacles) {
        if (lineIntersectsPolygon(p1, p2, poly)) {
            return false;
        }
    }
    return true;
}

bool VisibilityGraph::lineIntersectsPolygon(
    const Point2D& p1, const Point2D& p2, 
    const Polygon& poly) const
{
    const size_t n = poly.vertices.size();
    if (n < 3) return false;
    
    for (size_t i = 0; i < n; i++) {
        const Point2D& a = poly.vertices[i];
        const Point2D& b = poly.vertices[(i + 1) % n];
        
        if (segmentsIntersect(p1, p2, a, b)) {
            return true;
        }
    }
    
    if (pointInPolygon(p1, poly)) return true;
    if (pointInPolygon(p2, poly)) return true;
    
    return false;
}

bool VisibilityGraph::segmentsIntersect(
    const Point2D& p1, const Point2D& p2,
    const Point2D& p3, const Point2D& p4) const
{
    auto orientation = [](const Point2D& p, const Point2D& q, const Point2D& r) -> int {
        double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (std::abs(val) < 1e-10) return 0;
        return (val > 0) ? 1 : 2;
    };
    
    int o1 = orientation(p1, p2, p3);
    int o2 = orientation(p1, p2, p4);
    int o3 = orientation(p3, p4, p1);
    int o4 = orientation(p3, p4, p2);
    
    if (o1 != o2 && o3 != o4) return true;
    return false;
}

bool VisibilityGraph::pointInPolygon(const Point2D& point, const Polygon& poly) const
{
    bool inside = false;
    const size_t n = poly.vertices.size();
    
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        const Point2D& pi = poly.vertices[i];
        const Point2D& pj = poly.vertices[j];
        
        if (((pi.y > point.y) != (pj.y > point.y)) &&
            (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x)) {
            inside = !inside;
        }
    }
    
    return inside;
}

std::vector<Point2D> VisibilityGraph::shortestPath(
    const Point2D& start, 
    const Point2D& goal) const
{
    std::vector<Point2D> empty_path;
    
    std::vector<Point2D> all_nodes = nodes;
    bool start_found = false, goal_found = false;
    
    for (const auto& node : nodes) {
        if (std::abs(node.x - start.x) < 0.1 && std::abs(node.y - start.y) < 0.1)
            start_found = true;
        if (std::abs(node.x - goal.x) < 0.1 && std::abs(node.y - goal.y) < 0.1)
            goal_found = true;
    }
    
    if (!start_found) all_nodes.push_back(start);
    if (!goal_found) all_nodes.push_back(goal);
    
    std::vector<std::vector<std::pair<int, double>>> adj(all_nodes.size());
    
    for (size_t i = 0; i < all_nodes.size(); i++) {
        for (size_t j = i + 1; j < all_nodes.size(); j++) {
            if (i < nodes.size() && j < nodes.size()) {
                if (!isVisible(all_nodes[i], all_nodes[j])) continue;
            } else {
                bool visible = true;
                for (const auto& poly : obstacles) {
                    if (lineIntersectsPolygon(all_nodes[i], all_nodes[j], poly)) {
                        visible = false;
                        break;
                    }
                }
                if (!visible) continue;
            }
            
            double dist = std::sqrt(
                std::pow(all_nodes[j].x - all_nodes[i].x, 2) + 
                std::pow(all_nodes[j].y - all_nodes[i].y, 2)
            );
            adj[i].push_back({j, dist});
            adj[j].push_back({i, dist});
        }
    }
    
    int n = all_nodes.size();
    int start_idx = n - 2;
    int goal_idx = n - 1;
    
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<int> parent(n, -1);
    std::vector<bool> visited(n, false);
    
    dist[start_idx] = 0.0;
    
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0.0, start_idx});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (visited[u]) continue;
        visited[u] = true;
        
        if (u == goal_idx) break;
        
        for (const auto& [v, w] : adj[u]) {
            if (!visited[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    
    if (parent[goal_idx] == -1) return empty_path;
    
    std::vector<Point2D> path;
    int curr = goal_idx;
    while (curr != -1) {
        path.push_back(all_nodes[curr]);
        curr = parent[curr];
    }
    std::reverse(path.begin(), path.end());
    
    return path;
}

double VisibilityGraph::pointToLineDistance(
    const Point2D& point,
    const Point2D& line_start,
    const Point2D& line_end) const
{
    double A = point.x - line_start.x;
    double B = point.y - line_start.y;
    double C = line_end.x - line_start.x;
    double D = line_end.y - line_start.y;
    
    double dot = A * C + B * D;
    double len_sq = C * C + D * D;
    double param = -1.0;
    
    if (len_sq != 0.0) param = dot / len_sq;
    
    double xx, yy;
    
    if (param < 0.0) {
        xx = line_start.x;
        yy = line_start.y;
    } else if (param > 1.0) {
        xx = line_end.x;
        yy = line_end.y;
    } else {
        xx = line_start.x + param * C;
        yy = line_start.y + param * D;
    }
    
    return std::sqrt(std::pow(point.x - xx, 2) + std::pow(point.y - yy, 2));
}

// FAR Planner Implementation
FARPlanner::FARPlanner()
    : max_planning_time_(1.0)
    , robot_radius_(0.2)
    , visibility_range_(10.0)
    , obstacle_threshold_(0.5)
    , map_resolution_(0.05)
    , map_origin_x_(0.0)
    , map_origin_y_(0.0)
    , map_width_(200)
    , map_height_(200)
    , obstacles_updated_(false)
{
}

void FARPlanner::initialize(const rclcpp::Node::SharedPtr& node)
{
    node_ = node;
    
    node_->declare_parameter("max_planning_time", 1.0);
    node_->declare_parameter("robot_radius", 0.2);
    node_->declare_parameter("visibility_range", 10.0);
    node_->declare_parameter("obstacle_threshold", 0.5);
    node_->declare_parameter("map_resolution", 0.05);
    
    node_->get_parameter("max_planning_time", max_planning_time_);
    node_->get_parameter("robot_radius", robot_radius_);
    node_->get_parameter("visibility_range", visibility_range_);
    node_->get_parameter("obstacle_threshold", obstacle_threshold_);
    node_->get_parameter("map_resolution", map_resolution_);
}

void FARPlanner::setObstacles(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg)
{
    processPointCloud(cloud_msg);
    obstacles_updated_ = true;
}

void FARPlanner::setStart(const geometry_msgs::msg::Pose& pose)
{
    current_pos_.x = pose.position.x;
    current_pos_.y = pose.position.y;
    current_theta_ = 0.0;
}

void FARPlanner::setGoal(const geometry_msgs::msg::Pose& pose)
{
    goal_pos_.x = pose.position.x;
    goal_pos_.y = pose.position.y;
}

bool FARPlanner::makePlan(nav_msgs::msg::Path& path)
{
    path.header.stamp = node_->now();
    path.header.frame_id = "map";
    path.poses.clear();
    
    if (current_obstacles_.empty()) {
        geometry_msgs::msg::Pose start_pose;
        start_pose.position.x = current_pos_.x;
        start_pose.position.y = current_pos_.y;
        start_pose.position.z = 0.0;
        start_pose.orientation.w = 1.0;
        
        geometry_msgs::msg::Pose goal_pose;
        goal_pose.position.x = goal_pos_.x;
        goal_pose.position.y = goal_pos_.y;
        goal_pose.position.z = 0.0;
        goal_pose.orientation.w = 1.0;
        
        geometry_msgs::msg::PoseStamped start_pose_msg = poseToPoseStamped(start_pose, "map");
        geometry_msgs::msg::PoseStamped goal_pose_msg = poseToPoseStamped(goal_pose, "map");
        path.poses.push_back(start_pose_msg);
        path.poses.push_back(goal_pose_msg);
        return true;
    }
    
    vis_graph_.buildFromPolygons(current_obstacles_);
    
    vis_graph_.nodes.push_back(current_pos_);
    vis_graph_.nodes.push_back(goal_pos_);
    
    auto shortest_path = vis_graph_.shortestPath(current_pos_, goal_pos_);
    
    if (shortest_path.empty()) {
        RCLCPP_WARN(node_->get_logger(), "FAR Planner: No path found");
        return false;
    }
    
    for (const auto& pt : shortest_path) {
        geometry_msgs::msg::PoseStamped pose;
        pose.header.stamp = node_->now();
        pose.header.frame_id = "map";
        pose.pose.position.x = pt.x;
        pose.pose.position.y = pt.y;
        pose.pose.position.z = 0.0;
        pose.pose.orientation.w = 1.0;
        path.poses.push_back(pose);
    }
    
    obstacles_updated_ = false;
    return true;
}

void FARPlanner::processPointCloud(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg)
{
    current_obstacles_ = extractPolygons(cloud_msg);
}

std::vector<Polygon> FARPlanner::extractPolygons(const sensor_msgs::msg::PointCloud2::SharedPtr& cloud_msg)
{
    std::vector<Polygon> polygons;
    
    const int width = cloud_msg->width;
    const int height = cloud_msg->height;
    const int point_step = cloud_msg->point_step;
    const int data_size = cloud_msg->data.size();
    
    double resolution = visibility_range_ / 100.0;
    
    std::vector<std::vector<bool>> grid(map_width_, std::vector<bool>(map_height_, false));
    
    int x_idx = -1, y_idx = -1;
    for (int i = 0; i < cloud_msg->fields.size(); i++) {
        if (cloud_msg->fields[i].name == "x") x_idx = i;
        if (cloud_msg->fields[i].name == "y") y_idx = i;
    }
    
    if (x_idx < 0 || y_idx < 0) return polygons;
    
    for (size_t i = 0; i < data_size; i += point_step) {
        float x, y;
        memcpy(&x, &cloud_msg->data[i + cloud_msg->fields[x_idx].offset], sizeof(float));
        memcpy(&y, &cloud_msg->data[i + cloud_msg->fields[y_idx].offset], sizeof(float));
        
        double dist = std::sqrt(x * x + y * y);
        if (dist > visibility_range_ || dist < 0.1) continue;
        
        int gx = static_cast<int>((x + visibility_range_) / resolution);
        int gy = static_cast<int>((y + visibility_range_) / resolution);
        
        if (gx >= 0 && gx < 200 && gy >= 0 && gy < 200) {
            for (int ox = -2; ox <= 2; ox++) {
                for (int oy = -2; oy <= 2; oy++) {
                    int nx = gx + ox;
                    int ny = gy + oy;
                    if (nx >= 0 && nx < 200 && ny >= 0 && ny < 200) {
                        grid[nx][ny] = true;
                    }
                }
            }
        }
    }
    
    std::vector<std::vector<std::pair<int, int>>> blobs;
    std::vector<std::vector<bool>> visited(200, std::vector<bool>(200, false));
    
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            if (grid[i][j] && !visited[i][j]) {
                std::vector<std::pair<int, int>> blob;
                std::queue<std::pair<int, int>> q;
                q.push({i, j});
                visited[i][j] = true;
                
                while (!q.empty()) {
                    auto [cx, cy] = q.front();
                    q.pop();
                    blob.push_back({cx, cy});
                    
                    const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
                    const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
                    for (int k = 0; k < 8; k++) {
                        int nx = cx + dx[k];
                        int ny = cy + dy[k];
                        if (nx >= 0 && nx < 200 && ny >= 0 && ny < 200 &&
                            grid[nx][ny] && !visited[nx][ny]) {
                            visited[nx][ny] = true;
                            q.push({nx, ny});
                        }
                    }
                }
                
                if (blob.size() > 10) {
                    blobs.push_back(blob);
                }
            }
        }
    }
    
    for (const auto& blob : blobs) {
        if (blob.empty()) continue;
        
        Polygon poly;
        poly.is_obstacle = true;
        
        double min_x = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        double min_y = std::numeric_limits<double>::max();
        double max_y = std::numeric_limits<double>::lowest();
        
        for (const auto& [gx, gy] : blob) {
            double wx = (gx - 100) * resolution;
            double wy = (gy - 100) * resolution;
            min_x = std::min(min_x, wx);
            max_x = std::max(max_x, wx);
            min_y = std::min(min_y, wy);
            max_y = std::max(max_y, wy);
        }
        
        poly.vertices.push_back(Point2D(min_x, min_y));
        poly.vertices.push_back(Point2D(max_x, min_y));
        poly.vertices.push_back(Point2D(max_x, max_y));
        poly.vertices.push_back(Point2D(min_x, max_y));
        
        polygons.push_back(poly);
    }
    
    return polygons;
}

bool FARPlanner::isInsideRobot(const Point2D& point) const
{
    double dist = std::sqrt(
        std::pow(point.x - current_pos_.x, 2) + 
        std::pow(point.y - current_pos_.y, 2)
    );
    return dist < robot_radius_;
}

geometry_msgs::msg::PoseStamped FARPlanner::poseToPoseStamped(
    const geometry_msgs::msg::Pose& pose, 
    const std::string& frame_id)
{
    geometry_msgs::msg::PoseStamped ps;
    ps.header.stamp = node_->now();
    ps.header.frame_id = frame_id;
    ps.pose = pose;
    return ps;
}

}  // namespace far_planner_ros2
