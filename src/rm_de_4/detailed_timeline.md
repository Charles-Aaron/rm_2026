# 详细行为树时序图

## 50Hz主循环时序 (20ms周期)

```
帧号: 1      2      3      4      5      6      7      8      9      10
时间: 0ms    20ms   40ms   60ms   80ms   100ms  120ms  140ms  160ms  180ms
     │      │      │      │      │      │      │      │      │      │
     ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼
    ┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐
    │检查 ││检查 ││检查 ││检查 ││检查 ││检查 ││检查 ││检查 ││检查 ││检查 │
    │比赛 ││比赛 ││比赛 ││比赛 ││比赛 ││比赛 ││比赛 ││比赛 ││比赛 ││比赛 │
    │状态 ││状态 ││状态 ││状态 ││状态 ││状态 ││状态 ││状态 ││状态 ││状态 │
    └─────┘└─────┘└─────┘└─────┘└─────┘└─────┘└─────┘└─────┘└─────┘└─────┘
```

## 单帧详细执行时序 (20ms内)

```
时间: 0ms    1ms    2ms    3ms    4ms    5ms    6ms    7ms    8ms    9ms    10ms   11ms   12ms   13ms   14ms   15ms   16ms   17ms   18ms   19ms   20ms
     │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │      │
     ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼
    ┌─────┐
    │Game │◄── 1ms ──►
    │Status│
    └─────┘
         │
         ▼ (如果比赛开始)
    ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
    │                                    Fallback 优先级执行 (2-15ms)                                                                                │
    └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
         │
         ▼
    ┌─────┐
    │Outpost│◄── 1ms ──►
    │Attack │
    └─────┘
         │ (如果成功)
         ▼
    ┌─────┐
    │Robot │◄── 2ms ──►
    │Status│
    └─────┘
         │ (如果成功)
         ▼
    ┌─────┐
    │Send  │◄── 5ms ──►
    │Goal  │
    └─────┘
         │
         ▼
    ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
    │                                    等待下一帧 (16-20ms)                                                                                      │
    └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

## 不同场景的执行时序

### 场景1: 正常攻击前哨站
```
帧1 (0-20ms):
0ms:   GameStatusSub 检查 (1ms)
1ms:   OutpostAttack 检查 (1ms) - SUCCESS
2ms:   RobotStatusSub 检查 (2ms) - SUCCESS  
4ms:   SendGoalAction 执行 (5ms) - RUNNING
9ms:   等待导航完成 (11ms)

帧2 (20-40ms):
20ms:  GameStatusSub 检查 (1ms)
21ms:  OutpostAttack 检查 (1ms) - SUCCESS
22ms:  RobotStatusSub 检查 (2ms) - SUCCESS
24ms:  SendGoalAction 继续 (5ms) - RUNNING
29ms:  等待导航完成 (11ms)

... 持续直到导航完成
```

### 场景2: 需要补给
```
帧1 (0-20ms):
0ms:   GameStatusSub 检查 (1ms)
1ms:   OutpostAttack 检查 (1ms) - FAILURE
2ms:   SelectSupplyPoint 执行 (2ms) - SUCCESS
4ms:   SendGoalAction 执行 (5ms) - RUNNING
9ms:   等待导航完成 (11ms)
```

### 场景3: 队友阵亡，转为防守
```
帧1 (0-20ms):
0ms:   GameStatusSub 检查 (1ms)
1ms:   OutpostAttack 检查 (1ms) - FAILURE
2ms:   SelectSupplyPoint 检查 (2ms) - FAILURE
4ms:   EndgameNeedSupply 检查 (1ms) - FAILURE
5ms:   RushEnemyBuff 检查 (2ms) - FAILURE
7ms:   RobotStatusSub 检查 (2ms) - FAILURE
9ms:   AlliesDown 检查 (1ms) - SUCCESS
10ms:  SendGoalAction 执行 (5ms) - RUNNING
15ms:  等待导航完成 (5ms)

帧2 (20-40ms):
20ms:  GameStatusSub 检查 (1ms)
21ms:  AlliesDown 检查 (1ms) - SUCCESS
22ms:  SendGoalAction 继续 (5ms) - RUNNING
27ms:  等待导航完成 (13ms)

... 导航完成后
帧N:
Xms:   GuardFortress 开始 (1ms) - RUNNING
X+1ms: 守卫60秒开始
```

## 关键时间节点分析

### 条件检查时间分布
```
GameStatusSub:     1ms  - 状态订阅检查
OutpostAttack:     1ms  - 前哨站血量检查
RobotStatusSub:    2ms  - 血量+弹量检查
AlliesDown:        1ms  - 队友状态检查
EndgameNeedSupply: 1ms  - 时间+状态判断
RecoveredEnough:   1ms  - 恢复状态检查
```

### 动作执行时间分布
```
SendGoalAction:    5ms  - 发送导航指令
SelectSupplyPoint: 2ms  - 计算补给点
RandomPatrolPose:  1ms  - 随机选择
RushEnemyBuff:     2ms  - 策略计算
GuardFortress:     1ms  - 守卫逻辑
```

### 导航任务时间
```
短距离导航:    2-5秒   (100-250帧)
中距离导航:    5-10秒  (250-500帧)
长距离导航:    10-20秒 (500-1000帧)
```

## 性能瓶颈分析

### 单帧时间分配
```
条件检查:    2-6ms   (10-30%)
动作执行:    5-10ms  (25-50%)
等待时间:    4-13ms  (20-65%)
```

### 优化建议
```
1. 条件检查并行化: 可节省1-2ms
2. 导航指令缓存: 可节省2-3ms
3. 状态订阅优化: 可节省1ms
4. 决策频率调整: 可考虑30Hz以节省计算资源
```

## 异常情况时序

### 导航失败处理
```
检测失败:    1ms
重试决策:    2ms
新目标计算:  2ms
重新发送:    5ms
总计:        10ms
```

### 通信中断处理
```
检测中断:    1ms
使用缓存:    0ms
降级策略:    2ms
安全措施:    5ms
总计:        8ms
```

### 系统错误处理
```
错误检测:    1ms
安全模式:    1ms
初始移动:    5ms
总计:        7ms
``` 