# AllRobotHP.msg
# 所有机器人的血量信息

std_msgs/Header header

# 前哨站血量
uint16 red_outpost_hp
uint16 blue_outpost_hp

# 基地血量
uint16 red_base_hp
uint16 blue_base_hp

# 红方机器人血量 (按ID编号)
uint16 red_1_robot_hp
uint16 red_2_robot_hp
uint16 red_3_robot_hp
uint16 red_4_robot_hp
uint16 red_5_robot_hp
uint16 red_7_robot_hp

# 蓝方机器人血量 (按ID编号)
uint16 blue_1_robot_hp
uint16 blue_2_robot_hp
uint16 blue_3_robot_hp
uint16 blue_4_robot_hp
uint16 blue_5_robot_hp
uint16 blue_7_robot_hp
