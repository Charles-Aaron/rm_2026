#game_status
uint8 game_progress # 当前比赛阶段
uint16 stage_remain_time # 当前阶段剩余时间
#robot_status
bool team_color # 队伍颜色 0->红 1->蓝
uint16 bullet_allowance_17mm # 17mm 剩余弹量
uint16 current_hp # 血量

#others
uint16 red_1_robot_hp
uint16 red_2_robot_hp
uint16 red_3_robot_hp
uint16 red_4_robot_hp
uint16 red_outpost_hp
uint16 red_base_hp

uint16 blue_1_robot_hp
uint16 blue_2_robot_hp
uint16 blue_3_robot_hp
uint16 blue_4_robot_hp
uint16 blue_outpost_hp
uint16 blue_base_hp
float64 enemy_x
float64 enemy_y

float64 operator_x
float64 operator_y
