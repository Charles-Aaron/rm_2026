# 行为树策略与写法对比总结

本文对比四套行为树/决策实现：

- 当前工程：`rm_sentry_decision`
- 川大方案：`chuanda/rm_decision_cpp`
- `rm_bt` 方案：当前目录里未找到精确的 `rm_bt2`，这里按 `rm_bt/rm_bt` 分析
- COD 方案：`cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior`

重点关注三个问题：

- 策略层：比赛中优先做什么
- 行为树写法：怎样组织优先级、状态更新、补给、巡逻
- 可吸收点：哪些设计适合合入当前哨兵决策

---

## 1. 当前工程 rm_sentry_decision

### 策略概况

当前工程里有多份 XML。你最近关注的是：

- `rm_sentry_decision/config/保守和激烈/rmuc_2025_aggressive.xml`
- `rm_sentry_decision/config/保守和激烈/rmuc_2025_conservative.xml`

激进版主流程大致是：

```text
SequenceStar aggressive_offense_strategy
├── GameStatusSub(start_status=4)
├── ReactiveFallback init_phase
│   ├── init_supply_if_needed
│   └── goto_first_attack_point
└── Repeat
    └── ReactiveFallback aggressive_main_loop
        ├── retreat_to_supply
        ├── guard_buff_point
        ├── go_to_base_defense
        └── attack_cycle
```

主循环优先级：

1. 低血撤退补给
2. 队友阵亡守增益点
3. 终局回防基地
4. 第一进攻点和第二进攻点之间进攻循环

### 行为树写法特点

优点：

- 主循环用了 `ReactiveFallback`，低血补给分支写在第一位，理论上能抢占后面的进攻循环。
- 补给流程使用 `SequenceStar`，有“进入流程后继续执行”的意图。
- `SendThroughGoalsAction` 支持途经点，适合回补给或穿越路线。
- `SelfHpBelow` 和 `SelfHpAtLeast` 做了比赛状态检查，避免上电默认 0 血量误触发。

风险：

- 激进版低血后不是直接回补给点，而是先执行 `retreat_via_waypoints`，终点是第一进攻点 `(5.27, -3.07)`，之后才执行 `goto_supply_from_attack1` 去补给点 `(0,0)`。如果前一个导航卡住，补给点动作永远不会被 tick 到。
- 状态输入分散。`GameStatusSub` 订阅 `/game_status`，`SelfHpBelow` 订阅 `/game_detection_status`。如果实车只发布其中一个话题，就会出现比赛已开始但低血条件没有更新的情况。
- 主循环当前是先 tick 行为树，再 `spin_some`，本帧使用的是上一帧的 ROS 数据。
- `SendGoalAction` 和 `SendThroughGoalsAction` 在发布 `/navigation_done` 时有循环 sleep，单次会阻塞约 0.5 秒，影响响应低血抢占。

---

## 2. 川大 chuanda 方案

相关文件：

- `chuanda/rm_decision_cpp/src/tree_exec_node.cpp`
- `chuanda/rm_decision_cpp/behavior_tree/25trees/25UCblue.xml`
- `chuanda/rm_decision_cpp/behavior_tree/25trees/25UCred.xml`
- `chuanda/rm_decision_cpp/src/behaviors/topics2blackboard.cpp`
- `chuanda/rm_decision_cpp/src/behaviors/nav2pose.cpp`

### 策略概况

川大方案的核心结构是：

```text
ReactiveSequence
├── Topics2Blackboard
└── ReactiveFallback(_while="game_progress==4")
    ├── low hp / low ammo supply
    ├── base/outpost related patrol or defense
    ├── tracking attack
    └── normal patrol
```

以 `25UCblue.xml` 为例：

- 每轮先通过 `Topics2Blackboard` 将裁判系统和视觉目标写到黑板。
- 比赛开始后进入 `ReactiveFallback`。
- 低血/低弹分支放在最前面。
- 正常状态下根据基地血量、前哨站血量、是否识别目标、比赛时间选择进攻或巡逻。

补给触发示例：

```xml
<ScriptCondition code="my_base_hp&lt;2001 &amp;&amp; (current_hp&lt;155||projectile_allowance_17mm&lt;16)"/>
```

蓝方还有更保守的高基地血量触发条件：

```xml
<ScriptCondition code="my_base_hp&gt;2000 &amp;&amp; (current_hp&lt;275||projectile_allowance_17mm&lt;16)"/>
```

### 行为树写法特点

优点：

- 状态统一写黑板，XML 条件直接读 `{current_hp}`、`{projectile_allowance_17mm}`、`{game_progress}`，逻辑直观。
- 每轮执行顺序是 `spin_some(node)` 再 `tree.tickOnce()`，保证 tick 时拿到最新回调数据。
- 低血/低弹补给分支位于 `ReactiveFallback` 前面，不容易被巡逻/进攻分支饿死。
- 补给分支用 `SequenceWithMemory`，进入补给流程后会保持流程进度，不会每帧从头重来。
- 导航动作 `Nav2Pose` 在 `onHalted()` 里取消旧 goal，适合高优先级抢占。

风险：

- `Nav2Pose::onRunning()` 发现 goal 更新时，旧 goal 的取消逻辑被注释掉，可能有旧目标残留风险。
- `spin_until_future_complete` 在动作节点里会阻塞等待 goal handle，虽然有超时，但会占用行为树线程。
- 部分 XML 中 goal 字符串疑似有格式笔误，例如 `-2.10,-5.88.0,...`，实际使用前要校验。

### 适合吸收的点

- 建立统一的 `Topics2Blackboard` / `GameStatusToBlackboard`。
- 用 XML 的黑板变量表达关键条件，避免每个条件节点各自订阅不同话题。
- 低血/低弹分支放在 `ReactiveFallback` 第一位，并直接进入补给流程。
- 主循环改为先 `spin_some` 再 tick。

---

## 3. rm_bt 方案

相关文件：

- `rm_bt/rm_bt/config/rm_bt_RMUC2.xml`
- `rm_bt/rm_bt/config/rm_bt_tree.xml`
- `rm_bt/rm_bt/plugins/condition/restores_hp_or_pill.cpp`
- `rm_bt/rm_bt/plugins/action/send_goal.cpp`
- `rm_bt/rm_bt/src/rm_bt.cpp`

### 策略概况

`rm_bt` 方案更像一个插件化、子树化的策略系统。

主树大致是：

```text
ReactiveSequence
├── SubTree 获取比赛相关信息
└── WhileDoElse
    ├── ISGameStart
    ├── WhileDoElse
    │   ├── IfRestoresHpOrPill
    │   ├── SubTree 路线(回到补给区)
    │   └── WhileDoElse
    │       ├── IsTimeBack
    │       ├── SubTree 比赛时间>=设定时间，返回基地
    │       └── SubTree 比赛时间<设定时间，正常策略
    └── Sleep
```

正常策略里再分：

- 识别到敌人后停下来
- 敌方前哨站死亡后去中央高地巡逻
- 前哨站未死亡时去敌方前哨站附近巡逻
- 到指定比赛时间后回基地巡逻

### 行为树写法特点

优点：

- 子树拆分非常清晰，`获取比赛相关信息`、`路线(回到补给区)`、`巡逻区(基地)`、`巡逻区(中央高地)` 都是独立 `BehaviorTree ID`。
- 补给条件 `IfRestoresHpOrPill` 有滞回逻辑：
  - HP 低于阈值时进入补给。
  - 进入补给后，只要还没达到满血阈值，就继续保持补给状态。
  - 同时考虑弹丸数量和弹丸刷新。
- 使用 BehaviorTree.ROS2 插件体系：
  - `RosTopicSubNode` 订阅话题并写黑板。
  - `RosActionNode` 封装 Nav2 Action。
  - 自定义节点以插件动态加载。
- `SendGoalAction` 基于 `BT::RosActionNode<nav2_msgs::action::NavigateToPose>`，动作生命周期比手写 future 更标准。

风险：

- `WhileDoElse` 嵌套较深，对不熟悉 BT.CPP 4 的人不如 `ReactiveFallback` 直观。
- 插件化架构迁移成本较高，当前工程是 BehaviorTree.CPP v3，不宜整套照搬。
- `rm_bt/src/rm_bt.cpp` 使用固定文件名 `./rm_bt_RMUC2.xml`，部署时依赖工作目录。

### 适合吸收的点

- 补给条件的“滞回保持”必须吸收。
- 把长策略拆成子树，减少一个 XML 文件内的巨型嵌套。
- 条件节点应该只表达策略条件，不要混杂导航细节。
- 导航动作应有清晰的 halt/cancel 行为。

---

## 4. COD_Behavior 方案

相关文件：

- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/src/cod_behavior.cpp`
- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/cod_bt/singlenav_attack_front_tree.xml`
- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/cod_bt/multiplenav_attack_tree.xml`
- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/include/cod_behavior/action.h`
- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/include/cod_behavior/condition.h`
- `cod_-rm2026_-behavior-tree-feature-finish/COD_Behavior/doc/PubNav2Goal_Patrol.md`

### 策略概况

COD 方案更偏“区域/航点巡逻 + 低血回家”的结构。

单点导航版核心结构：

```text
Sequence
├── PubNav2Goal(main_position)
├── Delay 10s
└── KeepRunningUntilFailure
    └── ReactiveFallback
        ├── WriteToBlackboard
        ├── low hp go home
        ├── zone patrol
        ├── preload patrol
        └── AlwaysSuccess
```

低血分支：

```text
HpCondition
├── PubNav2Goal(home_position)
├── StayHome
└── PubNav2Goal(main_position)
```

其中：

- `HpCondition`: HP < 210 触发。
- `StayHome`: HP < 350 时返回 RUNNING，HP >= 350 才 SUCCESS。

多点导航版把回家和去增益点等路线放到 CSV：

```text
NavigateThroughPosesAction(waypoint_file=go_home.csv)
StayHome
NavigateThroughPosesAction(waypoint_file=go_gain.csv)
```

### 行为树写法特点

优点：

- 主循环也是先 `spin_some` 再 `tickOnce`，并且 10ms tick 一次，响应快。
- 位姿从 launch 参数或 CSV 读取，避免在 XML 中硬编码大量坐标。
- 巡逻方案非常清楚：
  - `LoadWaypoints` 从 CSV 加载航点。
  - `GetCurrentWaypoint` 取当前航点。
  - `PubNav2Goal` 按最小间隔发布目标点。
  - `WaitUntilReached` 用 TF 判断是否到达。
  - `WaitDuration` 到点等待。
  - `NextWaypoint` 循环切下一个点。
- 巡逻被补给打断后，`wp_idx` 可以保留，恢复后继续上次航点。
- `WaitUntilReached` 不依赖 Nav2 Action result，而是用 TF 到达半径判断，适合“只要到附近就算完成”的场景。
- 文档明确解释为什么用 `/goal_pose` topic 替代多层 Action：链路更简单，cancel/重发问题少。

风险：

- `PubNav2Goal` 话题式导航简单，但缺少 Action 结果语义。如果 Nav2 拒绝目标或规划失败，行为树只能靠 TF 等待，可能无限 RUNNING。
- `WriteToBlackboard` 构造函数里重新 `make_shared("WriteToBlackboard")`，没有复用传入的全局节点，结构上有点怪。
- 使用绝对路径 CSV，不适合直接部署到不同机器，需要改成 package share 路径或 launch 参数。

### 适合吸收的点

- 巡逻航点从 CSV/配置读取，而不是写死在 XML。
- 使用 `WaitUntilReached` 的 TF 半径判断，配合 `release_nav_on_close` 可以减少“到点附近但 Action 不结束”的问题。
- `StayHome` 这种“保持到恢复阈值”的条件很适合补给逻辑。
- 对重复发布目标做节流，例如 `min_pub_interval_ms=500`。

---

## 5. 三套方案的核心差异

| 方案 | 状态来源 | 优先级组织 | 补给写法 | 巡逻/导航写法 | 最值得吸收 |
|---|---|---|---|---|---|
| 当前 `rm_sentry_decision` | 各节点独立订阅，且话题不完全统一 | `ReactiveFallback` + `SequenceStar` | HP 低触发，先撤到第一进攻点再去补给 | `SendGoalAction` / `SendThroughGoalsAction` | 主体框架可用，但补给路线要改 |
| `chuanda` | `Topics2Blackboard` 统一写黑板 | `ReactiveSequence` + `ReactiveFallback` | `SequenceWithMemory`，低血/低弹直接补给 | `Nav2Pose` Action | 状态统一、XML 条件直观 |
| `rm_bt` | ROS topic 插件写黑板 | 大量 `SubTree` + `WhileDoElse` | 滞回补给，HP/弹量/刷新综合判断 | BehaviorTree.ROS2 `RosActionNode` | 补给滞回、子树拆分、插件化 |
| `COD_Behavior` | `WriteToBlackboard` 写黑板 | `ReactiveFallback` + `KeepRunningUntilFailure` | `HpCondition` + `StayHome` | CSV 航点、topic 发 goal、TF 到达判断 | CSV 巡逻、TF 到点、非阻塞等待 |

---

## 6. 对当前工程的推荐融合方案

### 推荐主树结构

```text
ReactiveSequence
├── UpdateBlackboard
├── GameStarted
└── ReactiveFallback
    ├── SupplyMode
    ├── EmergencyBaseDefense
    ├── AlliesDownBuffGuard
    ├── OutpostOrAttack
    └── PatrolLoop
```

### 推荐补给子树

补给子树应直接以补给点为最终目标：

```text
SupplyMode
├── NeedSupplyWithHysteresis
├── SwitchPoseAction(防御姿态/移动姿态)
├── SetNavSpeedAction
├── SendThroughGoalsAction(final_pose=补给点)
├── WaitUntilRecovered
└── OptionalReturnToFront
```

关键原则：

- 低血/低弹后不要先回第一进攻点。
- 中间点只能作为 through pose，最终点必须是补给点。
- `NeedSupplyWithHysteresis` 进入补给后要保持 SUCCESS，直到 HP 和弹量都恢复。
- 如果补给路线失败，不能无限卡在一个中间导航；应有 fallback 或超时策略。

### 推荐状态更新方式

吸收 `chuanda` 和 `rm_bt` 的方式：

```text
UpdateBlackboard
├── current_hp
├── bullet_allowance_17mm
├── game_progress
├── stage_remain_time
├── team_color
├── allies hp
└── base/outpost hp
```

建议统一到一个话题或一个 DataManager：

- 不要让 `GameStatusSub` 订 `/game_status`，`SelfHpBelow` 订 `/game_detection_status`。
- 如果两个话题都要支持，应该在 DataManager 里做兼容，而不是散落在多个条件节点里。
- 主循环改成：

```cpp
rclcpp::spin_some(node);
tree.tickRoot();
rate.sleep();
```

### 推荐巡逻方式

吸收 COD 的巡逻结构：

```text
LoadWaypoints
KeepRunningUntilFailure
└── Sequence
    ├── GetCurrentWaypoint
    ├── SendGoal / PubGoal
    ├── WaitUntilReached
    ├── WaitDuration
    └── NextWaypoint
```

如果继续使用 Action：

- `SendGoalAction` 应支持到点半径释放。
- `onHalted()` 必须可靠 cancel goal。
- 失败不要用无限 `RetryUntilSuccessful` 卡死整个高优先级分支。

如果使用 `/goal_pose` topic：

- 需要 `WaitUntilReached` 或 Nav2 状态监听，否则不知道目标是否失败。
- 发布目标要节流，例如 500ms。

---

## 7. 最重要的落地修改清单

1. 把激进树低血分支改成“直接回补给点”，不要先完成回第一进攻点。
2. 新增或改造 `NeedSupplyWithHysteresis`，同时判断 HP 和弹量。
3. 统一状态输入，优先做 `UpdateBlackboard/DataManager`。
4. 主循环改成先 `spin_some` 再 tick。
5. 去掉导航节点里 0.5 秒阻塞式 `/navigation_done` 发布，改成非阻塞或一次发布。
6. 巡逻点改为配置/CSV 管理，并保存当前巡逻索引。
7. 对 `RetryUntilSuccessful num_attempts=1000000` 谨慎使用，补给和回防分支应有超时或降级路径。

---

## 8. 一句话总结

最适合当前工程吸收的组合是：

```text
chuanda 的黑板状态更新
+ rm_bt 的补给滞回和子树拆分
+ COD 的 CSV 巡逻与 TF 到点判断
+ 当前工程已有的 SendThroughGoals 路线能力
```

但补给策略必须改成：低血/低弹触发后，最高优先级直接以补给点为最终目标，避免卡在“先回第一进攻点”的中间流程。
