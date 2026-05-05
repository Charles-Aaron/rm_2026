cmake_minimum_required(VERSION 3.10)
project(rm_bt)

## Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#######################
## Find dependencies ##
#######################

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

###########
## Build ##
###########

set(PACKAGE_INCLUDE_DEPENDS
    rclcpp
    std_msgs
    behaviortree_cpp
    behaviortree_ros2
    nav2_msgs
    geometry_msgs
    rm_sentry_decision_interfaces
)

# 定义宏函数 add_target_dependencies
function(add_target_dependencies target)
    target_include_directories(${target} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
    ament_target_dependencies(${target} ${PACKAGE_INCLUDE_DEPENDS})
    target_compile_definitions(${target} PRIVATE BT_PLUGIN_EXPORT)
endfunction()

ament_auto_add_executable(rm_bt
    src/rm_bt.cpp
)

# Controllers

# Decorators

# Conditions
ament_auto_add_library(check_game_start SHARED
    plugins/condition/check_game_start.cpp
)
add_target_dependencies(check_game_start)

#判断哨兵是否需要回血
ament_auto_add_library(restores_health SHARED
    plugins/condition/restores_health.cpp
)
add_target_dependencies(restores_health)

#判断哨兵机器人是否自瞄锁定地方机器人
ament_auto_add_library(check_detect_enemy SHARED
    plugins/condition/check_detect_enemy.cpp
)
add_target_dependencies(check_detect_enemy)
##判断地方前哨战是否阵亡
ament_auto_add_library(check_enemy_outpost_die SHARED
    plugins/condition/check_enemy_outpost_die.cpp
)
add_target_dependencies(check_enemy_outpost_die)

#判断哨兵是否需要回血或者补充弹丸，为restores_health升级版本
ament_auto_add_library(restores_hp_or_pill SHARED
    plugins/condition/restores_hp_or_pill.cpp
)
add_target_dependencies(restores_hp_or_pill)

##用于判断比赛时间超过设计的时间时，哨兵是否返回基地进行巡航
ament_auto_add_library(check_time_back SHARED
    plugins/condition/check_time_back.cpp
)
add_target_dependencies(check_time_back)


# Actions
ament_auto_add_library(send_goal SHARED 
    plugins/action/send_goal.cpp
)
add_target_dependencies(send_goal)

ament_auto_add_library(sub_game_status SHARED
    plugins/action/sub_game_status.cpp
)
add_target_dependencies(sub_game_status)

ament_auto_add_library(sub_decision_status SHARED
    plugins/action/sub_decision_status.cpp
)
add_target_dependencies(sub_decision_status)

ament_auto_add_library(sub_BothCampHp SHARED
    plugins/action/sub_BothCampHp.cpp
)
add_target_dependencies(sub_BothCampHp)

ament_auto_add_library(sub_is_detect_enemy SHARED
    plugins/action/sub_is_detect_enemy.cpp
)
add_target_dependencies(sub_is_detect_enemy)

ament_auto_add_library(pub_float SHARED
    plugins/action/pub_float.cpp
)
add_target_dependencies(pub_float)

ament_auto_add_library(pub_current_area SHARED
    plugins/action/pub_current_area.cpp
)
add_target_dependencies(pub_current_area)

ament_auto_add_library(pub_current_location SHARED
    plugins/action/pub_current_location.cpp
)   
add_target_dependencies(pub_current_location)

#订阅比赛时间
ament_auto_add_library(sub_game_time SHARED
    plugins/action/sub_game_time.cpp
)
add_target_dependencies(sub_game_time)
#订阅当前哨兵允许发弹量
ament_auto_add_library(sub_projectile_number SHARED
    plugins/action/sub_projectile_number.cpp
)
add_target_dependencies(sub_projectile_number)
#订阅敌方前哨战是否阵亡
ament_auto_add_library(sub_enemy_outpost_status SHARED
    plugins/action/sub_enemy_outpost_status.cpp
)
add_target_dependencies(sub_enemy_outpost_status)

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
)