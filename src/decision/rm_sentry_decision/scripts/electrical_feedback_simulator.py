#!/usr/bin/env python3
"""Simulate sentry electrical feedback for no-hardware BT tests."""

import argparse

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from rm_decision_interfaces.msg import SentryPoseCommand, SentryPoseResult
from rm_msgs.msg import GimbalStatus


class ElectricalFeedbackSimulator(Node):
    def __init__(self, args):
        super().__init__("sentry_electrical_feedback_simulator")
        self.ack_pose = args.ack_pose
        self.gimbal_mode = args.initial_gimbal_mode
        self.pose_result_pub = self.create_publisher(SentryPoseResult, "/sentry/pose_result", 10)
        self.gimbal_status_pub = self.create_publisher(
            GimbalStatus, "/gimbal_controller/gimbal_status", 10
        )
        self.pose_command_sub = self.create_subscription(
            SentryPoseCommand, "/sentry/pose_command", self.pose_command_cb, 10
        )
        self.gimbal_command_sub = self.create_subscription(
            SentryPoseCommand, "/sentry/gimbal_lower_command", self.gimbal_command_cb, 10
        )
        self.timer = self.create_timer(1.0 / args.rate_hz, self.publish_gimbal_status)
        self.get_logger().info(
            "电控反馈模拟器已启动：应答 /sentry/pose_command，发布 /gimbal_controller/gimbal_status"
        )

    def pose_command_cb(self, msg):
        if not self.ack_pose:
            return
        result = SentryPoseResult()
        result.pose_type = msg.pose_type
        result.pose_success = True
        self.pose_result_pub.publish(result)
        self.get_logger().info(f"姿态回执模拟：pose_type={msg.pose_type} success=True")

    def gimbal_command_cb(self, msg):
        self.gimbal_mode = 5 if msg.fold_type == 1 else 1
        self.publish_gimbal_status()
        self.get_logger().info(
            f"云台反馈模拟：fold_type={msg.fold_type} -> gimbal_mode={self.gimbal_mode}"
        )

    def publish_gimbal_status(self):
        msg = GimbalStatus()
        msg.stamp = self.get_clock().now().to_msg()
        msg.gimbal_mode = int(self.gimbal_mode)
        self.gimbal_status_pub.publish(msg)


def main():
    parser = argparse.ArgumentParser(description="Simulate electrical feedback for rm_sentry_decision")
    parser.add_argument("--rate-hz", type=float, default=20.0)
    parser.add_argument("--initial-gimbal-mode", type=int, choices=[1, 5], default=1)
    parser.add_argument("--no-pose-ack", dest="ack_pose", action="store_false")
    parser.set_defaults(ack_pose=True)
    args = parser.parse_args()

    rclpy.init()
    node = ElectricalFeedbackSimulator(args)
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
