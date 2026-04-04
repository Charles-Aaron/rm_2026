# 🔧 修复：RViz中没有路径的问题

## 问题原因

行为树有一个**前置条件**，需要满足才能执行后续行为：

```xml
<Condition ID="GameStatusSub" name="CheckGameStart" start_status="4"/>
```

这个条件检查 `game_progress` 字段，**必须等于 4** 才会返回 SUCCESS，然后行为树才会执行后续的 Operator 节点。

## ✅ 解决方案

### 方法1：使用修复后的脚本（推荐）

脚本已经修复，现在会发布 `game_progress: 4`：

```bash
# 持续发布（确保game_progress=4）
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh operator 5.0 3.0
```

### 方法2：手动发布正确的消息

确保 `game_progress: 4`（不是3）：

```bash
ros2 topic pub -r 1 /game_detection_status rm_decision_interfaces/msg/GameStatus \
  '{game_progress: 4, stage_remain_time: 295, team_color: false, bullet_allowance_17mm: 450, current_hp: 175, red_1_robot_hp: 100, red_2_robot_hp: 100, red_3_robot_hp: 100, red_4_robot_hp: 100, red_outpost_hp: 0, red_base_hp: 1500, blue_1_robot_hp: 100, blue_2_robot_hp: 100, blue_3_robot_hp: 100, blue_4_robot_hp: 100, blue_outpost_hp: 0, blue_base_hp: 1500, enemy_x: 0.0, enemy_y: 0.0, operator_x: 5.0, operator_y: 3.0}'
```

## 🔍 验证步骤

### 1. 检查行为树条件是否满足

查看决策节点日志，应该能看到：
- "CheckGameStart" 返回 SUCCESS
- "Pursue Operator" 开始执行

### 2. 检查导航Action Server

```bash
# 检查导航action server是否可用
ros2 action list | grep navigate
```

应该看到：`/navigate_to_pose` 或 `/red_standard_robot1/navigate_to_pose`

### 3. 检查导航目标是否发送

```bash
# 查看是否有导航目标发送
ros2 topic echo /red_standard_robot1/goal_pose --once
```

### 4. 检查导航反馈

```bash
# 查看导航反馈
ros2 topic echo /red_standard_robot1/navigate_to_pose/_action/feedback --once
```

## 📋 完整检查清单

- [ ] `game_progress: 4`（不是3）
- [ ] `operator_x` 和 `operator_y` 非零
- [ ] 决策节点正在运行
- [ ] 导航action server可用
- [ ] 消息持续发布（使用 `-r 1`）

## 🚀 快速修复命令

```bash
# 1. 停止当前的发布（如果有）
# 按 Ctrl+C

# 2. 使用修复后的脚本重新发布
cd /home/nuc/rm_2026
source install/setup.bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh operator 5.0 3.0

# 3. 观察RViz，应该能看到路径了
```

## 💡 其他可能的问题

如果修复后仍然没有路径，检查：

1. **导航系统是否正常**
   ```bash
   ros2 node list | grep navigator
   ```

2. **地图是否加载**
   ```bash
   ros2 topic echo /map --once
   ```

3. **机器人位置是否已知**
   ```bash
   ros2 topic echo /tf --once | grep map
   ```

4. **查看决策节点完整日志**
   在运行决策节点的终端查看是否有错误信息
