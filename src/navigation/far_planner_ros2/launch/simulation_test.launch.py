# 仿真测试Launch文件 - Nav2 + FAR Planner 双规划器
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # 获取包路径
    nav_bringup_share = get_package_share_directory('pb2025_nav_bringup')
    far_planner_share = get_package_share_directory('far_planner_ros2')
    
    # 启动参数
    use_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='true',
        description='Use simulation time'
    )
    
    namespace = DeclareLaunchArgument(
        'robot_namespace',
        default_value='red_standard_robot1',
        description='Robot namespace'
    )
    
    # 设置环境变量
    set_env_vars = SetEnvironmentVariable(
        'RCUTILS_CONSOLE_OUTPUT_FORMAT', 
        '[{severity}] [{name}]: {message}'
    )
    
    # 1. 启动 Livox 激光雷达驱动
    livox_driver = Node(
        package='livox_ros_driver2',
        executable='livox_ros_driver2_node',
        name='livox_driver',
        output='screen',
        parameters=[{
            'publish_freq': 10.0,
            'xfer_format': 1,
            'multi_topic': 0,
            'data_src': 0,
            'pub_bag': False,
            'output_topic': '/livox/lidar'
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 2. 启动 Point-LIO SLAM
    point_lio = Node(
        package='point_lio',
        executable='point_lio',
        name='point_lio',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time')
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 3. 启动地形分析模块
    terrain_analysis = Node(
        package='terrain_analysis',
        executable='terrain_analysis_node',
        name='terrain_analysis',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'map_range': 5.0,
            'voxel_size': 0.05
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 4. 启动 FAR Planner
    far_planner = Node(
        package='far_planner_ros2',
        executable='far_planner_node',
        name='far_planner',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'pointcloud_topic': '/red_standard_robot1/terrain_map',
            'robot_pose_topic': '/red_standard_robot1/odom',
            'output_path_topic': '/red_standard_robot1/plan_far',
            'visibility_range': 10.0,
            'robot_radius': 0.2,
            'max_planning_time': 0.5
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 5. 启动 Nav2 控制器服务器
    controller_server = Node(
        package='nav2_controller',
        executable='controller_server',
        name='controller_server',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'controller_frequency': 15.0,
            'FollowPurePursuit.follow_lookahead_dist': 2.2,
            'FollowPurePursuit.max_linear_vel': 2.8
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 6. 启动 Nav2 规划器服务器
    planner_server = Node(
        package='nav2_planner',
        executable='planner_server',
        name='planner_server',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'planner_plugins': ['GridBased'],
            'GridBased': {
                'plugin': 'nav2_smac_planner/SmacPlannerHybrid',
                'tolerance': 0.5,
                'allow_unknown': True,
                'max_planning_time': 3.5,
                'motion_model_for_search': 'DUBIN'
            }
        }],
        namespace=LaunchConfiguration('robot_namespace')
    )
    
    # 7. 启动 Nav2 生命周期管理器
    lifecycle_manager = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager',
        output='screen',
        parameters=[{
            'autostart': True,
            'node_names': [
                'controller_server',
                'planner_server',
                'far_planner'
            ]
        }]
    )
    
    # 8. 启动 RViz2
    rviz2 = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', os.path.join(nav_bringup_share, 'rviz', 'nav2_default_view.rviz')]
    )
    
    return LaunchDescription([
        use_sim_time,
        namespace,
        set_env_vars,
        livox_driver,
        point_lio,
        terrain_analysis,
        far_planner,
        controller_server,
        planner_server,
        lifecycle_manager,
        rviz2
    ])
