# 快速切换行为指令

## ⚠️ 重要：必须先source工作空间

```bash
cd /home/nuc/rm_2026
source install/setup.bash
```

## 🚀 方法1：使用脚本（最简单，推荐）

```bash
# 前往操作手位置
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh operator 5.0 3.0

# 前往敌方位置
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh enemy 8.0 4.0

# 取消所有指令
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh cancel

# 触发攻击前哨站
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh attack-outpost 3000
```

## 📝 方法2：使用YAML文件

```bash
# 先source工作空间
cd /home/nuc/rm_2026
source install/setup.bash

# 取消所有指令
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat src/decision/rm_sentry_decision/docs/examples/game_status_cancel.yaml)" --once

# 前往操作手位置（需要修改YAML文件中的operator_x和operator_y）
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus "$(cat src/decision/rm_sentry_decision/docs/examples/game_status_operator.yaml)" --once
```

## 🔧 方法3：直接命令（需要source工作空间）

```bash
# 先source
cd /home/nuc/rm_2026
source install/setup.bash

# 然后运行（注意：命令很长，建议使用脚本）
ros2 topic pub /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{game_progress: 3, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 5.0, operator_y: 3.0}' --once
```

## 💡 如果遇到 "The passed message type is invalid" 错误

1. **确保source了工作空间**：
   ```bash
   cd /home/nuc/rm_2026
   source install/setup.bash
   ```

2. **检查消息类型是否可用**：
   ```bash
   ros2 interface list | grep rm_decision
   ```

3. **使用脚本（自动处理source）**：
   ```bash
   ./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh operator 5.0 3.0
   ```
