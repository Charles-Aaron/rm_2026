import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    AppendEnvironmentVariable,
    DeclareLaunchArgument,
    ExecuteProcess,
    TimerAction,
)
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_simulator = get_package_share_directory("rmu_gazebo_simulator")
    pkg_rmoss_gz_resources = get_package_share_directory("rmoss_gz_resources")

    world_sdf_path = LaunchConfiguration("world_sdf_path")
    ign_config_path = LaunchConfiguration("ign_config_path")
    use_gui = LaunchConfiguration("use_gui")
    gui_delay = LaunchConfiguration("gui_delay")

    declare_world_sdf_path = DeclareLaunchArgument(
        "world_sdf_path",
        default_value=os.path.join(
            pkg_simulator, "resource", "worlds", "rmul_2024_world.sdf"
        ),
        description="Path to the world SDF file",
    )

    declare_ign_config_path = DeclareLaunchArgument(
        "ign_config_path",
        default_value=os.path.join(pkg_simulator, "resource", "ign", "gui.config"),
        description="Path to the Ignition Gazebo GUI configuration file",
    )

    declare_use_gui = DeclareLaunchArgument(
        "use_gui",
        default_value="True",
        description="Whether to start Gazebo GUI window",
    )

    declare_gui_delay = DeclareLaunchArgument(
        "gui_delay",
        default_value="15.0",
        description="Delay (seconds) before launching Gazebo GUI to reduce startup contention",
    )

    append_enviroment_worlds = AppendEnvironmentVariable(
        name="GAZEBO_PLUGIN_PATH",
        value=os.path.join(pkg_simulator, "resource", "worlds"),
    )

    append_enviroment_models = AppendEnvironmentVariable(
        name="IGN_GAZEBO_RESOURCE_PATH",
        value=os.path.join(pkg_simulator, "resource", "models"),
    )

    append_enviroment_shared_models = AppendEnvironmentVariable(
        name="IGN_GAZEBO_RESOURCE_PATH",
        value=os.path.join(pkg_rmoss_gz_resources, "resource", "models"),
    )

    gazebo_server = ExecuteProcess(
        cmd=[
            "ign",
            "gazebo",
            "-s",
            "-r",
            world_sdf_path,
            "--force-version",
            "6",
        ],
        output="screen",
    )

    gazebo_gui = ExecuteProcess(
        cmd=[
            "ign",
            "gazebo",
            "-g",
            "--gui-config",
            ign_config_path,
            "--force-version",
            "6",
        ],
        output="screen",
        condition=IfCondition(use_gui),
        additional_env={
            # Avoid forcing software GL by default; let system pick hardware acceleration first.
            "QT_QPA_PLATFORM": "xcb",
        },
    )

    robot_ign_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
        ],
    )

    ld = LaunchDescription()

    ld.add_action(declare_world_sdf_path)
    ld.add_action(declare_ign_config_path)
    ld.add_action(declare_use_gui)
    ld.add_action(declare_gui_delay)
    ld.add_action(append_enviroment_worlds)
    ld.add_action(append_enviroment_models)
    ld.add_action(append_enviroment_shared_models)
    ld.add_action(gazebo_server)
    ld.add_action(TimerAction(period=gui_delay, actions=[gazebo_gui]))
    ld.add_action(robot_ign_bridge)

    return ld
