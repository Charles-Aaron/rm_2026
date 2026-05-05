#pragma once

#include "behaviortree_cpp_v3/bt_factory.h"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rclcpp/rclcpp.hpp"
//#include "tf2/LinearMath/Quaternion.h"
//#include "tf2_geometry_msgs/tf2_geometry_msgs.h"

#include <atomic>
#include <vector>

namespace rm_sentry_decision {

class CalculateAttackPose : public BT::SyncActionNode
{
public:
    CalculateAttackPose(const std::string &name, const BT::NodeConfiguration &config);

    static BT::PortsList providedPorts() {
        return {
            BT::OutputPort<std::string>("goal_pose")
        };
    }

    BT::NodeStatus tick() override;

private:
    // 回调订阅的敌人位置
    void gameStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg);

    // 候选点生成与筛选
    std::vector<geometry_msgs::msg::Point> generateCandidates(double ex, double ey);
    bool isFeasible(const geometry_msgs::msg::Point &p);
    geometry_msgs::msg::PoseStamped buildPose(const geometry_msgs::msg::Point &p, double ex, double ey);

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_sub_;
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr costmap_sub_;
    nav_msgs::msg::OccupancyGrid::SharedPtr costmap_;

    std::atomic<double> enemy_x_{0.0};
    std::atomic<double> enemy_y_{0.0};

    // 固定参数
    const int num_sectors_ = 20;
    const double attack_radius_ = 2.0;
    const int cost_threshold_ = 100;
    std::string poseToString(const geometry_msgs::msg::PoseStamped &pose);
};

} // namespace rm_sentry_decision
