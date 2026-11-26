import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    namespace = LaunchConfiguration("namespace")
    device_code = LaunchConfiguration("device_code")
    rviz = LaunchConfiguration("rviz")
    rviz_config = LaunchConfiguration("rviz_config")

    driver_config = os.path.join(
        get_package_share_directory("livox_ros_driver2"), "config", "MID360_config.json"
    )
    lio_config = os.path.join(
        get_package_share_directory("navigation_bringup"), "config", "zlio.yaml"
    )
    frames_config = os.path.join(
        get_package_share_directory("navigation_bringup"), "config", "frames.yaml"
    )
    default_rviz_config = os.path.join(
        get_package_share_directory("navigation_bringup"),
        "rviz",
        "x_sentry_standalone.rviz",
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
                # ZLIO uses per-point Livox timestamps and IMU-rate predictions.
                "publish_freq": 30.0,
                "output_data_type": 0,
                "frame_id": "lidar_link",
                "imu_frame_id": "imu_link",
                "lvx_file_path": "",
                "user_config_path": driver_config,
                "cmdline_input_bd_code": device_code,
            }
        ],
    )

    lio = Node(
        package="frontend",
        executable="zlio_node",
        name="zlio",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            frames_config,
            {
                "config_file": LaunchConfiguration("lio_config"),
                "input_type": "livox",
                "lidar_topic": "livox/lidar",
                "imu_topic": "livox/imu",
                "body_frame": "imu_link",
                "world_frame": "imu_odom",
                # Livox driver forwards acceleration in g, as did the old frontend.
                "imu_acceleration_scale": 9.81,
                "publish_tf": False,
                # 与 BIT 地形层的 5 cm 体素一致，仅改变输出云采样。
                "output_resolution": 0.05,
                "coordinated_reset": ParameterValue(
                    LaunchConfiguration("coordinated_reset"), value_type=bool),
                "use_sim_time": LaunchConfiguration("use_sim_time"),
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
        parameters=[frames_config, {"use_sim_time": LaunchConfiguration("use_sim_time")}],
        condition=IfCondition(LaunchConfiguration("static_frames")),
    )

    localization = Node(
        package="scan_context_relocalization",
        executable="zlio_navigation",
        name="zlio_navigation",
        namespace=namespace,
        output="screen",
        condition=IfCondition(LaunchConfiguration("localization")),
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            frames_config,
            LaunchConfiguration("localization_config"),
            {
                "prior_map": LaunchConfiguration("prior_map"),
                "use_sim_time": LaunchConfiguration("use_sim_time"),
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
        parameters=[{"use_sim_time": LaunchConfiguration("use_sim_time")}],
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
            # 独立建图由完整后端提供地图 TF，不启动导航定位适配和导航静态树。
            DeclareLaunchArgument("localization", default_value="true", choices=["true", "false"]),
            DeclareLaunchArgument("static_frames", default_value="true", choices=["true", "false"]),
            DeclareLaunchArgument("coordinated_reset", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument(
                "device_code", default_value="livox0000000001"
            ),
            DeclareLaunchArgument("rviz", default_value="true"),
            DeclareLaunchArgument("rviz_config", default_value=default_rviz_config),
            DeclareLaunchArgument("use_sim_time", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument("lio_config", default_value=lio_config),
            DeclareLaunchArgument("prior_map", default_value="",
                                  description="z map bundle; empty selects manual initialpose alignment"),
            DeclareLaunchArgument("localization_config", default_value=os.path.join(
                get_package_share_directory("navigation_bringup"), "config", "localization.yaml")),
            driver,
            lio,
            frames,
            localization,
            rviz_node,
        ]
    )
