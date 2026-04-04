#!/bin/bash

# 持续发布行为切换消息（推荐用于实际控制）

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
    echo "持续发布: 前往操作手要求位置 ($2, $3)"
    echo "按 Ctrl+C 停止"
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
    ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)"
    ;;
    
  enemy)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "用法: $0 enemy <x> <y>"
      echo "示例: $0 enemy 8.0 4.0"
      exit 1
    fi
    echo "持续发布: 前往敌方位置 ($2, $3)"
    echo "按 Ctrl+C 停止"
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
    ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)"
    ;;
    
  cancel)
    echo "持续发布: 取消所有指令"
    echo "按 Ctrl+C 停止"
    ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat "$EXAMPLES_DIR/game_status_cancel.yaml")"
    ;;
    
  attack-outpost)
    HP=${2:-3000}
    echo "持续发布: 触发攻击前哨站（敌方前哨站血量=$HP）"
    echo "按 Ctrl+C 停止"
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
    ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat /tmp/game_status_temp.yaml)"
    ;;
    
  help|--help|-h)
    echo "持续发布行为切换消息脚本"
    echo ""
    echo "用法: $0 <行为类型> [参数...]"
    echo ""
    echo "可用行为:"
    echo "  operator <x> <y>         - 持续发布前往操作手要求位置"
    echo "  enemy <x> <y>            - 持续发布前往敌方位置"
    echo "  cancel                   - 持续发布取消所有指令"
    echo "  attack-outpost [hp]       - 持续发布触发攻击前哨站（默认hp=3000）"
    echo ""
    echo "注意: 此脚本会持续发布消息（1Hz），按 Ctrl+C 停止"
    echo ""
    echo "示例:"
    echo "  $0 operator 5.0 3.0"
    echo "  $0 enemy 8.0 4.0"
    echo "  $0 cancel"
    ;;
    
  *)
    echo "错误: 未知的行为类型 '$1'"
    echo "使用 '$0 help' 查看帮助"
    exit 1
    ;;
esac
