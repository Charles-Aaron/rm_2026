# 直接运行的命令（不使用脚本）

## ⚠️ 重要：先source工作空间

```bash
cd /home/nuc/rm_2026
source install/setup.bash
```

## 🚀 行为切换命令

### 1. 前往操作手要求位置（单次发布）

```bash
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 5.0, operator_y: 3.0}' --once
```

### 2. 前往操作手要求位置（持续发布，推荐）

```bash
ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 5.0, operator_y: 3.0}'
```

**注意**：按 `Ctrl+C` 停止持续发布

### 3. 前往敌方位置（持续发布）

```bash
ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 8.0, enemy_y: 4.0, operator_x: 0.0, operator_y: 0.0}'
```

### 4. 取消所有指令（持续发布）

```bash
ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 0.0, operator_y: 0.0}'
```

### 5. 触发攻击前哨站（持续发布）

```bash
ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 3000, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 0.0, operator_y: 0.0}'
```

## 📝 自定义坐标

修改以下字段即可：

- **操作手位置**：修改 `operator_x` 和 `operator_y`
- **敌方位置**：修改 `enemy_x` 和 `enemy_y`
- **取消指令**：将坐标设为 `0.0`

## 🔑 关键字段说明

- `game_progress: 4` - **必须为4**，否则行为树不会执行
- `operator_x`, `operator_y` - 操作手要求的位置坐标（非零才会执行）
- `enemy_x`, `enemy_y` - 敌方位置坐标（非零才会执行）
- `blue_outpost_hp` - 敌方前哨站血量（>2000会触发攻击）

## 💡 使用建议

1. **持续发布**：使用 `-r 1`（1Hz频率）持续发布，确保决策节点始终收到最新状态
2. **停止发布**：按 `Ctrl+C` 停止
3. **单次发布**：使用 `--once` 只发布一次（适合测试）

