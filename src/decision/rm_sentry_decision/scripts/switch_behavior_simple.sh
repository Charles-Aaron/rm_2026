#!/bin/bash

# 简单的行为切换脚本 - 使用YAML文件

# 自动source工作空间
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
EXAMPLES_DIR="$PACKAGE_DIR/docs/examples"

if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
    # shellcheck disable=SC1090
    source "$WORKSPACE_DIR/install/setup.bash"
fi

case "$1" in
  operator)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "用法: $0 operator <x> <y>"
      echo "示例: $0 operator 5.0 3.0"
      exit 1
    fi
    echo "切换到: 前往操作手要求位置 ($2, $3)"
    # 创建临时YAML文件
    cat > /tmp/game_status_temp.yaml <<EOF
game_progress: 4
stage_remain_time: 295
team_color: false
bullet_allowance_17mm: 450
current_hp: 175
red_1_robot_hp: 100
red_2_robot_hp: 100
red_3_robot_hp: 100
red_4_robot_hp: 100
red_outpost_hp: 0
red_base_hp: 1500
blue_1_robot_hp: 100
blue_2_robot_hp: 100
blue_3_robot_hp: 100
blue_4_robot_hp: 100
blue_outpost_hp: 0
blue_base_hp: 1500
enemy_x: 0.0
enemy_y: 0.0
operator_x: $2
operator_y: $3
EOF
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)" --once
    ;;
    
  enemy)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "用法: $0 enemy <x> <y>"
      echo "示例: $0 enemy 8.0 4.0"
      exit 1
    fi
    echo "切换到: 前往敌方位置 ($2, $3)"
    # 创建临时YAML文件
    cat > /tmp/game_status_temp.yaml <<EOF
game_progress: 4
stage_remain_time: 295
team_color: false
bullet_allowance_17mm: 450
current_hp: 175
red_1_robot_hp: 100
red_2_robot_hp: 100
red_3_robot_hp: 100
red_4_robot_hp: 100
red_outpost_hp: 0
red_base_hp: 1500
blue_1_robot_hp: 100
blue_2_robot_hp: 100
blue_3_robot_hp: 100
blue_4_robot_hp: 100
blue_outpost_hp: 0
blue_base_hp: 1500
enemy_x: $2
enemy_y: $3
operator_x: 0.0
operator_y: 0.0
EOF
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)" --once
    ;;
    
  cancel)
    echo "取消所有指令"
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat "$EXAMPLES_DIR/game_status_cancel.yaml")" --once
    ;;
    
  attack-outpost)
    HP=${2:-3000}
    echo "触发攻击前哨站（敌方前哨站血量=$HP）"
    # 创建临时YAML文件
    cat > /tmp/game_status_temp.yaml <<EOF
game_progress: 4
stage_remain_time: 295
team_color: false
bullet_allowance_17mm: 450
current_hp: 175
red_1_robot_hp: 100
red_2_robot_hp: 100
red_3_robot_hp: 100
red_4_robot_hp: 100
red_outpost_hp: 0
red_base_hp: 1500
blue_1_robot_hp: 100
blue_2_robot_hp: 100
blue_3_robot_hp: 100
blue_4_robot_hp: 100
blue_outpost_hp: $HP
blue_base_hp: 1500
enemy_x: 0.0
enemy_y: 0.0
operator_x: 0.0
operator_y: 0.0
EOF
    ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)" --once
    ;;
    
  help|--help|-h)
    echo "行为树行为切换脚本"
    echo ""
    echo "用法: $0 <行为类型> [参数...]"
    echo ""
    echo "可用行为:"
    echo "  operator <x> <y>         - 前往操作手要求位置"
    echo "  enemy <x> <y>            - 前往敌方位置"
    echo "  cancel                   - 取消所有指令"
    echo "  attack-outpost [hp]       - 触发攻击前哨站（默认hp=3000）"
    echo ""
    echo "示例:"
    echo "  $0 operator 5.0 3.0"
    echo "  $0 enemy 8.0 4.0"
    echo "  $0 cancel"
    echo "  $0 attack-outpost 3500"
    ;;
    
  *)
    echo "错误: 未知的行为类型 '$1'"
    echo "使用 '$0 help' 查看帮助"
    exit 1
    ;;
esac
