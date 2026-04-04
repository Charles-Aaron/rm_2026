#!/bin/bash

# 行为树行为切换脚本
# 用法: ./switch_behavior.sh <行为类型> [参数...]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
    # shellcheck disable=SC1090
    source "$WORKSPACE_DIR/install/setup.bash"
fi

NAMESPACE="${NAMESPACE:-}"  # 可以通过环境变量设置命名空间
TOPIC_PREFIX="${NAMESPACE:+/$NAMESPACE}"

case "$1" in
  operator)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "错误: 需要提供 x 和 y 坐标"
      echo "用法: $0 operator <x> <y>"
      exit 1
    fi
    echo "切换到: 前往操作手要求位置 ($2, $3)"
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{operator_x: $2, operator_y: $3, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  enemy)
    if [ -z "$2" ] || [ -z "$3" ]; then
      echo "错误: 需要提供 x 和 y 坐标"
      echo "用法: $0 enemy <x> <y>"
      exit 1
    fi
    echo "切换到: 前往敌方位置 ($2, $3)"
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{enemy_x: $2, enemy_y: $3, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  cancel)
    echo "取消所有指令（操作手和敌方）"
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{operator_x: 0.0, operator_y: 0.0, enemy_x: 0.0, enemy_y: 0.0, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  cancel-operator)
    echo "取消操作手指令"
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{operator_x: 0.0, operator_y: 0.0, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  cancel-enemy)
    echo "取消敌方追踪"
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{enemy_x: 0.0, enemy_y: 0.0, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  attack-outpost)
    echo "触发攻击前哨站（设置敌方前哨站血量>2000）"
    if [ -z "$2" ]; then
      HP=3000
    else
      HP=$2
    fi
    ros2 topic pub ${TOPIC_PREFIX}/game_detection_status rm_decision_interfaces/msg/GameStatus \
      "{blue_outpost_hp: $HP, game_progress: 3, stage_remain_time: 295}"
    ;;
    
  supply)
    echo "触发补给行为（设置低血量）"
    if [ -z "$2" ]; then
      HP=80
    else
      HP=$2
    fi
    ros2 topic pub ${TOPIC_PREFIX}/robot_status rm_decision_interfaces/msg/RobotStatus \
      "{robot_id: 7, current_hp: $HP, shooter_heat: 20, team_color: true}"
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
    echo "  cancel-operator          - 取消操作手指令"
    echo "  cancel-enemy             - 取消敌方追踪"
    echo "  attack-outpost [hp]      - 触发攻击前哨站（默认hp=3000）"
    echo "  supply [hp]              - 触发补给行为（默认hp=80）"
    echo ""
    echo "示例:"
    echo "  $0 operator 5.0 3.0"
    echo "  $0 enemy 8.0 4.0"
    echo "  $0 cancel"
    echo "  $0 attack-outpost 3500"
    echo ""
    echo "环境变量:"
    echo "  NAMESPACE                - 设置话题命名空间（如: red_standard_robot1）"
    echo ""
    echo "示例（带命名空间）:"
    echo "  NAMESPACE=red_standard_robot1 $0 operator 5.0 3.0"
    ;;
    
  *)
    echo "错误: 未知的行为类型 '$1'"
    echo "使用 '$0 help' 查看帮助"
    exit 1
    ;;
esac
