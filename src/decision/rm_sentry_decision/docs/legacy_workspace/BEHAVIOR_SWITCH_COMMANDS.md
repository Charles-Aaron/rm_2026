# 行为树行为切换指令快速参考

## 🚀 最常用指令

### 1. 前往操作手要求位置
```bash
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{operator_x: 5.0, operator_y: 3.0, game_progress: 3, stage_remain_time: 295}'
```

### 2. 前往敌方位置
```bash
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{enemy_x: 8.0, enemy_y: 4.0, game_progress: 3, stage_remain_time: 295}'
```

### 3. 取消所有指令（让机器人执行其他行为）
```bash
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{operator_x: 0.0, operator_y: 0.0, enemy_x: 0.0, enemy_y: 0.0, game_progress: 3, stage_remain_time: 295}'
```

## 📝 使用切换脚本（推荐）

```bash
# 前往操作手位置
./src/decision/rm_sentry_decision/scripts/switch_behavior.sh operator 5.0 3.0

# 前往敌方位置
./src/decision/rm_sentry_decision/scripts/switch_behavior.sh enemy 8.0 4.0

# 取消所有指令
./src/decision/rm_sentry_decision/scripts/switch_behavior.sh cancel

# 查看帮助
./src/decision/rm_sentry_decision/scripts/switch_behavior.sh help
```

## 🎯 其他常用切换

### 触发攻击前哨站
```bash
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{blue_outpost_hp: 3000, game_progress: 3, stage_remain_time: 295}'
```

### 触发补给行为（低血量）
```bash
ros2 topic pub /robot_status rm_decision_interfaces/msg/RobotStatus \
  '{robot_id: 7, current_hp: 80, shooter_heat: 20, team_color: true}'
```

## 📊 查看当前状态

```bash
# 查看话题列表
ros2 topic list

# 查看话题内容
ros2 topic echo /game_detection_status
ros2 topic echo /robot_status

# 查看话题频率
ros2 topic hz /game_detection_status
```

## ⚠️ 重要提示

1. **坐标为零 = 取消指令**：将 `operator_x/y` 或 `enemy_x/y` 设为 0.0 会取消对应行为
2. **行为优先级**：行为树按优先级执行，Operator > 补给 > Enemy > 攻击前哨站 > ...
3. **命名空间**：如果使用了命名空间，话题路径会变为 `/<namespace>/game_detection_status`

## 🔗 完整文档

详细说明请查看 `src/decision/rm_sentry_decision/docs/legacy_workspace/README.md` 文件。
