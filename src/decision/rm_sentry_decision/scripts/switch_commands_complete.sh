#!/bin/bash

# 完整的行为切换命令（包含所有必需字段）

# 自动定位并 source 工作区
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
    # shellcheck disable=SC1090
    source "$WORKSPACE_DIR/install/setup.bash"
fi

case "$1" in
  operator)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "用法: $0 operator <x> <y>"
      exit 1
    fi
    echo "切换到: 前往操作手要求位置 ($2, $3)"
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{game_progress: 3, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: $2, operator_y: $3}" --once
    ;;
    
  enemy)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "用法: $0 enemy <x> <y>"
      exit 1
    fi
    echo "切换到: 前往敌方位置 ($2, $3)"
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{game_progress: 3, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: $2, enemy_y: $3, operator_x: 0.0, operator_y: 0.0}" --once
    ;;
    
  cancel)
    echo "取消所有指令"
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{game_progress: 3, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 0.0, operator_y: 0.0}" --once
    ;;
    
  attack-outpost)
    HP=${2:-3000}
    echo "触发攻击前哨站（敌方前哨站血量=$HP）"
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{game_progress: 3, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: $HP, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 0.0, operator_y: 0.0}" --once
    ;;
    
  *)
    echo "用法: $0 {operator|enemy|cancel|attack-outpost} [参数...]"
    echo ""
    echo "示例:"
    echo "  $0 operator 5.0 3.0"
    echo "  $0 enemy 8.0 4.0"
    echo "  $0 cancel"
    echo "  $0 attack-outpost 3000"
    exit 1
    ;;
esac
