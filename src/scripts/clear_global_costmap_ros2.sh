#!/bin/bash
# 必须加载的ROS2环境（示例为Humble版本）
source /opt/ros/humble/setup.bash
source ~/rm_2025/install/setup.bash

# Nav2服务调用命令（需确认服务名称)
ros2 service call /red_standard_robot1/global_costmap/clear_entirely_global_costmap nav2_msgs/srv/ClearEntireCostmap "{}" 

