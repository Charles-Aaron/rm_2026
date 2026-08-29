#!/usr/bin/env python3
"""
简单行为树测试脚本 - 与 SimVisualizationMove 联动
测试行为树能否正常发送移动指令

使用方法:
1. 启动决策节点: ros2 launch rm_sentry_decision rm_sentry_decision.launch.py
2. 运行此测试脚本: python3 test_conservative_bt_panel.py

行为树执行序列:
1. 前进 3 秒 (linear.x=0.5) - 向前移动
2. 左移 3 秒 (linear.y=0.5) - 向左移动
3. 后退 3 秒 (linear.x=-0.5) - 向后移动
4. 右移 3 秒 (linear.y=-0.5) - 向右移动
5. 左转 3 秒 (angular.z=0.3) - 原地左转
6. 右转 3 秒 (angular.z=-0.3) - 原地右转
7. 停止 1 秒
"""

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from rm_decision_interfaces.msg import GameStatus, RobotStatus, AllRobotHP, BulletAllowance


class ConservativeBtPanelTester(Node):
    def __init__(self):
        super().__init__('conservative_bt_panel_tester')
        
        qos = 10
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.robot_pub = self.create_publisher(RobotStatus, "/robot_status", qos)
        self.hp_pub = self.create_publisher(AllRobotHP, "/all_robot_hp", qos)
        self.bullet_pub = self.create_publisher(BulletAllowance, "/bullet_allowance", qos)
        
        self.timer = self.create_timer(0.2, self.publish_messages)
        
        self.get_logger().info("=" * 70)
        self.get_logger().info("简单行为树测试 - SimVisualizationMove")
        self.get_logger().info("=" * 70)
        self.get_logger().info("")
        self.get_logger().info("行为树执行序列:")
        self.get_logger().info("  1. 前进 3 秒 (linear.x=0.5) - 向前移动")
        self.get_logger().info("  2. 左移 3 秒 (linear.y=0.5) - 向左移动")
        self.get_logger().info("  3. 后退 3 秒 (linear.x=-0.5) - 向后移动")
        self.get_logger().info("  4. 右移 3 秒 (linear.y=-0.5) - 向右移动")
        self.get_logger().info("  5. 左转 3 秒 (angular.z=0.3) - 原地左转")
        self.get_logger().info("  6. 右转 3 秒 (angular.z=-0.3) - 原地右转")
        self.get_logger().info("  7. 停止 1 秒")
        self.get_logger().info("")
        self.get_logger().info("总时长: 约 19 秒")
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
        game_msg = self.get_game_msg()
        robot_msg = self.get_robot_msg()
        hp_msg = self.get_hp_msg()
        bullet_msg = self.get_bullet_msg()
        
        self.game_pub.publish(game_msg)
        self.robot_pub.publish(robot_msg)
        self.hp_pub.publish(hp_msg)
        self.bullet_pub.publish(bullet_msg)


def main():
    rclpy.init()
    node = ConservativeBtPanelTester()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        print("\n✋ 用户停止")
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
