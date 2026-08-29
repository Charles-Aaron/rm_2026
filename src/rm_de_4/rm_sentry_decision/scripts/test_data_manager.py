#!/usr/bin/env python3
"""
DataManager 功能测试脚本

测试共享数据管理器的功能和性能优化效果
"""

import rclpy
from rclpy.node import Node
from rm_decision_interfaces.msg import GameStatus, RobotStatus, AllRobotHP, BulletAllowance
import time
import psutil
import os

class DataManagerTester(Node):
    def __init__(self):
        super().__init__('data_manager_tester')
        
        # 创建发布器来模拟数据源
        self.game_status_pub = self.create_publisher(GameStatus, '/game_status', 10)
        self.robot_status_pub = self.create_publisher(RobotStatus, '/robot_status', 10)
        self.all_robot_hp_pub = self.create_publisher(AllRobotHP, '/all_robot_hp', 10)
        self.bullet_allowance_pub = self.create_publisher(BulletAllowance, '/bullet_allowance', 10)
        
        # 创建定时器来定期发布测试数据
        self.timer = self.create_timer(0.1, self.publish_test_data)  # 10Hz
        
        # 记录开始时间
        self.start_time = time.time()
        self.message_count = 0
        
        self.get_logger().info("DataManager测试器已启动")
    
    def publish_test_data(self):
        """发布测试数据"""
        current_time = time.time()
        
        # 创建GameStatus消息
        game_status = GameStatus()
        game_status.current_hp = 800
        game_status.stage_remain_time = 300
        game_status.game_progress = 1
        game_status.team_color = True  # 蓝队
        game_status.bullet_allowance_17mm = 500
        game_status.red_outpost_hp = 0
        game_status.blue_outpost_hp = 1500
        game_status.red_base_hp = 1500
        game_status.blue_base_hp = 1500
        game_status.red_1_robot_hp = 600
        game_status.red_3_robot_hp = 400
        game_status.red_4_robot_hp = 500
        game_status.blue_1_robot_hp = 600
        game_status.blue_3_robot_hp = 400
        game_status.blue_4_robot_hp = 500
        
        # 创建RobotStatus消息
        robot_status = RobotStatus()
        robot_status.current_hp = 800
        robot_status.team_color = True
        robot_status.shooter_heat = 50
        
        # 创建AllRobotHP消息
        all_robot_hp = AllRobotHP()
        all_robot_hp.red_outpost_hp = 0
        all_robot_hp.blue_outpost_hp = 1500
        all_robot_hp.red_base_hp = 1500
        all_robot_hp.blue_base_hp = 1500
        all_robot_hp.red_1_robot_hp = 600
        all_robot_hp.red_3_robot_hp = 400
        all_robot_hp.red_4_robot_hp = 500
        all_robot_hp.blue_1_robot_hp = 600
        all_robot_hp.blue_3_robot_hp = 400
        all_robot_hp.blue_4_robot_hp = 500
        
        # 创建BulletAllowance消息
        bullet_allowance = BulletAllowance()
        bullet_allowance.bullet_allowance_17mm = 500
        
        # 发布消息
        self.game_status_pub.publish(game_status)
        self.robot_status_pub.publish(robot_status)
        self.all_robot_hp_pub.publish(all_robot_hp)
        self.bullet_allowance_pub.publish(bullet_allowance)
        
        self.message_count += 4
        
        # 每10秒输出一次统计信息
        if int(current_time - self.start_time) % 10 == 0 and self.message_count > 0:
            self.log_performance_stats()
    
    def log_performance_stats(self):
        """记录性能统计信息"""
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        self.get_logger().info(
            f"性能统计 - "
            f"消息数: {self.message_count}, "
            f"内存使用: {memory_info.rss / 1024 / 1024:.2f} MB, "
            f"CPU使用: {process.cpu_percent():.1f}%"
        )

def main(args=None):
    rclpy.init(args=args)
    
    tester = DataManagerTester()
    
    try:
        rclpy.spin(tester)
    except KeyboardInterrupt:
        tester.get_logger().info("测试被用户中断")
    finally:
        tester.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main() 