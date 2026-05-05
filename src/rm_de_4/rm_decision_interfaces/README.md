 # 模拟测试指南

本包的消息在实赛由裁判系统/视觉发布，离线调试可用 `ros2 topic pub` 伪造。以下为常用话题示例，仅保留决策用到的最少字段。

---

## RobotStatus  `/robot_status`  (10 Hz)
```bash
ros2 topic pub -r 10 /robot_status rm_decision_interfaces/msg/RobotStatus '{ robot_id: 7, current_hp: 175, shooter_heat: 20, team_color: true }'
```

## GameStatus  `/game_status`  (1 Hz)
```bash
ros2 topic pub -r 1 /game_status rm_decision_interfaces/msg/GameStatus '{ game_progress: 3, stage_remain_time: 295 }'
```

## AllRobotHP  `/all_robot_hp`  (3 Hz)
```bash
ros2 topic pub -r 3 /all_robot_hp rm_decision_interfaces/msg/AllRobotHP '{ red_3_robot_hp: 100, blue_3_robot_hp: 75, red_outpost_hp: 0, blue_outpost_hp: 35, red_base_hp: 30, blue_base_hp: 25 }'
```

## BulletAllowance  `/bullet_allowance`  (3 Hz)
```bash
ros2 topic pub -r 3 /bullet_allowance rm_decision_interfaces/msg/BulletAllowance '{ bullet_allowance_17mm: 450 }'
```

## FriendLocation  `/friend_location`  (1 Hz)
```bash
ros2 topic pub -r 1 /friend_location rm_decision_interfaces/msg/FriendLocation '{ hero_x: 2.75, hero_y: 0.0 }'
```

## RFID  `/rfid`  (3 Hz)
```bash
ros2 topic pub -r 3 /rfid rm_decision_interfaces/msg/RFID '{ rfid_patrol_status: 0 }'
```

---

### Tips
1. 终端 A 运行决策节点，终端 B 循环发布测试数据。
2. 可写脚本或 `ros2 bag play` 一键回放。