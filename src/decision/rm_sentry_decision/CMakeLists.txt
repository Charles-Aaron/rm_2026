cmake_minimum_required(VERSION 3.5)
project(rm_sentry_decision)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(rm_decision_interfaces REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(ament_index_cpp REQUIRED)

# 查找yaml-cpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
# find_package(rosidl_default_generators REQUIRED)

# 添加头文件包含路径
include_directories(
  include
  ${rclcpp_INCLUDE_DIRS}
  ${rclcpp_action_INCLUDE_DIRS}
  ${rcl_interfaces_INCLUDE_DIRS}
  ${behaviortree_cpp_v3_INCLUDE_DIRS}
  ${rm_decision_interfaces_INCLUDE_DIRS}
  ${nav2_msgs_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
)

# 添加可执行文件
add_executable(decision_node
  src/decision_node.cpp
  src/robot_status_sub.cpp
  src/game_status_sub.cpp
  # src/base_attack.cpp
  src/outpost_attack.cpp
  src/send_goal.cpp
  src/send_through_goals.cpp
  src/need_supply.cpp
  src/recovered_enough.cpp
  # src/attack_strategy.cpp
  src/allies_down.cpp
  src/guard_fortress.cpp
  # src/endgame_need_supply.cpp
  # src/should_rush_base.cpp
  src/hero_protect_attack.cpp
  # src/compare_hp_diff.cpp
  # src/late_game_decision.cpp
  # src/rush_enemy_buff.cpp
  src/random_patrol_pose.cpp
  src/select_supply_point.cpp
  src/parameter_manager.cpp
  src/enemy.cpp
  src/operator.cpp
  # src/wait_for_outpost_destroyed.cpp
  src/self_hp_below.cpp
  src/self_hp_at_least.cpp
  src/is_outside_pose_radius.cpp
  src/pose_behavior.cpp
  src/sim_visualization_move.cpp
  src/calculate_attack_pose.cpp
  src/set_nav_speed.cpp
)

# 使用ament_target_dependencies链接库
ament_target_dependencies(decision_node
  rclcpp
  rclcpp_action
  rcl_interfaces
  behaviortree_cpp_v3
  rm_decision_interfaces
  nav2_msgs
  tf2
  tf2_ros
  ament_index_cpp
)

# 链接yaml-cpp库
target_link_libraries(decision_node ${YAML_CPP_LIBRARIES})
target_compile_options(decision_node PRIVATE ${YAML_CPP_CFLAGS_OTHER})

# 安装目标
install(TARGETS decision_node
  DESTINATION lib/${PROJECT_NAME}
)

# 安装头文件
install(DIRECTORY include/
  DESTINATION include
)
 
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

# 安装行为树 XML 配置
install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

# 安装 Python 测试脚本 (确保 chmod +x)
install(
  PROGRAMS scripts/simple_test.py
  DESTINATION lib/${PROJECT_NAME}
)

# Message files
#set(msg_files
#  msg/AllRobotHPEx.msg
#)
#
#rosidl_generate_interfaces(${PROJECT_NAME}
#  ${msg_files}
#  DEPENDENCIES builtin_interfaces
#  SKIP_INSTALL
#)
#
#rosidl_target_interfaces(decision_node
#  ${PROJECT_NAME} "rosidl_typesupport_cpp")
#
#ament_export_dependencies(rosidl_default_runtime)

ament_package()
