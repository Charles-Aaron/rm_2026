cmake_minimum_required(VERSION 3.8)
project(chassis_odom)

# 设置 C++17 标准（代码中使用了 std::optional）
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(customize_messages REQUIRED)

# 声明可执行文件
add_executable(send_goal
  src/chassis_odom.cpp
)


# 自动链接所有 ament 依赖项（推荐方式）
ament_target_dependencies(send_goal
  rclcpp
  nav_msgs
  geometry_msgs
  tf2_ros
  tf2_geometry_msgs
  std_msgs
  tf2
  rclcpp_action
  nav2_msgs 
  customize_messages 
)

# 安装可执行文件到 lib/<pkg_name>
install(TARGETS send_goal
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY 
    launch
    DESTINATION share/${PROJECT_NAME}
)

# 导出包配置
ament_package()
