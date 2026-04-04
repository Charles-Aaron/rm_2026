#include "calculate_attack_pose.hpp"

namespace rm_sentry_decision {

CalculateAttackPose::CalculateAttackPose(const std::string &name, const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("CalculateAttackPose: Failed to get ROS node from blackboard");
    }

    // 订阅比赛状态获取敌人位置
    game_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        std::bind(&CalculateAttackPose::gameStatusCallback, this, std::placeholders::_1)
    );

    // 订阅全局代价地图
    costmap_sub_ = node_->create_subscription<nav_msgs::msg::OccupancyGrid>(
        "/global_costmap/costmap", 10,
        [this](const nav_msgs::msg::OccupancyGrid::SharedPtr msg) {
            costmap_ = msg;
        }
    );
}

void CalculateAttackPose::gameStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg)
{
    enemy_x_ = msg->enemy_x;
    enemy_y_ = msg->enemy_y;
}

BT::NodeStatus CalculateAttackPose::tick()
{
    double ex = enemy_x_.load();
    double ey = enemy_y_.load();

    if (ex == 0.0 && ey == 0.0) {
        return BT::NodeStatus::FAILURE;
    }

    if (!costmap_) {
        RCLCPP_WARN(node_->get_logger(), "Costmap not received yet");
        return BT::NodeStatus::FAILURE;
    }

    auto candidates = generateCandidates(ex, ey);

    for (auto &p : candidates) {
        if (isFeasible(p)) {
            auto pose = buildPose(p, ex, ey);
            setOutput("goal_pose",  poseToString(pose));
            return BT::NodeStatus::SUCCESS;
        }
    }

    return BT::NodeStatus::FAILURE;
}

std::vector<geometry_msgs::msg::Point> CalculateAttackPose::generateCandidates(double ex, double ey)
{
    std::vector<geometry_msgs::msg::Point> pts;

    for (int i = 0; i < num_sectors_; ++i) {
        double angle = i * 2 * M_PI / num_sectors_;

        geometry_msgs::msg::Point p;
        p.x = ex + attack_radius_ * cos(angle);
        p.y = ey + attack_radius_ * sin(angle);
        p.z = 0.0;

        pts.push_back(p);
    }

    return pts;
}

bool CalculateAttackPose::isFeasible(const geometry_msgs::msg::Point &p)
{
    const auto &info = costmap_->info;
    int cell_x = static_cast<int>((p.x - info.origin.position.x) / info.resolution);
    int cell_y = static_cast<int>((p.y - info.origin.position.y) / info.resolution);

    if (cell_x < 0 || cell_x >= static_cast<int>(info.width) ||
        cell_y < 0 || cell_y >= static_cast<int>(info.height)) {
        return false;
    }

    int index = cell_y * info.width + cell_x;
    int8_t cost = costmap_->data[index];
    return cost >= 0 && cost <= cost_threshold_;
}

geometry_msgs::msg::PoseStamped CalculateAttackPose::buildPose(
    const geometry_msgs::msg::Point &p, double /*ex*/, double /*ey*/)
{
    geometry_msgs::msg::PoseStamped pose;
    pose.header.frame_id = costmap_->header.frame_id;
    pose.header.stamp = node_->now();
    pose.pose.position = p;

    // 固定朝向，不计算 yaw
    pose.pose.orientation.x = 0.0;
    pose.pose.orientation.y = 0.0;
    pose.pose.orientation.z = 0.0;
    pose.pose.orientation.w = 1.0;

    return pose;
}

std::string CalculateAttackPose::poseToString(const geometry_msgs::msg::PoseStamped &pose)
{
    std::stringstream ss;
    ss << pose.pose.position.x << ";"
       << pose.pose.position.y << ";"
       << pose.pose.position.z << ";"
       << pose.pose.orientation.x << ";"
       << pose.pose.orientation.y << ";"
       << pose.pose.orientation.z << ";"
       << pose.pose.orientation.w;
    return ss.str();
}

} // namespace rm_sentry_decision

// 注册 BT 节点
BT_REGISTER_NODES(factory) {
    factory.registerNodeType<rm_sentry_decision::CalculateAttackPose>("CalculateAttackPose");
}
