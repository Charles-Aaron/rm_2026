#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from rm_decision_interfaces.msg import RobotStatus, BulletAllowance, AllRobotHP, GameStatus

class StatePublisher(Node):
    def __init__(self):
        super().__init__('state_publisher')
        
        # Publishers
        self.robot_pub = self.create_publisher(RobotStatus, '/robot_status', 10)
        self.bullet_pub = self.create_publisher(BulletAllowance, '/bullet_allowance', 10)
        self.hp_pub = self.create_publisher(AllRobotHP, '/all_robot_hp', 10)
        self.game_pub = self.create_publisher(GameStatus, '/game_status', 10)
        
        # Timer
        self.timer = self.create_timer(2.0, self.publish_messages)
        self.stage = 0
        
    def publish_messages(self):
        # Robot status
        robot_msg = RobotStatus()
        robot_msg.robot_id = 7
        robot_msg.team_color = False  # Red team
        
        # Bullet allowance
        bullet_msg = BulletAllowance()
        
        # HP status
        hp_msg = AllRobotHP()
        
        # Game status
        game_msg = GameStatus()
        game_msg.game_progress = 4
        game_msg.stage_remain_time = 300
        
        if self.stage == 0:
            # Stage 1: Attack outpost
            print("🎯 Stage 1: Attack Outpost")
            robot_msg.current_hp = 300
            bullet_msg.bullet_allowance_17mm = 120
            hp_msg.blue_outpost_hp = 200  # Enemy outpost alive
            
        elif self.stage == 1:
            # Stage 2: Need supply  
            print("🔋 Stage 2: Need Supply")
            robot_msg.current_hp = 150
            bullet_msg.bullet_allowance_17mm = 30  # Low ammo
            hp_msg.blue_outpost_hp = 0
            hp_msg.blue_base_hp = 0
            
        elif self.stage == 2:
            # Stage 3: Patrol
            print("🚶 Stage 3: Patrol")
            robot_msg.current_hp = 200
            bullet_msg.bullet_allowance_17mm = 100
            hp_msg.blue_outpost_hp = 0
            hp_msg.blue_base_hp = 0
            
        elif self.stage == 3:
            # Stage 4: Go home (low resources)
            print("🏠 Stage 4: Go Home")
            robot_msg.current_hp = 50
            bullet_msg.bullet_allowance_17mm = 10
            hp_msg.blue_outpost_hp = 0
            hp_msg.blue_base_hp = 0
        
        # Publish all messages
        self.robot_pub.publish(robot_msg)
        self.bullet_pub.publish(bullet_msg)
        self.hp_pub.publish(hp_msg)
        self.game_pub.publish(game_msg)
        
        # Next stage
        self.stage = (self.stage + 1) % 4

def main():
    rclpy.init()
    node = StatePublisher()
    print("🚀 State Publisher started! Cycling through 4 stages every 8 seconds...")
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print("\n✋ Stopped by user")
    finally:
        rclpy.shutdown()

if __name__ == '__main__':
    main() 
