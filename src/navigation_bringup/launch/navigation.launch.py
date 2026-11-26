import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterValue, ParameterFile
from nav2_common.launch import RewrittenYaml


def generate_launch_description():
    rm_ros_prefix = "/home/nuc/rm_2026/.local_apt/root/opt/ros/humble"
    os.environ["AMENT_PREFIX_PATH"] = os.pathsep.join(
        [
            rm_ros_prefix,
            os.environ.get("AMENT_PREFIX_PATH", ""),
        ]
    )
    os.environ["LD_LIBRARY_PATH"] = os.pathsep.join(
        [
            os.path.join(rm_ros_prefix, "lib"),
            os.path.join(rm_ros_prefix, "lib", "x86_64-linux-gnu"),
            os.environ.get("LD_LIBRARY_PATH", ""),
        ]
    )

    namespace = LaunchConfiguration("namespace")
    start_navigation = LaunchConfiguration("start_navigation")
    map_file = LaunchConfiguration("map")
    params_file = LaunchConfiguration("params_file")
    rviz = LaunchConfiguration("rviz")
    rviz_config = LaunchConfiguration("rviz_config")
    cmd_vel_relay_enabled = LaunchConfiguration("cmd_vel_relay")
    gate_enabled = start_navigation

    sensor_lio = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("navigation_bringup"),
                "launch",
                "sensor_lio.launch.py",
            )
        ),
        launch_arguments={
            "namespace": namespace,
            "rviz": rviz,
            "rviz_config": rviz_config,
            "lio_config": LaunchConfiguration("lio_config"),
            "localization_config": LaunchConfiguration("localization_config"),
            "prior_map": LaunchConfiguration("prior_map"),
            "use_sim_time": LaunchConfiguration("use_sim_time"),
        }.items(),
    )

    odom_frame_adapter = Node(
        package="odom_frame_adapter",
        executable="odom_frame_adapter",
        name="odom_frame_adapter",
        namespace=namespace,
        output="screen",
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[
            {
                "input_topic": "imu_odom",
                "output_topic": "chassis_odometry",
                "input_frame": "imu_odom",
                "output_frame": "odom",
                "source_frame": "imu_link",
                "target_frame": "chassis_link",
                "use_sim_time": LaunchConfiguration("use_sim_time"),
            }
        ],
    )

    map_server = Node(
        package="nav2_map_server",
        executable="map_server",
        name="map_server",
        namespace=namespace,
        output="screen",
        parameters=[{"yaml_filename": map_file, "use_sim_time": LaunchConfiguration("use_sim_time")}],
        condition=IfCondition(start_navigation),
    )

    map_lifecycle = Node(
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager_map",
        namespace=namespace,
        output="screen",
        parameters=[
            {"autostart": True, "node_names": ["map_server"]}
        ],
        condition=IfCondition(start_navigation),
    )

    standalone = Node(
        package="sentry_nav", executable="sentry_navigation",
        namespace=namespace, output="screen", condition=IfCondition(start_navigation),
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[ParameterFile(RewrittenYaml(
            source_file=params_file, root_key=namespace,
            param_rewrites={
                "use_sim_time": LaunchConfiguration("use_sim_time"),
                "local_costmap.local_costmap.ros__parameters.terrain_layer.frame_topic":
                    PathJoinSubstitution(["/", namespace, "frontend/frame"]),
                "local_costmap.local_costmap.ros__parameters.terrain_layer.session_topic":
                    PathJoinSubstitution(["/", namespace, "frontend/session"]),
                "local_costmap.local_costmap.ros__parameters.terrain_layer.visualization.prefix":
                    PathJoinSubstitution(["/", namespace, "terrain"]),
                "local_costmap.local_costmap.ros__parameters.passable_layer.file": LaunchConfiguration("passable_regions"),
                "global_costmap.global_costmap.ros__parameters.passable_layer.file": LaunchConfiguration("passable_regions")},
            convert_types=True), allow_substs=True),
            PathJoinSubstitution([FindPackageShare("navigation_bringup"), "config", "frames.yaml"]),
            {"require_localization": True,
             "use_sim_time": ParameterValue(LaunchConfiguration("use_sim_time"), value_type=bool)}],
    )

    route_manager = Node(
        package="sentry_nav", executable="sentry_route_manager", name="sentry_route_manager",
        namespace=namespace, output="screen", condition=IfCondition(start_navigation),
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[ParameterFile(RewrittenYaml(source_file=params_file, root_key=namespace,
                                                param_rewrites={}, convert_types=True), allow_substs=True),
                    {"use_sim_time": ParameterValue(LaunchConfiguration("use_sim_time", default="false"), value_type=bool)}],
    )

    cmd_vel_relay = Node(
        package="cmd_vel_relay",
        executable="cmd_vel_relay",
        name="cmd_vel_relay",
        output="screen",
        parameters=[
            {
                "input_topic": PathJoinSubstitution(["/", namespace, "cmd_vel_controlled"]),
                "output_topic": "/red_standard_robot1/cmd_vel",
                "command_timeout": 0.2,
            }
        ],
        condition=IfCondition(cmd_vel_relay_enabled),
    )

    motion_gate = Node(
        package="sentry_nav", executable="sentry_motion_gate", name="sentry_motion_gate",
        namespace=namespace, output="screen", condition=IfCondition(gate_enabled),
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static"),
                    ("cmd_vel_translation", "cmd_vel"), ("cmd_vel", "cmd_vel_controlled")],
        parameters=[ParameterFile(RewrittenYaml(source_file=params_file, root_key=namespace,
                                                param_rewrites={}, convert_types=True), allow_substs=True),
                    {"special_points_file": LaunchConfiguration("special_points_file"),
                     "require_localization": True,
                     "enable_special_points": ParameterValue(LaunchConfiguration("enable_special_points"), value_type=bool),
                     "use_sim_time": LaunchConfiguration("use_sim_time", default="false")}],
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument("namespace", default_value="x_sentry"),
            DeclareLaunchArgument("use_sim_time", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument("prior_map", default_value="",
                                  description="z map bundle; empty selects manual initialpose alignment"),
            DeclareLaunchArgument("lio_config", default_value=PathJoinSubstitution(
                [FindPackageShare("navigation_bringup"), "config", "zlio.yaml"])),
            DeclareLaunchArgument("localization_config", default_value=PathJoinSubstitution(
                [FindPackageShare("navigation_bringup"), "config", "localization.yaml"])),
            DeclareLaunchArgument("start_navigation", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument("cmd_vel_relay", default_value="true"),
            DeclareLaunchArgument("special_points_file", default_value=PathJoinSubstitution(
                [FindPackageShare("navigation_bringup"), "config", "special_points.yaml"])),
            DeclareLaunchArgument("enable_special_points", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument("map", default_value=""),
            DeclareLaunchArgument("passable_regions", default_value="", description="Offline map-frame passable regions YAML"),
            DeclareLaunchArgument(
                "params_file",
                default_value=PathJoinSubstitution(
                    [FindPackageShare("navigation_bringup"), "config", "standalone_params.yaml"]),
            ),
            DeclareLaunchArgument("rviz", default_value="true"),
            DeclareLaunchArgument(
                "rviz_config",
                default_value=PathJoinSubstitution(
                    [FindPackageShare("navigation_bringup"), "rviz", "x_sentry_standalone.rviz"]),
            ),
            sensor_lio,
            odom_frame_adapter,
            map_server,
            map_lifecycle,
            standalone,
            route_manager,
            motion_gate,
            cmd_vel_relay,
        ]
    )
