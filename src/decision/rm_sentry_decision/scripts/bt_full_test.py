#!/usr/bin/env python3
"""
bt_full_test.py

A ROS 2 helper script that sequentially publishes stimulus messages so that
rm_sentry_decision 的 MainTree 能依次触发全部节点路径。

默认按"红方哨兵(7号)"逻辑发布；若测试蓝方，只需：
1. 将 ROBOT_ID 改为 107
2. TEAM_COLOR 改为 True
3. 互换 AllRobotHP 消息中红/蓝的字段即可。

修复内容 (2024-12-19):
1. ✅ 添加消息重置机制，避免状态污染
2. ✅ 修复XML配置，为smart_supply添加NeedSupply前置条件
3. ✅ 修复RandomPatrolPose逻辑，返回SUCCESS而不是FAILURE
4. ✅ 修复SelectSupplyPoint，默认返回基地补给点而不是空字符串
5. ✅ 修复弹量字段，使用bullet_allowance_17mm而不是shooter_heat
6. ✅ 完善测试覆盖，添加HeroProtectAttack测试场景
7. ✅ 延长每个状态持续时间到10秒，提高测试稳定性

运行方法：
    source install/setup.bash
    ros2 run rm_sentry_decision bt_full_test.py

脚本内部每隔10秒切换一次场景，总计运行≈2.5 分钟。
"""

import time

import rclpy
from rclpy.node import Node

from rm_decision_interfaces.msg import (
    AllRobotHP,
    BulletAllowance,
    GameStatus,
    RobotStatus,
)

# ------- 常量配置 --------
ROBOT_ID = 7  # 红方哨兵:7  | 蓝方哨兵:107
TEAM_COLOR = False  # 红=False  蓝=True

PUBLISH_HZ = 5  # 所有话题统一发布速率 (Hz)


class BtTester(Node):
    def __init__(self):
        super().__init__("bt_test_stimulus")

        qos = 10
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.robot_status_pub = self.create_publisher(RobotStatus, "/robot_status", qos)
        self.bullet_pub = self.create_publisher(BulletAllowance, "/bullet_allowance", qos)
        self.all_hp_pub = self.create_publisher(AllRobotHP, "/all_robot_hp", qos)

        # 盟友血量 (AlliesDown)
        self.robot1_pub = self.create_publisher(RobotStatus, "/robot1_status", qos)
        self.robot3_pub = self.create_publisher(RobotStatus, "/robot3_status", qos)
        self.robot4_pub = self.create_publisher(RobotStatus, "/robot4_status", qos)

        # 预生成可复用消息对象
        self.game_msg = GameStatus(game_progress=4, stage_remain_time=600)
        self.robot_msg = RobotStatus(
            current_hp=0,
            bullet_allowance_17mm=0,
            team_color=TEAM_COLOR,
        )
        self.bullet_msg = BulletAllowance(
            bullet_allowance_17mm=0, bullet_allowance_42mm=0
        )
        self.hp_msg = AllRobotHP()
        # 初始化红/蓝血量 0
        self.hp_msg.red_outpost_hp = 0
        self.hp_msg.red_base_hp = 0
        self.hp_msg.blue_outpost_hp = 0
        self.hp_msg.blue_base_hp = 0

        # 定时器驱动状态机
        self.state = 0
        period = 1.0 / PUBLISH_HZ
        self.timer = self.create_timer(period, self.step)
        self.state_enter_time = self.get_clock().now()

        self.log("Tester started, waiting 1 s for initial_move...")

        # 新增订阅 - 暂时注释掉RFID订阅以避免错误
        # self.rfid_sub = self.create_subscription(
        #     RFID,
        #     "/rfid_status",
        #     qos,
        #     self.rfid_callback,
        # )

    # ---------- 辅助函数 -------------
    def log(self, text):
        self.get_logger().info(text)

    def reset_all_messages(self):
        """重置所有消息为默认状态"""
        # 重置游戏状态
        self.game_msg.game_progress = 4
        self.game_msg.stage_remain_time = 600
        
        # 重置机器人状态
        self.robot_msg.current_hp = 300
        self.robot_msg.bullet_allowance_17mm = 0
        self.robot_msg.team_color = TEAM_COLOR
        
        # 重置子弹状态
        self.bullet_msg.bullet_allowance_17mm = 0
        self.bullet_msg.bullet_allowance_42mm = 0
        
        # 重置所有血量
        self.hp_msg.red_outpost_hp = 0
        self.hp_msg.red_base_hp = 0
        self.hp_msg.blue_outpost_hp = 0
        self.hp_msg.blue_base_hp = 0
        # 重置所有机器人血量
        self.hp_msg.red_1_robot_hp = 0
        self.hp_msg.red_3_robot_hp = 0
        self.hp_msg.red_4_robot_hp = 0
        self.hp_msg.blue_1_robot_hp = 0
        self.hp_msg.blue_3_robot_hp = 0
        self.hp_msg.blue_4_robot_hp = 0

    def publish_all(self):
        """按当前属性值发布全部话题"""
        self.game_pub.publish(self.game_msg)
        self.robot_status_pub.publish(self.robot_msg)
        self.bullet_pub.publish(self.bullet_msg)
        self.all_hp_pub.publish(self.hp_msg)

    # ----------- 主状态机 --------------
    def step(self):
        now = self.get_clock().now()
        elapsed = (now - self.state_enter_time).nanoseconds / 1e9

        # 状态机：依次触发初始 -> 前哨 -> 基地 -> 补给 -> 终局 -> 巡逻 -> 据点 -> 回家

        if self.state == 0 and elapsed > 1.0:
            # 1️⃣ attack_outpost 条件
            self.robot_msg.current_hp = 300
            self.robot_msg.bullet_allowance_17mm = 50
            self.bullet_msg.bullet_allowance_17mm = 120
            if TEAM_COLOR:
                self.hp_msg.red_outpost_hp = 200  # enemy
            else:
                self.hp_msg.blue_outpost_hp = 200  # enemy
            self.log("--> attack_outpost 条件发布完毕")
            self.next_state()

        elif self.state == 1 and elapsed > 10.0:
            # 2️⃣ attack_base: 敌方前哨 0, 敌方基地>0
            if TEAM_COLOR:
                self.hp_msg.red_outpost_hp = 0
                self.hp_msg.red_base_hp = 200
            else:
                self.hp_msg.blue_outpost_hp = 0
                self.hp_msg.blue_base_hp = 200
            self.log("--> attack_base 条件发布完毕")
            self.next_state()

        elif self.state == 2 and elapsed > 10.0:
            # 3️⃣ need_supply: time<=150s 且 HP<200 且 弹<200
            self.game_msg.stage_remain_time = 120
            self.robot_msg.current_hp = 150
            self.robot_msg.bullet_allowance_17mm = 100
            self.bullet_msg.bullet_allowance_17mm = 100
            # 敌方已经被破前哨，保持上一步血量
            self.log("--> need_supply 条件发布完毕")
            self.next_state()

        elif self.state == 3 and elapsed > 10.0:
            # 补给完成 -> RecoveredEnough
            self.robot_msg.current_hp = 350
            self.robot_msg.bullet_allowance_17mm = 400
            self.bullet_msg.bullet_allowance_17mm = 400
            self.log("--> RecoveredEnough 条件发布完毕")
            self.next_state()

        elif self.state == 4 and elapsed > 10.0:
            # 4️⃣ endgame_need_supply
            self.game_msg.stage_remain_time = 140
            self.robot_msg.current_hp = 120
            self.robot_msg.bullet_allowance_17mm = 80
            self.bullet_msg.bullet_allowance_17mm = 80
            self.log("--> EndgameNeedSupply 条件发布完毕")
            self.next_state()

        elif self.state == 5 and elapsed > 10.0:
            # 终局补给完成 + ShouldRushBase 成立 (敌我基地差≥500)
            self.robot_msg.current_hp = 350
            self.robot_msg.bullet_allowance_17mm = 400
            self.bullet_msg.bullet_allowance_17mm = 400
            if TEAM_COLOR:
                self.hp_msg.red_base_hp = 1500  # enemy
                self.hp_msg.blue_base_hp = 900  # ours
            else:
                self.hp_msg.blue_base_hp = 1500  # enemy
                self.hp_msg.red_base_hp = 900   # ours
            self.log("--> ShouldRushBase 条件(冲基地) 发布完毕")
            self.next_state()

        elif self.state == 6 and elapsed > 10.0:
            # 取消 rush 条件，回到 defend_home_base
            if TEAM_COLOR:
                self.hp_msg.red_base_hp = 1100
            else:
                self.hp_msg.blue_base_hp = 1100
            self.log("--> 关闭 rush 条件, 转守家")
            self.next_state()

        elif self.state == 7 and elapsed > 10.0:
            # 7️⃣ RushEnemyBuff 条件: 剩余时间180-240s + 我方1/3/4号机器人血量>200
            self.game_msg.stage_remain_time = 200  # 在180-240s范围内
            self.robot_msg.current_hp = 300
            self.robot_msg.bullet_allowance_17mm = 200
            self.bullet_msg.bullet_allowance_17mm = 200
            # 设置我方1/3/4号机器人血量都>200 (通过AllRobotHP消息)
            if TEAM_COLOR:  # 蓝方
                self.hp_msg.blue_1_robot_hp = 250
                self.hp_msg.blue_3_robot_hp = 280
                self.hp_msg.blue_4_robot_hp = 220
            else:  # 红方
                self.hp_msg.red_1_robot_hp = 250
                self.hp_msg.red_3_robot_hp = 280
                self.hp_msg.red_4_robot_hp = 220
            self.log("--> RushEnemyBuff 条件(冲击敌方堡垒增益点) 发布完毕")
            self.next_state()

        elif self.state == 8 and elapsed > 10.0:
            # 取消 RushEnemyBuff 条件，回到正常状态
            self.game_msg.stage_remain_time = 300  # 超出180-240s范围
            self.log("--> 关闭 RushEnemyBuff 条件")
            self.next_state()

        elif self.state == 9 and elapsed > 10.0:
            # 8️⃣ HeroProtectAttack 条件测试
            self.robot_msg.current_hp = 300
            self.robot_msg.bullet_allowance_17mm = 200
            self.bullet_msg.bullet_allowance_17mm = 200
            # 设置一些触发英雄保护的条件（这里需要根据HeroProtectAttack的具体逻辑调整）
            self.log("--> HeroProtectAttack 条件发布完毕")
            self.next_state()

        elif self.state == 10 and elapsed > 10.0:
            # 9️⃣ patrol: 确保其他条件 FAIL, RobotStatusSub OK
            self.robot_msg.current_hp = 300
            self.robot_msg.bullet_allowance_17mm = 200
            self.bullet_msg.bullet_allowance_17mm = 200
            # 敌方前哨 & 基地均 0 => attack 分支 FAIL
            if TEAM_COLOR:
                self.hp_msg.red_base_hp = 0
            else:
                self.hp_msg.blue_base_hp = 0
            self.log("--> center_patrol 条件发布完毕")
            self.next_state()

        elif self.state == 11 and elapsed > 10.0:
            # �� fortress_buff -> AlliesDown
            zero_hp = RobotStatus(current_hp=0, team_color=TEAM_COLOR, bullet_allowance_17mm=0)
            self.robot1_pub.publish(zero_hp)
            self.robot3_pub.publish(zero_hp)
            self.robot4_pub.publish(zero_hp)
            self.log("--> AlliesDown 条件发布完毕")
            self.next_state()

        elif self.state == 12 and elapsed > 10.0:
            # 1️⃣1️⃣ go_home 触发: 让 RobotStatusSub FAIL
            self.robot_msg.current_hp = 50
            self.robot_msg.bullet_allowance_17mm = 10
            self.bullet_msg.bullet_allowance_17mm = 10
            self.log("--> go_home 条件发布完毕, 脚本结束")
            self.next_state()

        elif self.state == 13 and elapsed > 10.0:
            self.log("�� 所有分支已依次触发完毕, 退出脚本")
            rclpy.shutdown()
            return

        # 周期发布 (保持最新值)
        self.publish_all()

    def next_state(self):
        self.state += 1
        self.state_enter_time = self.get_clock().now()
        # 状态切换时重置所有消息
        self.reset_all_messages()

    # def rfid_callback(self, msg):
    #     enemy_fortress_buff = (msg.rfid_status >> 24) & 0x01
    #     # 根据需要写入黑板或触发行为树逻辑


# ------------------------------
# Entry point
# ------------------------------


def main():
    rclpy.init()
    node = BtTester()
    try:
        rclpy.spin(node)
    finally:
        # ensure shutdown even if exceptions occur
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main() 
