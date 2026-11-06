import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, PythonExpression
from launch_ros.actions import Node, PushRosNamespace, SetRemap
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterValue, ParameterFile
from nav2_common.launch import RewrittenYaml


def generate_launch_description():
    rm_ros_prefix = "/home/nuc/rm_2026/.local_apt/root/opt/ros/humble"
    rm_plugin_prefix = "/home/nuc/rm_2026/install/pb_nav2_plugins"
    os.environ["AMENT_PREFIX_PATH"] = os.pathsep.join(
        [
            rm_plugin_prefix,
            rm_ros_prefix,
            os.environ.get("AMENT_PREFIX_PATH", ""),
        ]
    )
    os.environ["LD_LIBRARY_PATH"] = os.pathsep.join(
        [
            os.path.join(rm_plugin_prefix, "lib"),
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
    navigation_backend = LaunchConfiguration("navigation_backend")
    gate_enabled = start_navigation
    nav2_enabled = PythonExpression(["'", start_navigation, "'.lower() == 'true' and '", navigation_backend, "' != 'standalone'"])
    standalone_enabled = PythonExpression(["'", start_navigation, "'.lower() == 'true' and '", navigation_backend, "' == 'standalone'"])

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
                "output_topic": "chassis_odom",
                "source_frame": "imu_link",
                "target_frame": "chassis_link",
            }
        ],
    )

    map_server = Node(
        package="nav2_map_server",
        executable="map_server",
        name="map_server",
        namespace=namespace,
        output="screen",
        parameters=[{"yaml_filename": map_file}],
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

    nav2 = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution(
                [FindPackageShare("nav2_bringup"), "launch", "navigation_launch.py"]
            )
        ),
        launch_arguments={
            "namespace": namespace,
            "use_namespace": "True",
            "autostart": "True",
            "params_file": params_file,
        }.items(),
        condition=IfCondition(nav2_enabled),
    )

    nav2_group = GroupAction(
        [
            PushRosNamespace(namespace),
            SetRemap(src="/tf", dst="tf"),
            SetRemap(src="/tf_static", dst="tf_static"),
            nav2,
        ],
        condition=IfCondition(nav2_enabled),
    )

    standalone = Node(
        package="sentry_nav", executable="sentry_navigation",
        namespace=namespace, output="screen", condition=IfCondition(standalone_enabled),
        remappings=[("/tf", "tf"), ("/tf_static", "tf_static")],
        parameters=[ParameterFile(RewrittenYaml(
            source_file=params_file, root_key=namespace,
            param_rewrites={
                "local_costmap.local_costmap.ros__parameters.intensity_voxel_layer.lidar.topic":
                    PathJoinSubstitution(["/", namespace, "registered_cloud"]),
                "local_costmap.local_costmap.ros__parameters.passable_layer.file": LaunchConfiguration("passable_regions"),
                "global_costmap.global_costmap.ros__parameters.passable_layer.file": LaunchConfiguration("passable_regions")},
            convert_types=True), allow_substs=True)],
    )

    route_manager = Node(
        package="sentry_nav", executable="sentry_route_manager", name="sentry_route_manager",
        namespace=namespace, output="screen", condition=IfCondition(standalone_enabled),
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
                     "enable_special_points": ParameterValue(LaunchConfiguration("enable_special_points"), value_type=bool),
                     "use_sim_time": LaunchConfiguration("use_sim_time", default="false")}],
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument("namespace", default_value="x_sentry"),
            DeclareLaunchArgument("start_nav2", default_value="false",
                                  description="Deprecated alias for start_navigation"),
            DeclareLaunchArgument("start_navigation", default_value=LaunchConfiguration("start_nav2")),
            DeclareLaunchArgument("cmd_vel_relay", default_value="true"),
            DeclareLaunchArgument("navigation_backend", default_value="standalone",
                                  choices=["standalone", "astar_snap_mpc"]),
            DeclareLaunchArgument("special_points_file", default_value=PathJoinSubstitution(
                [FindPackageShare("navigation_bringup"), "config", "special_points.yaml"])),
            DeclareLaunchArgument("enable_special_points", default_value="false", choices=["true", "false"]),
            DeclareLaunchArgument("map", default_value=""),
            DeclareLaunchArgument("passable_regions", default_value="", description="Offline map-frame passable regions YAML"),
            DeclareLaunchArgument(
                "params_file",
                default_value=PythonExpression(["'", get_package_share_directory("navigation_bringup"),
                    "/config/' + ('standalone_params.yaml' if '", navigation_backend,
                    "' == 'standalone' else 'nav2_params.yaml')"]),
            ),
            DeclareLaunchArgument("rviz", default_value="true"),
            DeclareLaunchArgument(
                "rviz_config",
                default_value=PythonExpression(["'", get_package_share_directory("navigation_bringup"),
                    "/rviz/' + ('x_sentry_standalone.rviz' if '", navigation_backend,
                    "' == 'standalone' else 'x_sentry_navigation.rviz')"]),
            ),
            sensor_lio,
            odom_frame_adapter,
            map_server,
            map_lifecycle,
            nav2_group,
            standalone,
            route_manager,
            motion_gate,
            cmd_vel_relay,
        ]
    )
