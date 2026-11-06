import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    namespace = LaunchConfiguration("namespace")
    device_code = LaunchConfiguration("device_code")
    rviz = LaunchConfiguration("rviz")
    rviz_config = LaunchConfiguration("rviz_config")

    driver_config = os.path.join(
        get_package_share_directory("livox_ros_driver2"), "config", "MID360_config.json"
    )
    lio_config = os.path.join(
        get_package_share_directory("small_point_lio"), "config", "mid360.yaml"
    )
    frames_config = os.path.join(
        get_package_share_directory("navigation_bringup"), "config", "frames.yaml"
    )
    default_rviz_config = os.path.join(
        get_package_share_directory("navigation_bringup"),
        "rviz",
        "x_sentry_navigation.rviz",
    )

    driver = Node(
        package="livox_ros_driver2",
        executable="livox_ros_driver2_node",
        name="livox_driver",
        namespace=namespace,
        output="screen",
        parameters=[
            {
                "xfer_format": 4,
                "multi_topic": 0,
                "data_src": 0,
                # Shorter lidar batches reduce the wait for fresh LIO state.
                # Per-point odometry publishing is configured in mid360.yaml.
                "publish_freq": 30.0,
                "output_data_type": 0,
                "frame_id": "imu_link",
                "lvx_file_path": "",
                "user_config_path": driver_config,
                "cmdline_input_bd_code": device_code,
            }
        ],
    )

    lio = Node(
        package="small_point_lio",
        executable="small_point_lio_node",
        name="small_point_lio",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            lio_config,
            {
                "lidar_topic": "livox/lidar",
                "imu_topic": "livox/imu",
                "lidar_frame": "imu_link",
                "imu_frame": "imu_link",
                "odom_frame": "imu_odom",
                "odometry_topic": "imu_odom",
                "registered_cloud_topic": "cloud_registered",
            },
        ],
    )

    frames = Node(
        package="frame_broadcaster",
        executable="frame_broadcaster",
        name="frame_broadcaster",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[frames_config],
    )

    cloud_frame_adapter = Node(
        package="cloud_frame_adapter",
        executable="cloud_frame_adapter",
        name="cloud_frame_adapter",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            {
                "input_topic": "cloud_registered",
                "output_topic": "registered_cloud",
                "source_frame": "imu_odom",
                "target_frame": "odom",
            }
        ],
    )

    initial_pose = Node(
        package="initial_pose",
        executable="initial_pose",
        name="initial_pose",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            {
                "map_frame": "map",
                "odom_frame": "odom",
                "base_frame": "chassis_link",
                "pose_topic": "initialpose",
            }
        ],
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        namespace=namespace,
        output="screen",
        arguments=["-d", rviz_config],
        condition=IfCondition(rviz),
        remappings=[
            ("/tf", "tf"),
            ("/tf_static", "tf_static"),
            ("/initialpose", "initialpose"),
        ],
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument("namespace", default_value="x_sentry"),
            DeclareLaunchArgument(
                "device_code", default_value="livox0000000001"
            ),
            DeclareLaunchArgument("rviz", default_value="true"),
            DeclareLaunchArgument("rviz_config", default_value=default_rviz_config),
            driver,
            lio,
            frames,
            cloud_frame_adapter,
            initial_pose,
            rviz_node,
        ]
    )
