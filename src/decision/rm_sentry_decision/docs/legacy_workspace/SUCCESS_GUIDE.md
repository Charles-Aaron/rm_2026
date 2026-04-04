# ✅ 行为切换成功指南

## 🎉 恭喜！消息已成功发布

从你的输出可以看到：
```
publishing #1: rm_decision_interfaces.msg.GameStatus(...operator_x=5.0, operator_y=3.0)
```

这说明：
- ✅ 消息格式正确
- ✅ 消息已成功发布
- ✅ 决策节点正在运行（`/red_standard_robot1/decision_node`）
- ✅ 有4个订阅者正在接收消息

## 📋 两种发布方式

### 方式1：单次发布（测试用）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_simple.sh operator 5.0 3.0
```
- 只发布一次消息
- 适合测试消息格式
- 如果决策节点处理很快，可能已经生效

### 方式2：持续发布（实际控制用，推荐）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh operator 5.0 3.0
```
- 持续发布消息（1Hz频率）
- 确保决策节点持续接收最新状态
- 按 `Ctrl+C` 停止

## 🔍 验证行为是否切换

### 1. 检查导航目标
```bash
# 查看导航目标话题
ros2 topic echo /red_standard_robot1/goal_pose --once
```

### 2. 查看决策节点日志
在运行决策节点的终端查看输出，应该能看到：
- "Pursue Operator" 相关的日志
- 导航目标发送的日志

### 3. 观察机器人行为
- 机器人应该开始导航到操作手指定的位置 (5.0, 3.0)
- 在RViz中可以看到导航路径和目标点

## 🚀 常用切换命令

### 前往操作手位置（持续发布）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh operator 5.0 3.0
```

### 前往敌方位置（持续发布）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh enemy 8.0 4.0
```

### 取消所有指令（持续发布）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh cancel
```

### 触发攻击前哨站（持续发布）
```bash
./src/decision/rm_sentry_decision/scripts/switch_behavior_continuous.sh attack-outpost 3000
```

## 💡 重要提示

1. **持续发布 vs 单次发布**
   - 单次发布：适合测试，消息可能被其他发布者覆盖
   - 持续发布：适合实际控制，确保决策节点始终收到最新状态

2. **停止持续发布**
   - 按 `Ctrl+C` 停止持续发布
   - 停止后，决策节点会使用最后收到的消息状态

3. **多个终端建议**
   - 终端1：运行导航系统
   - 终端2：运行决策节点
   - 终端3：持续发布行为切换消息

4. **检查行为树优先级**
   - Operator（操作手）是最高优先级
   - 如果设置了 operator_x/y，机器人会优先执行这个行为
   - 设置为 0.0 可以取消该行为

## 🎯 下一步

现在你可以：
1. 使用持续发布脚本控制机器人行为
2. 观察机器人的实际导航行为
3. 根据需要切换不同的行为模式

祝你测试顺利！🎉
