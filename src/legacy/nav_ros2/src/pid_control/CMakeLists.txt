cmake_minimum_required(VERSION 3.8)
project(pid_control)
# 设置 C++ 标准，默认使用 C++14，这是 ROS 2 的最低要求
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
# 查找 ament 构建系统
find_package(ament_cmake REQUIRED)
# 查找 ROS 2 核心依赖项
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
# 添加可执行文件
add_executable(pid_control src/pid_control.cpp)
# 设置目标的头文件目录
target_include_directories(pid_control
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
# 设置目标的依赖项
ament_target_dependencies(pid_control
    rclcpp
    tf2
    tf2_ros
    tf2_geometry_msgs
    nav_msgs
    sensor_msgs
    geometry_msgs
    std_msgs
    Eigen3
)
# 安装可执行文件
install(TARGETS pid_control
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

# 测试配置（可选）
if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    # 自动查找并运行代码检查工具
    ament_lint_auto_find_test_dependencies()
endif()
# 结束 ament 项目配置
ament_package()