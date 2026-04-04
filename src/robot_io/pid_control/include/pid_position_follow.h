#ifndef PID_POSITION_FOLLOW_H
#define PID_POSITION_FOLLOW_H

#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/path.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2/utils.h>
#include <Eigen/Eigen>
#include <chrono>

#include "utility.h" // 假设utility.h已经更新为ROS 2版本
#include "cubic_spline/cubic_spline_ros.h" // 假设cubic_spline_ros已经更新为ROS 2版本

class RobotCtrl : public rclcpp::Node {
public:
    RobotCtrl() ;
    ~RobotCtrl() = default;

    void ImuCallback(const sensor_msgs::msg::Imu &msg);

    void GlobalPathCallback(const nav_msgs::msg::Path::SharedPtr msg);

    void FollowTraj(const geometry_msgs::msg::PoseStamped& robot_pose,
                    const nav_msgs::msg::Path& traj,
                    geometry_msgs::msg::Twist& cmd_vel);

    void FindNearstPose(geometry_msgs::msg::PoseStamped& robot_pose,
                        nav_msgs::msg::Path& path,
                        int& prune_index,
                        double prune_ahead_dist);

    void Plan();

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr local_path_pub_;

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr global_path_sub_;
    rclcpp::TimerBase::SharedPtr plan_timer_;

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    nav_msgs::msg::Path global_path_;

    bool plan_ = false;
    int prune_index_ = 0;

    double max_x_speed_;
    double max_y_speed_;

    double set_yaw_speed_;

    double p_value_;
    double i_value_;
    double d_value_;

    int plan_freq_;
    double goal_dist_tolerance_;
    double prune_ahead_dist_;

    std::string global_frame_;

    double a_gimbal_yaw_position;
    double a_gimbal_pitch_position;

    double cur_gimbal_yaw_position;
    double cur_gimbal_pitch_position;

    double yaw_;  // 机器人航向角

    uint8_t game_state_ = 4;   // 可用做控制比赛是否开始
    int planner_state_ = 2;   // 规划状态 0：静止  1：原地小陀螺  2：路径跟踪
    int tower_off;
    tf2::Stamped<tf2::Transform> global2path_transform_; // 确保变量被声明
};


// 弧度制归一化
double normalizeRadian(const double angle) {
    double n_angle = std::fmod(angle, 2 * M_PI);
    n_angle = n_angle > M_PI ? n_angle - 2 * M_PI : n_angle < -M_PI ? 2 * M_PI + n_angle : n_angle;
    return n_angle;
}

// 限制值的范围
double ABS_limit(double value, double limit) {
    if (value < limit && value > -limit) {
        return 0;
    } else {
        return value;
    }
}

#endif // PID_POSITION_FOLLOW_H