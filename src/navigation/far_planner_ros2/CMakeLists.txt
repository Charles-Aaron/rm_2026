cmake_minimum_required(VERSION 3.8)
project(far_planner_ros2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclpy REQUIRED)
find_package(nav2_core REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(pluginlib REQUIRED)

# Include directories
include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
)

# C++ executables
add_executable(far_planner_node src/far_planner_node.cpp
                                   src/far_planner.cpp)

ament_target_dependencies(far_planner_node
  rclcpp
  rclcpp_action
  nav2_core
  nav2_msgs
  nav_msgs
  geometry_msgs
  sensor_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  visualization_msgs
  pcl_conversions
  Eigen3
)

# Nav2 plugin
add_library(far_planner_plugin SHARED src/far_planner_plugin.cpp src/far_planner.cpp)

ament_target_dependencies(far_planner_plugin
  rclcpp
  nav2_core
  nav2_msgs
  nav_msgs
  geometry_msgs
  sensor_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  Eigen3
  pluginlib
)

# Register plugin description to ament index for pluginlib discovery
pluginlib_export_plugin_description_file(nav2_core far_planner_plugin.xml)

# Install targets
install(TARGETS far_planner_node
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS far_planner_plugin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY
  launch
  param
  config
  DESTINATION share/${PROJECT_NAME}/
)

install(FILES far_planner_plugin.xml
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
