#!/usr/bin/env python3
"""Publish simulated judge/status messages for rm_sentry_decision."""

import argparse

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from rm_decision_interfaces.msg import AllRobotHP, BulletAllowance, GameStatus, RobotStatus


class RefereeSimulator(Node):
    def __init__(self, args):
        super().__init__("sentry_referee_simulator")
        self.args = args
        qos = 10
        self.game_detection_pub = self.create_publisher(GameStatus, "/game_detection_status", qos)
        self.game_pub = self.create_publisher(GameStatus, "/game_status", qos)
        self.robot_pub = self.create_publisher(RobotStatus, "/robot_status", qos)
        self.bullet_pub = self.create_publisher(BulletAllowance, "/bullet_allowance", qos)
        self.hp_pub = self.create_publisher(AllRobotHP, "/all_robot_hp", qos)
        self.timer = self.create_timer(1.0 / args.rate_hz, self.publish_state)
        team = "蓝方" if args.team_color else "红方"
        self.get_logger().info(
            f"模拟裁判已启动: progress={args.game_progress} "
            f"remain={args.stage_remain_time} hp={args.current_hp} "
            f"ammo={args.ammo_17mm} team={team}")

    def publish_state(self):
        game = GameStatus()
        game.game_progress = self.args.game_progress
        game.stage_remain_time = self.args.stage_remain_time
        game.team_color = self.args.team_color
        game.current_hp = self.args.current_hp
        game.bullet_allowance_17mm = self.args.ammo_17mm
        game.red_base_hp = self.args.red_base_hp
        game.blue_base_hp = self.args.blue_base_hp
        game.red_outpost_hp = self.args.red_outpost_hp
        game.blue_outpost_hp = self.args.blue_outpost_hp
        game.red_1_robot_hp = self.args.red_1_hp
        game.red_3_robot_hp = self.args.red_3_hp
        game.red_4_robot_hp = self.args.red_4_hp
        game.blue_1_robot_hp = self.args.blue_1_hp
        game.blue_3_robot_hp = self.args.blue_3_hp
        game.blue_4_robot_hp = self.args.blue_4_hp

        robot = RobotStatus()
        robot.team_color = self.args.team_color
        robot.current_hp = self.args.current_hp
        robot.bullet_allowance_17mm = self.args.ammo_17mm

        bullet = BulletAllowance()
        bullet.bullet_allowance_17mm = self.args.ammo_17mm

        hp = AllRobotHP()
        hp.red_base_hp = self.args.red_base_hp
        hp.blue_base_hp = self.args.blue_base_hp
        hp.red_outpost_hp = self.args.red_outpost_hp
        hp.blue_outpost_hp = self.args.blue_outpost_hp
        hp.red_1_robot_hp = self.args.red_1_hp
        hp.red_3_robot_hp = self.args.red_3_hp
        hp.red_4_robot_hp = self.args.red_4_hp
        hp.blue_1_robot_hp = self.args.blue_1_hp
        hp.blue_3_robot_hp = self.args.blue_3_hp
        hp.blue_4_robot_hp = self.args.blue_4_hp

        self.game_detection_pub.publish(game)
        self.game_pub.publish(game)
        self.robot_pub.publish(robot)
        self.bullet_pub.publish(bullet)
        self.hp_pub.publish(hp)


def nonnegative_int(value):
    parsed = int(float(value))
    if parsed < 0:
        raise argparse.ArgumentTypeError("must be >= 0")
    return parsed


def main():
    parser = argparse.ArgumentParser(description="Simulated judge system for rm_sentry_decision")
    parser.add_argument("--rate-hz", type=float, default=10.0)
    parser.add_argument("--game-progress", type=nonnegative_int, default=4)
    parser.add_argument("--stage-remain-time", type=nonnegative_int, default=300)
    parser.add_argument("--team-color", choices=["red", "blue"], default="red")
    parser.add_argument("--current-hp", type=nonnegative_int, default=400)
    parser.add_argument("--ammo-17mm", type=nonnegative_int, default=500)
    parser.add_argument("--red-base-hp", type=nonnegative_int, default=3000)
    parser.add_argument("--blue-base-hp", type=nonnegative_int, default=3000)
    parser.add_argument("--red-outpost-hp", type=nonnegative_int, default=0)
    parser.add_argument("--blue-outpost-hp", type=nonnegative_int, default=0)
    parser.add_argument("--red-1-hp", type=nonnegative_int, default=400)
    parser.add_argument("--red-3-hp", type=nonnegative_int, default=400)
    parser.add_argument("--red-4-hp", type=nonnegative_int, default=400)
    parser.add_argument("--blue-1-hp", type=nonnegative_int, default=400)
    parser.add_argument("--blue-3-hp", type=nonnegative_int, default=400)
    parser.add_argument("--blue-4-hp", type=nonnegative_int, default=400)
    args = parser.parse_args()
    args.team_color = args.team_color == "blue"

    rclpy.init()
    node = RefereeSimulator(args)
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()
