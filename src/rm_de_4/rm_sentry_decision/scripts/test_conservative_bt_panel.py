#!/usr/bin/env python3
"""
保守版行为树测试脚本 - 测试所有6个优先级
循环测试每个优先级场景
"""

import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from rm_decision_interfaces.msg import GameStatus, RobotStatus, AllRobotHP, BulletAllowance


class ConservativeBtPanelTester(Node):
    def __init__(self):
        super().__init__('conservative_bt_panel_tester')
        
        qos = QoSProfile(depth=10)
        
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.game_detection_pub = self.create_publisher(GameStatus, "/game_detection_status", qos)
        self.robot_pub = self.create_publisher(RobotStatus, "/robot_status", qos)
        self.hp_pub = self.create_publisher(AllRobotHP, "/all_robot_hp", qos)
        self.bullet_pub = self.create_publisher(BulletAllowance, "/bullet_allowance", qos)
        
        self.timer = self.create_timer(0.1, self.publish_messages)
        self.stage_timer = self.create_timer(5.0, self.next_stage)
        self.status_timer = self.create_timer(1.0, self.print_status)
        
        self.current_stage = 0
        self.count = 0
        
        self.stages = [
            {
                "name": "优先级1: 攻击前哨站",
                "condition": "敌方前哨站存活 + 机器人状态正常",
                "action": "前进 (attack)",
                "game": {"game_progress": 4, "blue_outpost_hp": 1500, "red_base_hp": 3000},
                "hp": {"red_1_robot_hp": 400, "red_3_robot_hp": 400, "red_4_robot_hp": 400},
            },
            {
                "name": "优先级2: 英雄保护点",
                "condition": "HeroProtectAttack + 机器人状态正常",
                "action": "后退 (defense)",
                "game": {"game_progress": 4, "blue_outpost_hp": 0, "red_base_hp": 2000},
                "hp": {"red_1_robot_hp": 400, "red_3_robot_hp": 400, "red_4_robot_hp": 400},
            },
            {
                "name": "优先级3: 智能补给",
                "condition": "NeedSupply (血量低)",
                "action": "左移 (patrol)",
                "game": {"game_progress": 4, "blue_outpost_hp": 0, "red_base_hp": 3000},
                "hp": {"red_1_robot_hp": 400, "red_3_robot_hp": 400, "red_4_robot_hp": 400},
                "robot": {"current_hp": 80},
            },
            {
                "name": "优先级4: 终局策略",
                "condition": "RecoveredEnough (终局)",
                "action": "左移 (patrol)",
                "game": {"game_progress": 4, "stage_remain_time": 100, "blue_outpost_hp": 0, "red_base_hp": 3000},
                "hp": {"red_1_robot_hp": 400, "red_3_robot_hp": 400, "red_4_robot_hp": 400},
            },
            {
                "name": "优先级5: 守卫基地",
                "condition": "AlliesDown (队友阵亡)",
                "action": "后退 (defense)",
                "game": {"game_progress": 4, "blue_outpost_hp": 0, "red_base_hp": 3000},
                "hp": {"red_1_robot_hp": 0, "red_3_robot_hp": 0, "red_4_robot_hp": 0},
            },
            {
                "name": "优先级6: 巡逻",
                "condition": "默认 (无特殊条件)",
                "action": "左移 (patrol)",
                "game": {"game_progress": 4, "blue_outpost_hp": 0, "red_base_hp": 3000},
                "hp": {"red_1_robot_hp": 400, "red_3_robot_hp": 400, "red_4_robot_hp": 400},
            },
        ]
        
        self.get_logger().info("=" * 70)
        self.get_logger().info("保守版行为树测试 - 循环测试6个优先级")
        self.get_logger().info("=" * 70)
        self.get_logger().info("")
        self.get_logger().info("每个场景测试5秒，自动切换")
        self.get_logger().info("=" * 70)
        
    def get_game_msg(self):
        msg = GameStatus()
        stage = self.stages[self.current_stage]
        
        msg.game_progress = stage["game"].get("game_progress", 4)
        msg.stage_remain_time = stage["game"].get("stage_remain_time", 180)
        msg.team_color = False
        msg.current_hp = stage.get("robot", {}).get("current_hp", 400)
        msg.bullet_allowance_17mm = 500
        msg.red_outpost_hp = 1500
        msg.blue_outpost_hp = stage["game"].get("blue_outpost_hp", 1500)
        msg.red_base_hp = stage["game"].get("red_base_hp", 3000)
        msg.blue_base_hp = 3000
        msg.red_1_robot_hp = stage["hp"].get("red_1_robot_hp", 400)
        msg.red_2_robot_hp = 400
        msg.red_3_robot_hp = stage["hp"].get("red_3_robot_hp", 400)
        msg.red_4_robot_hp = stage["hp"].get("red_4_robot_hp", 400)
        msg.blue_1_robot_hp = 400
        msg.blue_2_robot_hp = 400
        msg.blue_3_robot_hp = 400
        msg.blue_4_robot_hp = 400
        return msg
    
    def get_robot_msg(self):
        msg = RobotStatus()
        stage = self.stages[self.current_stage]
        msg.current_hp = stage.get("robot", {}).get("current_hp", 400)
        msg.team_color = False
        return msg
    
    def get_hp_msg(self):
        msg = AllRobotHP()
        stage = self.stages[self.current_stage]
        msg.red_outpost_hp = 1500
        msg.blue_outpost_hp = stage["game"].get("blue_outpost_hp", 1500)
        msg.red_base_hp = stage["game"].get("red_base_hp", 3000)
        msg.blue_base_hp = 3000
        msg.red_1_robot_hp = stage["hp"].get("red_1_robot_hp", 400)
        msg.red_2_robot_hp = 400
        msg.red_3_robot_hp = stage["hp"].get("red_3_robot_hp", 400)
        msg.red_4_robot_hp = stage["hp"].get("red_4_robot_hp", 400)
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
        self.count += 1
        
        game_msg = self.get_game_msg()
        robot_msg = self.get_robot_msg()
        hp_msg = self.get_hp_msg()
        bullet_msg = self.get_bullet_msg()
        
        self.game_pub.publish(game_msg)
        self.game_detection_pub.publish(game_msg)
        self.robot_pub.publish(robot_msg)
        self.hp_pub.publish(hp_msg)
        self.bullet_pub.publish(bullet_msg)
    
    def next_stage(self):
        self.current_stage = (self.current_stage + 1) % len(self.stages)
        stage = self.stages[self.current_stage]
        self.get_logger().info("")
        self.get_logger().info("=" * 50)
        self.get_logger().info(f"切换到: {stage['name']}")
        self.get_logger().info("=" * 50)
    
    def print_status(self):
        stage = self.stages[self.current_stage]
        self.get_logger().info("")
        self.get_logger().info("=" * 50)
        self.get_logger().info(f"当前测试: {stage['name']}")
        self.get_logger().info("=" * 50)
        self.get_logger().info(f"  条件: {stage['condition']}")
        self.get_logger().info(f"  预期动作: {stage['action']}")
        self.get_logger().info("-" * 50)
        self.get_logger().info(f"  game_progress: {stage['game'].get('game_progress', 4)}")
        self.get_logger().info(f"  敌方前哨站HP: {stage['game'].get('blue_outpost_hp', 1500)}")
        self.get_logger().info(f"  己方基地HP: {stage['game'].get('red_base_hp', 3000)}")
        self.get_logger().info(f"  队友1/3/4号HP: {stage['hp'].get('red_1_robot_hp', 400)}/{stage['hp'].get('red_3_robot_hp', 400)}/{stage['hp'].get('red_4_robot_hp', 400)}")
        self.get_logger().info("=" * 50)


def main():
    rclpy.init()
    node = ConservativeBtPanelTester()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print("\n用户停止")
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()

