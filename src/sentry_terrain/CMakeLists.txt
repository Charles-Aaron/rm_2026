cmake_minimum_required(VERSION 3.16)
project(sentry_terrain LANGUAGES C CXX)
find_package(ament_cmake REQUIRED)
foreach(dep rclcpp rclcpp_lifecycle nav2_costmap_2d nav2_util pluginlib
    localization_interfaces frame_broadcaster sensor_msgs nav_msgs std_msgs pcl_conversions)
  find_package(${dep} REQUIRED)
endforeach()
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED COMPONENTS common)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
add_library(bit_terrain_core SHARED
  vendor/bit_rog/src/prob_map.cpp vendor/bit_rog/src/inf_map.cpp
  vendor/bit_rog/src/esdf_map.cpp vendor/bit_rog/src/sliding_map.cpp
  vendor/bit_rog/src/counter_map.cpp vendor/bit_rog/src/raycaster.cpp
  vendor/bit_rog/src/projection_layer.cpp src/terrain_core.cpp)
target_compile_features(bit_terrain_core PUBLIC cxx_std_17)
target_compile_definitions(bit_terrain_core PUBLIC ORIGIN_AT_CORNER USE_ROS2 ROOT_DIR="")
target_include_directories(bit_terrain_core PUBLIC include)
target_include_directories(bit_terrain_core SYSTEM PUBLIC vendor/bit_rog/include ${PCL_INCLUDE_DIRS})
target_link_libraries(bit_terrain_core PUBLIC Eigen3::Eigen fmt::fmt ${PCL_LIBRARIES})
ament_target_dependencies(bit_terrain_core PUBLIC rclcpp nav2_util pcl_conversions)
add_library(sentry_terrain_layer SHARED src/terrain_layer.cpp)
target_compile_features(sentry_terrain_layer PUBLIC cxx_std_17)
target_compile_options(sentry_terrain_layer PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(sentry_terrain_layer bit_terrain_core Threads::Threads)
ament_target_dependencies(sentry_terrain_layer nav2_costmap_2d pluginlib
  localization_interfaces frame_broadcaster rclcpp_lifecycle nav_msgs std_msgs)
pluginlib_export_plugin_description_file(nav2_costmap_2d terrain_plugins.xml)
install(TARGETS bit_terrain_core sentry_terrain_layer LIBRARY DESTINATION lib)
install(DIRECTORY vendor/bit_rog DESTINATION share/${PROJECT_NAME}/vendor)

ament_package()
