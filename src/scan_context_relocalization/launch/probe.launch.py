"""Offline relocalization and optional RViz; ZLIO and rosbag run separately."""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    def typed(name, value_type):
        return ParameterValue(LaunchConfiguration(name), value_type=value_type)

    return LaunchDescription([
        DeclareLaunchArgument('prior_map', description='Absolute path to upstream map bundle'),
        DeclareLaunchArgument('output_csv', default_value=''),
        DeclareLaunchArgument('frame_topic', default_value='/frontend/frame'),
        DeclareLaunchArgument('query_interval', default_value='3.0'),
        DeclareLaunchArgument('visualization_map_voxel', default_value='0.2'),
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        DeclareLaunchArgument('rviz', default_value='true'),
        Node(
            package='scan_context_relocalization', executable='scan_context_probe',
            name='scan_context_probe', output='screen',
            parameters=[{
                'prior_map': typed('prior_map', str),
                'output_csv': typed('output_csv', str),
                'frame_topic': typed('frame_topic', str),
                'query_interval': typed('query_interval', float),
                'visualization_map_voxel': typed('visualization_map_voxel', float),
                'use_sim_time': typed('use_sim_time', bool),
                'visualization': True,
                'map_frame': 'experiment_map',
            }]),
        Node(
            package='rviz2', executable='rviz2', name='scan_context_rviz',
            condition=IfCondition(LaunchConfiguration('rviz')), output='screen',
            arguments=['-d', PathJoinSubstitution([
                FindPackageShare('scan_context_relocalization'), 'rviz', 'probe.rviz'])],
            parameters=[{'use_sim_time': typed('use_sim_time', bool)}]),
    ])
