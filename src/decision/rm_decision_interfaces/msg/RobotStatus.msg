# Unified robot status payload.
# Fields are merged from the old nav_ros2 interface and the newer sentry
# decision workspace so the repository has a single compatible definition.
uint8 robot_id
uint16 current_hp
uint16 shooter_heat
bool team_color
bool is_attacked
bool is_detect_enemy
uint16 bullet_allowance_17mm
