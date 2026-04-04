# Nav2 + FAR Planner 双规划器集成启动文件
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # 包路径
    nav_bringup_share = get_package_share_directory('pb2025_nav_bringup')
    
    # 启动参数
    use_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation time'
    )
    
    # Nav2 配置文件
    nav2_params_file = DeclareLaunchArgument(
        'nav2_params_file',
        default_value=os.path.join(nav_bringup_share, 'config', 'reality', 'nav2_params.yaml'),
        description='Nav2 parameters file'
    )
    
    # FAR Planner 参数文件
    far_planner_params_file = DeclareLaunchArgument(
        'far_planner_params_file',
        default_value='~/far_planner_ros2/param/far_planner_params.yaml',
        description='FAR Planner parameter file'
    )
    
    # 机器人命名空间
    robot_namespace = DeclareLaunchArgument(
        'robot_namespace',
        default_value='red_standard_robot1',
        description='Robot namespace'
    )
    
    # Nav2 bringup 启动 (包含原有的smac_planner)
    # 注意: 这里只启动规划服务器，实际需要完整的nav2_bringup
    planner_server = Node(
        package='nav2_planner',
        executable='planner_server',
        name='planner_server',
        output='screen',
        parameters=[LaunchConfiguration('nav2_params_file')],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # FAR Planner 节点 (作为快速响应规划器)
    far_planner_node = Node(
        package='far_planner_ros2',
        executable='far_planner_node',
        name='far_planner',
        output='screen',
        parameters=[LaunchConfiguration('far_planner_params_file')],
        namespace=LaunchConfiguration('robot_namespace'),
        remappings=[
            ('/terrain_map', '/red_standard_robot1/terrain_map'),
            ('/odom', '/red_standard_robot1/odom'),
            ('/plan', '/red_standard_robot1/plan_far'),
            ('/goal_pose', '/red_standard_robot1/goal_pose'),
        ]
    )
    
    # 生命周期管理器
    lifecycle_manager = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager',
        output='screen',
        parameters=[{
            'autostart': True,
            'node_names': ['planner_server', 'far_planner']
        }]
    )
    
    return LaunchDescription([
        use_sim_time,
        nav2_params_file,
        far_planner_params_file,
        robot_namespace,
        planner_server,
        far_planner_node,
        lifecycle_manager
    ])
