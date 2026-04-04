# 故障排查指南

## 问题：ros2 topic pub 一直显示 "Waiting for at least 1 matching subscription(s)..."

### 原因
**决策节点没有运行**。消息格式是正确的，但是没有订阅者订阅这个话题。

### 解决方案

#### 1. 检查决策节点是否运行
```bash
ros2 node list | grep -i "decision\|sentry"
```

如果没有看到 `/sentry_decision` 或类似节点，说明决策节点没有启动。

#### 2. 启动决策节点

**方法A：使用launch文件（推荐）**
```bash
cd /home/nuc/rm_2026
source install/setup.bash
ros2 launch rm_sentry_decision rm_sentry_decision.launch.py
```

**方法B：直接运行节点**
```bash
cd /home/nuc/rm_2026
source install/setup.bash
ros2 run rm_sentry_decision decision_node
```

**方法C：使用脚本**
```bash
./src/decision/rm_sentry_decision/scripts/start_decision_node.sh
```

#### 3. 验证节点已启动
```bash
# 检查节点
ros2 node list | grep sentry_decision

# 检查话题订阅者
ros2 topic info /game_detection_status
# 应该看到 Subscription count > 0
```

#### 4. 然后发布消息
```bash
# 现在可以发布消息了
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh operator 5.0 3.0
```

## 完整启动流程

1. **启动导航系统**（你已经完成）
   ```bash
   bash start1.sh
   ```

2. **启动决策节点**（新终端）
   ```bash
   cd /home/nuc/rm_2026
   source install/setup.bash
   ros2 launch rm_sentry_decision rm_sentry_decision.launch.py
   ```

3. **切换行为**（另一个终端）
   ```bash
   cd /home/nuc/rm_2026
   source install/setup.bash
   ./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh operator 5.0 3.0
   ```

## 检查清单

- [ ] 决策节点是否运行？ `ros2 node list | grep sentry_decision`
- [ ] 话题是否有订阅者？ `ros2 topic info /game_detection_status`
- [ ] 工作空间是否source？ `ros2 pkg list | grep rm_sentry_decision`
- [ ] 消息格式是否正确？ 使用脚本可以避免格式错误
