#include "pid_position_follow.h"

    RobotCtrl::RobotCtrl() : Node("robot_ctrl")
    {

        max_x_speed_ = this->get_parameter_or("max_x_speed", 1.2);
        max_y_speed_ = this->get_parameter_or("max_y_speed", 1.2);
        set_yaw_speed_ = this->get_parameter_or("set_yaw_speed", 2.0);
        p_value_ = this->get_parameter_or("p_value", 1.0);
        i_value_ = this->get_parameter_or("i_value", 1.0);
        d_value_ = this->get_parameter_or("d_value", 1.0);
        plan_freq_ = this->get_parameter_or("plan_frequency", 30);
        goal_dist_tolerance_ = this->get_parameter_or("goal_dist_tolerance", 0.2);
        prune_ahead_dist_ = this->get_parameter_or("prune_ahead_distance", 0.3);
        global_frame_ = this->get_parameter_or("global_frame", std::string("map"));

        // Create publishers and subscribers
        local_path_pub_ = this->create_publisher<nav_msgs::msg::Path>("local_path", 5);
        global_path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
    "/plan", 5, std::bind(&RobotCtrl::GlobalPathCallback, this, std::placeholders::_1));
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/base_vel", 10);

        // Initialize TF listener
        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // Create timer
        plan_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000 / plan_freq_), std::bind(&RobotCtrl::Plan, this));
    }

    void RobotCtrl::Plan()
    {
        if (planner_state_ == 2)
        {
            if (plan_)
            {
                this->get_parameter("tower_flag_off", tower_off);
                RCLCPP_INFO(this->get_logger(), "tower_off=%d", tower_off);
                auto begin = std::chrono::steady_clock::now();
                auto start = this->now();
                // 1. Update the transform from global path frame to local planner frame
                UpdateTransform( tf_buffer_ , global_frame_, global_path_.header.frame_id, global_path_.header.stamp, global2path_transform_);
                RCLCPP_INFO_STREAM(this->get_logger(), "Time elapsed: " << (this->now() - start).seconds());

                // 2. Get current robot pose in global path frame
                geometry_msgs::msg::PoseStamped robot_pose;
                GetGlobalRobotPose( tf_buffer_ , global_path_.header.frame_id, robot_pose);

                // 3. Check if robot has already arrived with given distance tolerance
                if (GetEuclideanDistance(robot_pose, global_path_.poses.back()) <= goal_dist_tolerance_ || prune_index_ == global_path_.poses.size() - 1)
                {
                    plan_ = false;
                    geometry_msgs::msg::Twist cmd_vel;
                    cmd_vel.linear.x = 0;
                    cmd_vel.linear.y = 0;
                    if (tower_off == 0)
                    {
                        cmd_vel.angular.z = 0;
                    }
                    else
                    {
                        cmd_vel.angular.z = set_yaw_speed_;
                    }
                    cmd_vel.linear.z = 1;   // bool success or not
                    cmd_vel_pub_->publish(cmd_vel);
                    RCLCPP_INFO(this->get_logger(), "Planning Success!");
                    return;
                }

                // 4. Get prune index from given global path
                FindNearstPose(robot_pose, global_path_, prune_index_, prune_ahead_dist_);

                // 5. Generate the prune path and transform it into local planner frame
                nav_msgs::msg::Path prune_path, local_path;

                local_path.header.frame_id = global_frame_;
                prune_path.header.frame_id = global_frame_;

                geometry_msgs::msg::PoseStamped tmp_pose;
                tmp_pose.header.frame_id = global_frame_;

                TransformPose(global2path_transform_, robot_pose, tmp_pose);
                prune_path.poses.push_back(tmp_pose);

                int i = prune_index_;

                while (i < global_path_.poses.size() && i - prune_index_ < 20)
                {
                    TransformPose(global2path_transform_, global_path_.poses[i], tmp_pose);
                    prune_path.poses.push_back(tmp_pose);
                    i++;
                }

                // 6. Generate the cubic spline trajectory from above prune path
                GenTraj(prune_path, local_path);
                local_path_pub_->publish(local_path);

                // 7. Follow the trajectory and calculate the velocity
                geometry_msgs::msg::Twist cmd_vel;
                FollowTraj(robot_pose, local_path, cmd_vel);
                cmd_vel_pub_->publish(cmd_vel);

                auto plan_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin);
                RCLCPP_INFO(this->get_logger(), "Planning takes %f ms and passed %d/%d.", plan_time.count() / 1000., prune_index_, static_cast<int>(global_path_.poses.size()));
            }
            else
            {
                geometry_msgs::msg::Twist cmd_vel;
                cmd_vel.linear.x = 0;
                cmd_vel.linear.y = 0;
                if (tower_off == 0)
                {
                    cmd_vel.angular.z = 0;
                }
                else
                {
                    cmd_vel.angular.z = set_yaw_speed_;
                }
                cmd_vel.linear.z = 0;   // bool success or not
                cmd_vel_pub_->publish(cmd_vel);
            }
        }
        else if (planner_state_ == 1)
        {
            geometry_msgs::msg::Twist cmd_vel;
            cmd_vel.linear.x = 0;
            cmd_vel.linear.y = 0;
            cmd_vel.angular.z = set_yaw_speed_;
            cmd_vel.linear.z = 0;   // bool success or not
            cmd_vel_pub_->publish(cmd_vel);
        }
        else
        {
            geometry_msgs::msg::Twist cmd_vel;
            cmd_vel.linear.x = 0;
            cmd_vel.linear.y = 0;
            cmd_vel.angular.z = 0;
            cmd_vel.linear.z = 0;   // bool success or not
            cmd_vel_pub_->publish(cmd_vel);
        }
    }

    void RobotCtrl::FindNearstPose(geometry_msgs::msg::PoseStamped &robot_pose, nav_msgs::msg::Path &path, int &prune_index, double prune_ahead_dist)
    {
        double dist_threshold = 10; // threshold is 10 meters (basically never over 10m i suppose)
        double sq_dist_threshold = dist_threshold * dist_threshold;
        double sq_dist;
        if (prune_index != 0)
        {
            sq_dist = GetEuclideanDistance(robot_pose, path.poses[prune_index - 1]);
        }
        else
        {
            sq_dist = 1e10;
        }

        double new_sq_dist = 0;
        while (prune_index < static_cast<int>(path.poses.size()))
        {
            new_sq_dist = GetEuclideanDistance(robot_pose, path.poses[prune_index]);
            if (new_sq_dist > sq_dist && sq_dist < sq_dist_threshold)
            {
                // Judge if it is in the same direction and sq_dist is further than 0.3 meters
                if ((path.poses[prune_index].pose.position.x - robot_pose.pose.position.x) *
                    (path.poses[prune_index - 1].pose.position.x - robot_pose.pose.position.x) +
                    (path.poses[prune_index].pose.position.y - robot_pose.pose.position.y) *
                    (path.poses[prune_index - 1].pose.position.y - robot_pose.pose.position.y) > 0 &&
                    sq_dist > prune_ahead_dist)
                {
                    prune_index--;
                }
                else
                {
                    sq_dist = new_sq_dist;
                }
                break;
            }
            sq_dist = new_sq_dist;
            ++prune_index;
        }

        prune_index = std::min(prune_index, static_cast<int>(path.poses.size() - 1));
    }

    void RobotCtrl::FollowTraj(const geometry_msgs::msg::PoseStamped &robot_pose, const nav_msgs::msg::Path &traj, geometry_msgs::msg::Twist &cmd_vel)
    {
        geometry_msgs::msg::PoseStamped robot_pose_1;
        GetGlobalRobotPose( tf_buffer_ , global_path_.header.frame_id, robot_pose_1);

        yaw_ = tf2::getYaw(robot_pose_1.pose.orientation);

        double diff_yaw = atan2((traj.poses[1].pose.position.y - robot_pose.pose.position.y), (traj.poses[1].pose.position.x - robot_pose.pose.position.x));

        double diff_distance = GetEuclideanDistance(robot_pose, traj.poses[1]);

        // Normalize yaw difference to [-PI, PI]
        if (diff_yaw > M_PI)
        {
            diff_yaw -= 2 * M_PI;
        }
        else if (diff_yaw < -M_PI)
        {
            diff_yaw += 2 * M_PI;
        }

        RCLCPP_INFO_STREAM(this->get_logger(), "diff_yaw: " << diff_yaw);
        RCLCPP_INFO_STREAM(this->get_logger(), "diff_distance: " << diff_distance);

        double vx_global = max_x_speed_ * cos(diff_yaw) * p_value_;
        double vy_global = max_y_speed_ * sin(diff_yaw) * p_value_;

        cmd_vel.linear.x = vx_global * cos(yaw_) + vy_global * sin(yaw_);
        cmd_vel.linear.y = -vx_global * sin(yaw_) + vy_global * cos(yaw_);

        if (tower_off == 1)
        {
            cmd_vel.angular.z = set_yaw_speed_;
        }
        else if (tower_off == 0)
        {
            cmd_vel.angular.z = 0;
        }
    }

    void RobotCtrl::GlobalPathCallback(const nav_msgs::msg::Path::SharedPtr msg)
    {
        if (!msg->poses.empty())
        {
            global_path_ = *msg;
            prune_index_ = 0;
            plan_ = true;
        }
    }

    // void RobotCtrl::ImuCallback(const sensor_msgs::msg::Imu::SharedPtr msg)
    // {
    //     // yaw_ = tf2::getYaw(msg->orientation);
    //     // RCLCPP_INFO_STREAM(this->get_logger(), "imu_yaw: " << yaw_);
    // }


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto robotctrl = std::make_shared<RobotCtrl>();
    rclcpp::spin(robotctrl);
    rclcpp::shutdown();
    return 0;
}
