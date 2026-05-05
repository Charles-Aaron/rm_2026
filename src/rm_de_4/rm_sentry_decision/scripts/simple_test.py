#!/usr/bin/env python3
"""
行为树完整测试脚本
测试所有6个优先级场景，每个场景持续10秒

场景列表：
1. 攻击前哨站 - 敌方前哨站存活，我方状态良好
2. 英雄保护 - 敌方前哨站已毁，英雄血量低
3. 智能补给 - 我方血量/弹药不足
4. 终局策略 - 补给完成，继续行动
5. 巡逻 - 无特殊条件，保底巡逻
6. 守卫基地 - 己方输出机全部阵亡
"""

import rclpy
from rclpy.node import Node
from rm_decision_interfaces.msg import GameStatus

class BtTester(Node):
    def __init__(self):
        super().__init__('bt_tester')
        
        qos = 10
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.game_detection_pub = self.create_publisher(GameStatus, "/game_detection_status", qos)
        
        self.timer = self.create_timer(0.2, self.publish_messages)
        self.stage = 0
        self.last_stage_time = self.get_clock().now()
        
        self.get_logger().info("=" * 70)
        self.get_logger().info("BT Full Tester - 6 stages, 10 seconds each")
        self.get_logger().info("=" * 70)
        
    def get_base_msg(self):
        """创建基础消息，设置默认值"""
        msg = GameStatus()
        msg.game_progress = 4
        msg.stage_remain_time = 300
        msg.team_color = False  # Red team
        
        msg.current_hp = 300
        msg.bullet_allowance_17mm = 150
        msg.red_base_hp = 3000
        msg.blue_base_hp = 3000
        msg.red_1_robot_hp = 400
        msg.red_2_robot_hp = 400
        msg.red_3_robot_hp = 400
        msg.red_4_robot_hp = 400
        msg.blue_1_robot_hp = 400
        msg.blue_outpost_hp = 0
        msg.red_outpost_hp = 1500
        
        return msg
        
    def publish_messages(self):
        now = self.get_clock().now()
        elapsed = (now - self.last_stage_time).nanoseconds / 1e9
        
        msg = self.get_base_msg()
        
        if self.stage == 0:
            self.get_logger().info(
                "🎯 Stage 1: ATTACK OUTPOST\n"
                "   Condition: OutpostAttack + RobotStatusSub\n"
                "   - Enemy Outpost: 1500 (ALIVE)\n"
                "   - My HP: 300, Bullets: 150 (OK)\n"
                "   Expected: 进攻姿态"
            )
            msg.blue_outpost_hp = 1500
            
        elif self.stage == 1:
            self.get_logger().info(
                "🛡️ Stage 2: HERO PROTECT\n"
                "   Condition: HeroProtectAttack + RobotStatusSub\n"
                "   - Enemy Outpost: 0 (DESTROYED)\n"
                "   - Hero HP: 100 (LOW!)\n"
                "   - My HP: 300, Bullets: 150 (OK)\n"
                "   Expected: 防御姿态"
            )
            msg.blue_outpost_hp = 0
            msg.red_1_robot_hp = 100
            
        elif self.stage == 2:
            self.get_logger().info(
                "🔋 Stage 3: SMART SUPPLY\n"
                "   Condition: NeedSupply\n"
                "   - My HP: 100 (LOW!)\n"
                "   - My Bullets: 30 (LOW!)\n"
                "   Expected: 移动姿态 (补给模式)"
            )
            msg.current_hp = 100
            msg.bullet_allowance_17mm = 30
            msg.blue_outpost_hp = 0
            
        elif self.stage == 3:
            self.get_logger().info(
                "✅ Stage 4: ENDGAME STRATEGY\n"
                "   Condition: RecoveredEnough\n"
                "   - My HP: 400 (RECOVERED)\n"
                "   - My Bullets: 400 (RECOVERED)\n"
                "   Expected: 移动姿态"
            )
            msg.current_hp = 400
            msg.bullet_allowance_17mm = 400
            msg.blue_outpost_hp = 0
            
        elif self.stage == 4:
            self.get_logger().info(
                "🚶 Stage 5: PATROL (Fallback)\n"
                "   Condition: None (保底)\n"
                "   - All conditions normal\n"
                "   - Allies alive (NOT down)\n"
                "   Expected: 移动姿态 (巡逻)"
            )
            msg.blue_outpost_hp = 0
            msg.red_1_robot_hp = 400
            msg.red_3_robot_hp = 400
            msg.red_4_robot_hp = 400
            
        elif self.stage == 5:
            self.get_logger().info(
                "⚔️ Stage 6: FORTRESS BUFF (Allies Down)\n"
                "   Condition: AlliesDown\n"
                "   - Red 1/3/4 Robot HP: 0 (ALL DEAD!)\n"
                "   Expected: 防御姿态 + 守卫基地增益点"
            )
            msg.blue_outpost_hp = 0
            msg.red_1_robot_hp = 0
            msg.red_3_robot_hp = 0
            msg.red_4_robot_hp = 0
        
        self.game_pub.publish(msg)
        self.game_detection_pub.publish(msg)
        
        if elapsed > 10.0:
            self.stage = (self.stage + 1) % 6
            self.last_stage_time = now
            self.get_logger().info("=" * 70)

def main():
    rclpy.init()
    node = BtTester()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print("\n✋ Stopped by user")
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
