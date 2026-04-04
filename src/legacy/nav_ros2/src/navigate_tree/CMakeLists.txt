cmake_minimum_required(VERSION 3.5)
project(navigate_tree)

# 找到必要的依赖项
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)  # 确保找到 behaviortree_cpp_v3
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(unique_identifier_msgs REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(customize_messages REQUIRED)
find_package(rosidl_generator_cpp REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# 包含头文件路径
include_directories(
    ${rclcpp_INCLUDE_DIRS}
    ${behaviortree_cpp_v3_INCLUDE_DIRS}  # 包含 behaviortree_cpp_v3 的头文件路径
    ${rclcpp_action_INCLUDE_DIRS}
    ${nav2_msgs_INCLUDE_DIRS}
    ${unique_identifier_msgs_INCLUDE_DIRS}
    ${rcl_interfaces_INCLUDE_DIRS}
     ${customize_messages_INCLUDE_DIRS}  # 添加自定义消息包的头文件路径
    ${CMAKE_CURRENT_SOURCE_DIR}/include/navigate_tree
    ${rosidl_generator_cpp_INCLUDE_DIRS} 
    ${tf2_ros_INCLUDE_DIRS} 
    ${tf2_INCLUDE_DIRS} 
    ${tf2_geometry_msgs_INCLUDE_DIRS} 
    ${geometry_msgs_INCLUDE_DIRS} 
    ${std_msgs_INCLUDE_DIRS} 
)

# 添加可执行文件和库
add_executable(bt_main src/bt_main.cpp)

add_library(condition_start src/condition_start.cpp)
add_library(condition_protect src/condition_protect.cpp)
add_library(action_start35 src/action_start35.cpp)
add_library(condition_operator src/condition_operator.cpp)
add_library(action_operator src/action_operator.cpp)
add_library(condition_enemy src/condition_enemy.cpp)
add_library(action_follow src/action_follow.cpp)
add_library(condition_blosupeng src/condition_blosupeng.cpp)
add_library(action_blosupeng src/action_blosupeng.cpp)
add_library(condition_tower src/condition_tower.cpp)
add_library(action_tower src/action_tower.cpp)
add_library(condition_home src/condition_home.cpp)
add_library(action_home src/action_home.cpp)
add_library(condition_fort src/condition_fort.cpp)
add_library(action_fort src/action_fort.cpp)
add_library(condition_free src/condition_free.cpp)
add_library(action_free src/action_free.cpp)

# 链接依赖项
target_link_libraries(bt_main
    ${rclcpp_LIBRARIES}
    /opt/ros/humble/lib/libbehaviortree_cpp_v3.so
    ${rclcpp_action_LIBRARIES}
    ${nav2_msgs_LIBRARIES}
    ${unique_identifier_msgs_LIBRARIES}
    ${rcl_interfaces_LIBRARIES}
    ${customize_messages_LIBRARIES}  # 添加自定义消息库的链接
    ${rosidl_generator_cpp_INCLUDE_DIRS} 
    ${tf2_ros_INCLUDE_DIRS} 
    ${tf2_INCLUDE_DIRS} 
    ${tf2_geometry_msgs_INCLUDE_DIRS} 
    ${geometry_msgs_INCLUDE_DIRS} 
    ${std_msgs_INCLUDE_DIRS}
    condition_start
    action_start35
    condition_protect
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

target_link_libraries(condition_start
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_protect
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_start35
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_operator
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_operator
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_enemy
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_blosupeng
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_blosupeng
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_tower
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_tower
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_home
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_home
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_fort
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_fort
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(condition_free
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_free
    ${customize_messages_LIBRARIES}  # 只链接自定义消息库
)
target_link_libraries(action_follow
    ${customize_messages_LIBRARIES}
    ${tf2_ros_LIBRARIES} 
    ${tf2_LIBRARIES} 
    ${tf2_geometry_msgs_LIBRARIES} 
    ${geometry_msgs_LIBRARIES}   # 只链接自定义消息库
)
# 安装目标

install(TARGETS bt_main
  LIBRARY DESTINATION lib/${PROJECT_NAME}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY 
    launch
    DESTINATION share/${PROJECT_NAME}
)
install(DIRECTORY config
    DESTINATION share/${PROJECT_NAME}
)

# 包含 ament 安装规则
ament_package()
