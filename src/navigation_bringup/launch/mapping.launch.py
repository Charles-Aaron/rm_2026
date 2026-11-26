"""独立实车建图：复用传感器链，使用原版 C++ 后端积累并保存地图包。"""

import math
import os

import yaml

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, IncludeLaunchDescription, Shutdown
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def mapping_body_parameters(frames_config):
    """启动时转换机械参数；后端的位姿和点云计算仍由原版 C++ 完成。"""
    with open(frames_config, encoding="utf-8") as stream:
        frames = yaml.safe_load(stream)["/**"]["ros__parameters"]
    translation = frames["imu_to_chassis_translation"]
    rpy = frames["imu_to_chassis_rpy"]
    if len(translation) != 3 or len(rpy) != 3 or not all(
            math.isfinite(value) for value in [*translation, *rpy]):
        raise ValueError("IMU/chassis extrinsics must contain three finite components")
    roll, pitch, yaw = rpy
    cr, sr = math.cos(roll), math.sin(roll)
    cp, sp = math.cos(pitch), math.sin(pitch)
    cy, sy = math.cos(yaw), math.sin(yaw)
    # frames.yaml 存 T_imu_chassis；后端需要 R_chassis_imu，以及 IMU→底盘的底盘系向量。
    imu_from_chassis = [
        [cy*cp, cy*sp*sr - sy*cr, cy*sp*cr + sy*sr],
        [sy*cp, sy*sp*sr + cy*cr, sy*sp*cr - cy*sr],
        [-sp, cp*sr, cp*cr],
    ]
    rotation = list(zip(*imu_from_chassis))
    inverse_pitch = math.asin(max(-1.0, min(1.0, -rotation[2][0])))
    if abs(math.cos(inverse_pitch)) > 1e-10:
        inverse_roll = math.atan2(rotation[2][1], rotation[2][2])
        inverse_yaw = math.atan2(rotation[1][0], rotation[0][0])
    else:
        inverse_roll = 0.0
        inverse_yaw = math.atan2(-rotation[0][1], rotation[1][1])
    offset = [sum(row[i] * translation[i] for i in range(3)) for row in rotation]
    return {
        "common.body.frame": frames["chassis_frame"],
        **{f"common.body.rotation.{axis}_deg": math.degrees(value)
           for axis, value in zip(("roll", "pitch", "yaw"),
                                  (inverse_roll, inverse_pitch, inverse_yaw))},
        **{f"common.body.offset.{axis}": value for axis, value in zip("xyz", offset)},
    }


def generate_launch_description():
    share = get_package_share_directory("navigation_bringup")
    namespace = LaunchConfiguration("namespace")
    use_sim_time = LaunchConfiguration("use_sim_time")

    sensors = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(share, "launch", "sensor_lio.launch.py")),
        launch_arguments={
            "namespace": namespace,
            "use_sim_time": use_sim_time,
            "lio_config": LaunchConfiguration("lio_config"),
            "device_code": LaunchConfiguration("device_code"),
            "rviz": "false",
            "localization": "false",
            "static_frames": "false",
            "coordinated_reset": "true",
        }.items(),
        condition=IfCondition(LaunchConfiguration("start_sensors")),
    )

    backend = Node(
        package="backend", executable="backend_node", name="mapping_backend",
        namespace=namespace, output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static"),
                    ("/localization/odometry", "mapping_backend/odometry")],
        parameters=[
            LaunchConfiguration("mapping_config"),
            mapping_body_parameters(os.path.join(share, "config", "frames.yaml")),
            {
                "use_sim_time": ParameterValue(use_sim_time, value_type=bool),
                "trajectory.save_directory": LaunchConfiguration("save_directory"),
                "trajectory.save_on_shutdown": ParameterValue(
                    LaunchConfiguration("save_on_shutdown"), value_type=bool),
                "common.scan_context.lidar_height": ParameterValue(
                    LaunchConfiguration("sensor_height"), value_type=float),
            },
        ],
        # 大地图的退出保存可显式开启；给予磁盘写入足够的退出时间。
        sigterm_timeout="60",
        on_exit=Shutdown(reason="Mapping backend exited"),
    )

    rviz = Node(
        package="rviz2", executable="rviz2", name="mapping_rviz",
        namespace=namespace, output="screen",
        arguments=["-d", LaunchConfiguration("rviz_config")],
        parameters=[{"use_sim_time": ParameterValue(use_sim_time, value_type=bool)}],
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static"),
                    ("/x_sentry/mapping_backend/runtime_map", "mapping_backend/runtime_map"),
                    ("/x_sentry/mapping_backend/optimized_path", "mapping_backend/optimized_path")],
        condition=IfCondition(LaunchConfiguration("rviz")),
    )

    return LaunchDescription([
        DeclareLaunchArgument("namespace", default_value="x_sentry"),
        DeclareLaunchArgument("use_sim_time", default_value="false", choices=["true", "false"]),
        DeclareLaunchArgument("start_sensors", default_value="true", choices=["true", "false"],
                              description="Disable only when the matching ZLIO input is already provided"),
        DeclareLaunchArgument("device_code", default_value="livox0000000001"),
        DeclareLaunchArgument("lio_config", default_value=os.path.join(share, "config", "zlio.yaml")),
        DeclareLaunchArgument("mapping_config", default_value=os.path.join(share, "config", "mapping.yaml")),
        DeclareLaunchArgument("save_directory", default_value="/home/nuc/x_sentry_ws/maps/biai_prior_chassis",
                              description="Destination of the complete 3D map bundle"),
        DeclareLaunchArgument("save_on_shutdown", default_value="false", choices=["true", "false"]),
        DeclareLaunchArgument("sensor_height", default_value="0.15588",
                              description="IMU-frame origin height above ground for Scan Context, in meters"),
        DeclareLaunchArgument("rviz", default_value="true", choices=["true", "false"]),
        DeclareLaunchArgument("rviz_config", default_value=os.path.join(share, "rviz", "x_sentry_mapping.rviz")),
        # 隔离传感器入口的 rviz=false 等局部参数，保留外层建图 RViz 开关。
        GroupAction([sensors]),
        backend,
        rviz,
    ])
