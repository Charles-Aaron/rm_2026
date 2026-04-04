# Unified game and decision status payload.
# The message intentionally keeps the old minimal match-state fields and the
# richer fields used by the current sentry decision tree.

# Match state
uint8 game_progress
uint16 stage_remain_time

# Aggregated robot state mirrored into decision input
bool team_color
uint16 bullet_allowance_17mm
uint16 current_hp

# HP snapshot used by several behavior-tree conditions
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

# External targets injected into decision logic
float64 enemy_x
float64 enemy_y
float64 operator_x
float64 operator_y
