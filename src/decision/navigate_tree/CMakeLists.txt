cmake_minimum_required(VERSION 3.5)
project(navigate_tree)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(customize_messages REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(unique_identifier_msgs REQUIRED)

set(NAVIGATE_TREE_COMMON_DEPENDENCIES
  customize_messages
  geometry_msgs
  nav2_msgs
  rcl_interfaces
  rclcpp
  rclcpp_action
  std_msgs
  tf2
  tf2_geometry_msgs
  tf2_ros
  unique_identifier_msgs
)

function(configure_navigate_tree_target target_name)
  target_include_directories(${target_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/navigate_tree
  )
  ament_target_dependencies(${target_name}
    ${NAVIGATE_TREE_COMMON_DEPENDENCIES}
  )
  target_link_libraries(${target_name}
    BT::behaviortree_cpp_v3
  )
endfunction()

add_library(condition_start src/condition_start.cpp)
configure_navigate_tree_target(condition_start)

add_library(condition_protect src/condition_protect.cpp)
configure_navigate_tree_target(condition_protect)

add_library(action_start35 src/action_start35.cpp)
configure_navigate_tree_target(action_start35)

add_library(condition_operator src/condition_operator.cpp)
configure_navigate_tree_target(condition_operator)

add_library(action_operator src/action_operator.cpp)
configure_navigate_tree_target(action_operator)

add_library(condition_enemy src/condition_enemy.cpp)
configure_navigate_tree_target(condition_enemy)

add_library(action_follow src/action_follow.cpp)
configure_navigate_tree_target(action_follow)

add_library(condition_blosupeng src/condition_blosupeng.cpp)
configure_navigate_tree_target(condition_blosupeng)

add_library(action_blosupeng src/action_blosupeng.cpp)
configure_navigate_tree_target(action_blosupeng)

add_library(condition_tower src/condition_tower.cpp)
configure_navigate_tree_target(condition_tower)

add_library(action_tower src/action_tower.cpp)
configure_navigate_tree_target(action_tower)

add_library(condition_home src/condition_home.cpp)
configure_navigate_tree_target(condition_home)

add_library(action_home src/action_home.cpp)
configure_navigate_tree_target(action_home)

add_library(condition_fort src/condition_fort.cpp)
configure_navigate_tree_target(condition_fort)

add_library(action_fort src/action_fort.cpp)
configure_navigate_tree_target(action_fort)

add_library(condition_free src/condition_free.cpp)
configure_navigate_tree_target(condition_free)

add_library(action_free src/action_free.cpp)
configure_navigate_tree_target(action_free)

add_executable(bt_main src/bt_main.cpp)
configure_navigate_tree_target(bt_main)
ament_target_dependencies(bt_main ament_index_cpp)
target_link_libraries(bt_main
  condition_start
  condition_protect
  action_start35
  condition_operator
  action_operator
  condition_enemy
  action_follow
  condition_blosupeng
  action_blosupeng
  condition_tower
  action_tower
  condition_home
  action_home
  condition_fort
  action_fort
  condition_free
  action_free
)

install(TARGETS bt_main
  LIBRARY DESTINATION lib/${PROJECT_NAME}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
