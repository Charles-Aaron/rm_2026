#!/usr/bin/env python3
"""
激进版行为树测试脚本
测试所有4个优先级场景，每个场景持续10秒

场景列表：
1. 进攻循环 - 正常状态，第一进攻点↔第二进攻点
2. 低血撤退补给 - 血量<110触发撤退补给
3. 队友阵亡守卫增益点 - 1/3/4号机器人全部阵亡
4. 终局回防 - 比赛剩余时间<150秒
"""

import rclpy
from rclpy.node import Node
from rm_decision_interfaces.msg import GameStatus, RobotStatus, AllRobotHP, BulletAllowance


class AggressiveBtTester(Node):
    def __init__(self):
        super().__init__('aggressive_bt_tester')
        
        qos = 10
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.robot_pub = self.create_publisher(RobotStatus, "/robot_status", qos)
        self.hp_pub = self.create_publisher(AllRobotHP, "/all_robot_hp", qos)
        self.bullet_pub = self.create_publisher(BulletAllowance, "/bullet_allowance", qos)
        
        self.timer = self.create_timer(0.2, self.publish_messages)
        self.stage = 0
        self.last_stage_time = self.get_clock().now()
        
        self.get_logger().info("=" * 70)
        self.get_logger().info("激进版行为树测试 - 4个场景，每个10秒")
        self.get_logger().info("=" * 70)
        
    def get_game_msg(self):
        msg = GameStatus()
        msg.game_progress = 4
        msg.stage_remain_time = 180
        return msg
    
    def get_robot_msg(self):
        msg = RobotStatus()
        msg.current_hp = 400
        msg.team_color = False
        return msg
    
    def get_hp_msg(self):
        msg = AllRobotHP()
        msg.red_outpost_hp = 1500
        msg.blue_outpost_hp = 1500
        msg.red_base_hp = 3000
        msg.blue_base_hp = 3000
        msg.red_1_robot_hp = 400
        msg.red_2_robot_hp = 400
        msg.red_3_robot_hp = 400
        msg.red_4_robot_hp = 400
        msg.red_5_robot_hp = 400
        msg.red_7_robot_hp = 400
        msg.blue_1_robot_hp = 400
        msg.blue_2_robot_hp = 400
        msg.blue_3_robot_hp = 400
        msg.blue_4_robot_hp = 400
        msg.blue_5_robot_hp = 400
        msg.blue_7_robot_hp = 400
        return msg
    
    def get_bullet_msg(self):
        msg = BulletAllowance()
        msg.bullet_allowance_17mm = 500
        msg.bullet_allowance_42mm = 100
        return msg
        
    def publish_messages(self):
        now = self.get_clock().now()
        elapsed = (now - self.last_stage_time).nanoseconds / 1e9
        
        game_msg = self.get_game_msg()
        robot_msg = self.get_robot_msg()
        hp_msg = self.get_hp_msg()
        bullet_msg = self.get_bullet_msg()
        
        if self.stage == 0:
            self.get_logger().info(
                "⚔️ Stage 1: 进攻循环\n"
                "   条件: 无特殊条件\n"
                "   - 血量: 400 (正常)\n"
                "   - 队友: 全部存活\n"
                "   - 剩余时间: 180秒\n"
                "   预期行为:\n"
                "     1. 移动姿态前往第二进攻点 (8.5, 2.5)\n"
                "     2. 进攻姿态活动\n"
                "     3. 防御姿态返回第一进攻点 (5.27, -3.07)"
            )
            
        elif self.stage == 1:
            self.get_logger().info(
                "🩸 Stage 2: 低血撤退补给\n"
                "   条件: SelfHpBelow(110)\n"
                "   - 血量: 100 (低于110!)\n"
                "   预期行为:\n"
                "     1. 防御姿态\n"
                "     2. 经途经点1(6.0,0.0)和途经点2(4.5,-2.0)返回第一进攻点\n"
                "     3. 前往补给点 (0.0, 0.0)"
            )
            robot_msg.current_hp = 100
            
        elif self.stage == 2:
            self.get_logger().info(
                "🛡️ Stage 3: 队友阵亡守卫增益点\n"
                "   条件: AlliesDown\n"
                "   - 红方1号: 0 (阵亡)\n"
                "   - 红方3号: 0 (阵亡)\n"
                "   - 红方4号: 0 (阵亡)\n"
                "   预期行为:\n"
                "     1. 防御姿态\n"
                "     2. 前往增益点 (1.0, -0.5) 守卫60秒"
            )
            hp_msg.red_1_robot_hp = 0
            hp_msg.red_3_robot_hp = 0
            hp_msg.red_4_robot_hp = 0
            
        elif self.stage == 3:
            self.get_logger().info(
                "🏠 Stage 4: 终局回防守点\n"
                "   条件: LateGameDecision\n"
                "   - 剩余时间: 120秒 (低于150秒)\n"
                "   预期行为:\n"
                "     1. 防御姿态\n"
                "     2. 前往防守点/基地 (-1.0, 2.0)"
            )
            game_msg.stage_remain_time = 120
        
        self.game_pub.publish(game_msg)
        self.robot_pub.publish(robot_msg)
        self.hp_pub.publish(hp_msg)
        self.bullet_pub.publish(bullet_msg)
        
        if elapsed > 10.0:
            self.stage = (self.stage + 1) % 4
            self.last_stage_time = now
            self.get_logger().info("=" * 70)


def main():
    rclpy.init()
    node = AggressiveBtTester()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print("\n✋ 用户停止")
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
