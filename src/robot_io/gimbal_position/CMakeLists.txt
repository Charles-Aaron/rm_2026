cmake_minimum_required(VERSION 3.8)
project(gimbal_position)

# 设置编译器警告（仅限GCC/Clang）
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找ROS 2核心依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)

# 添加可执行目标
add_executable(gimbal_position 
    src/gimbal_position.cpp
)

# 配置头文件包含路径
target_include_directories(gimbal_position PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖项（关键修复点）
ament_target_dependencies(gimbal_position
    rclcpp
    sensor_msgs
    std_msgs
    geometry_msgs
    tf2_ros
    tf2_geometry_msgs
    tf2
)

# 安装可执行文件
install(TARGETS gimbal_position
    DESTINATION lib/${PROJECT_NAME}
)

# 安装launch目录（确保launch文件可访问）
install(DIRECTORY 
    launch
    DESTINATION share/${PROJECT_NAME}
)

# 导出依赖必须在ament_package之前（核心修复）
ament_export_dependencies(
    rclcpp
    sensor_msgs
    std_msgs
    geometry_msgs
    tf2_ros
    tf2_geometry_msgs
    tf2
)

# 质量检查配置（仅在测试时启用）
if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    set(ament_cmake_copyright_FOUND TRUE)
    set(ament_cmake_cpplint_FOUND TRUE)
    ament_lint_auto_find_test_dependencies()
endif()

# 必须最后调用ament_package（顺序关键）
ament_package()


