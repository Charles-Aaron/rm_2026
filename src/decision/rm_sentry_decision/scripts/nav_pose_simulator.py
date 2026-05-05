#!/usr/bin/env python3
"""Simulate Nav2 action servers and robot TF pose for BT debugging."""

import math
import time

import rclpy
from builtin_interfaces.msg import Duration
from geometry_msgs.msg import TransformStamped
from nav2_msgs.action import NavigateThroughPoses, NavigateToPose
from rclpy.action import ActionServer
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from tf2_ros import TransformBroadcaster


class NavPoseSimulator(Node):
    def __init__(self):
        super().__init__("sentry_nav_pose_simulator")
        self.tf_broadcaster = TransformBroadcaster(self)
        self.current_x = 0.0
        self.current_y = 0.0
        self.current_z = 0.0
        self.timer = self.create_timer(0.05, self.publish_current_tf)
        self.to_pose_server = ActionServer(
            self,
            NavigateToPose,
            "navigate_to_pose",
            self.execute_to_pose)
        self.through_poses_server = ActionServer(
            self,
            NavigateThroughPoses,
            "navigate_through_poses",
            self.execute_through_poses)
        self.get_logger().info(
            "导航/位置模拟器已启动：提供 navigate_to_pose、navigate_through_poses，并发布 map->gimbal_yaw_fake/base_link TF")

    def publish_pose_tf(self, x, y, z=0.0):
        self.current_x = float(x)
        self.current_y = float(y)
        self.current_z = float(z)
        self.publish_current_tf()

    def publish_current_tf(self):
        now = self.get_clock().now().to_msg()
        for child in ("gimbal_yaw_fake", "base_link"):
            tf = TransformStamped()
            tf.header.stamp = now
            tf.header.frame_id = "map"
            tf.child_frame_id = child
            tf.transform.translation.x = self.current_x
            tf.transform.translation.y = self.current_y
            tf.transform.translation.z = self.current_z
            tf.transform.rotation.w = 1.0
            self.tf_broadcaster.sendTransform(tf)

    def duration_msg(self, seconds):
        msg = Duration()
        msg.sec = int(seconds)
        msg.nanosec = int((seconds - int(seconds)) * 1e9)
        return msg

    def interpolate_to(self, x, y, z, steps=10, delay=0.05):
        start_x = self.current_x
        start_y = self.current_y
        start_z = self.current_z
        for idx in range(1, steps + 1):
            ratio = idx / steps
            self.publish_pose_tf(
                start_x + (x - start_x) * ratio,
                start_y + (y - start_y) * ratio,
                start_z + (z - start_z) * ratio)
            time.sleep(delay)

    def execute_to_pose(self, goal_handle):
        pose = goal_handle.request.pose.pose.position
        self.get_logger().info(
            f"模拟单点导航：目标=({pose.x:.2f}, {pose.y:.2f})")
        feedback = NavigateToPose.Feedback()
        feedback.distance_remaining = math.hypot(pose.x - self.current_x, pose.y - self.current_y)
        feedback.current_pose = goal_handle.request.pose
        feedback.navigation_time = self.duration_msg(0.0)
        feedback.estimated_time_remaining = self.duration_msg(0.5)
        goal_handle.publish_feedback(feedback)

        self.interpolate_to(pose.x, pose.y, pose.z)

        feedback.distance_remaining = 0.0
        feedback.current_pose = goal_handle.request.pose
        feedback.navigation_time = self.duration_msg(0.5)
        feedback.estimated_time_remaining = self.duration_msg(0.0)
        goal_handle.publish_feedback(feedback)
        goal_handle.succeed()
        self.get_logger().info("模拟单点导航：已到达目标")
        return NavigateToPose.Result()

    def execute_through_poses(self, goal_handle):
        poses = goal_handle.request.poses
        self.get_logger().info(f"模拟途经点导航：收到 {len(poses)} 个点")
        total = len(poses)
        for idx, pose_stamped in enumerate(poses):
            pose = pose_stamped.pose.position
            feedback = NavigateThroughPoses.Feedback()
            feedback.distance_remaining = math.hypot(pose.x - self.current_x, pose.y - self.current_y)
            feedback.number_of_poses_remaining = max(total - idx, 1)
            feedback.current_pose = pose_stamped
            feedback.navigation_time = self.duration_msg(idx * 0.5)
            feedback.estimated_time_remaining = self.duration_msg(max(total - idx - 1, 0) * 0.5)
            goal_handle.publish_feedback(feedback)
            self.get_logger().info(
                f"模拟途经点导航：到达第 {idx + 1}/{total} 个点 ({pose.x:.2f}, {pose.y:.2f})")
            self.interpolate_to(pose.x, pose.y, pose.z)

        feedback = NavigateThroughPoses.Feedback()
        feedback.distance_remaining = 0.0
        feedback.number_of_poses_remaining = 0
        if poses:
            feedback.current_pose = poses[-1]
        feedback.navigation_time = self.duration_msg(total * 0.5)
        feedback.estimated_time_remaining = self.duration_msg(0.0)
        goal_handle.publish_feedback(feedback)
        goal_handle.succeed()
        self.get_logger().info("模拟途经点导航：全部点已到达")
        return NavigateThroughPoses.Result()


def main():
    rclpy.init()
    node = NavPoseSimulator()
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
