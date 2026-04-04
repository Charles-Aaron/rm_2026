# 模拟测试指南

本包现在是 `rm_2026` 工作区内唯一的哨兵决策接口包，消息定义兼容旧版
`nav_ros2` 与新版 `rm_sentry_decision`。

## 常用话题示例

### RobotStatus `/robot_status` (10 Hz)
```bash
ros2 topic pub -r 10 /robot_status rm_decision_interfaces/msg/RobotStatus \
  '{robot_id: 7, current_hp: 175, shooter_heat: 20, team_color: true, is_attacked: false, is_detect_enemy: false, bullet_allowance_17mm: 450}'
```

### GameStatus `/game_status` (1 Hz)
```bash
ros2 topic pub -r 1 /game_status rm_decision_interfaces/msg/GameStatus \
  '{game_progress: 3, stage_remain_time: 295, team_color: true, bullet_allowance_17mm: 450, current_hp: 175}'
```

### AllRobotHP `/all_robot_hp` (3 Hz)
```bash
ros2 topic pub -r 3 /all_robot_hp rm_decision_interfaces/msg/AllRobotHP \
  '{red_3_robot_hp: 100, blue_3_robot_hp: 75, red_outpost_hp: 0, blue_outpost_hp: 35, red_base_hp: 30, blue_base_hp: 25}'
```

### BulletAllowance `/bullet_allowance` (3 Hz)
```bash
ros2 topic pub -r 3 /bullet_allowance rm_decision_interfaces/msg/BulletAllowance \
  '{bullet_allowance_17mm: 450, bullet_allowance_42mm: 0}'
```

### FriendLocation `/friend_location` (1 Hz)
```bash
ros2 topic pub -r 1 /friend_location rm_decision_interfaces/msg/FriendLocation \
  '{hero_x: 2.75, hero_y: 0.0}'
```

### RFID `/rfid` (3 Hz)
```bash
ros2 topic pub -r 3 /rfid rm_decision_interfaces/msg/RFID \
  '{rfid_patrol_status: 0}'
```

### SentryPoseCommand `/sentry/pose_command`
```bash
ros2 topic pub --once /sentry/pose_command rm_decision_interfaces/msg/SentryPoseCommand \
  '{pose_type: 1}'
```

### Tips
1. 终端 A 运行决策节点，终端 B 循环发布测试数据。
2. 修改消息定义后，需要重新编译整个工作区中的相关包。
