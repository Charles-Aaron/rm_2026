# FAR Planner 独立启动文件
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    # 启动参数
    use_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation time'
    )
    
    # FAR Planner 参数文件
    param_file = DeclareLaunchArgument(
        'param_file',
        default_value='~/far_planner_ros2/param/far_planner_params.yaml',
        description='FAR Planner parameter file'
    )
    
    # FAR Planner 节点
    far_planner_node = Node(
        package='far_planner_ros2',
        executable='far_planner_node',
        name='far_planner',
        output='screen',
        parameters=[LaunchConfiguration('param_file')],
        remappings=[
            ('/terrain_map', '/red_standard_robot1/terrain_map'),
            ('/odom', '/red_standard_robot1/odom'),
            ('/plan', '/red_standard_robot1/plan'),
            ('/goal_pose', '/red_standard_robot1/goal_pose'),
        ]
    )
    
    return LaunchDescription([
        use_sim_time,
        param_file,
        far_planner_node
    ])
