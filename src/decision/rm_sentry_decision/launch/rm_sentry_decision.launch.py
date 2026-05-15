from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node, PushRosNamespace
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution

def generate_launch_description():
    # 命名空间参数
    namespace_arg = DeclareLaunchArgument(
        'namespace',
        default_value='',
        description='Robot namespace'
    )

    use_sim_time_arg = DeclareLaunchArgument(
        'use_sim_time',
        default_value='True',
        description='Use simulation (Gazebo) clock if true'
    )

    bt_xml_arg = DeclareLaunchArgument(
        'bt_xml',
        default_value='rmuc_2025_first_attack_patrol.xml',
        description='Behavior tree XML file under rm_sentry_decision/config'
    )

    bt_xml_path = PathJoinSubstitution([
        FindPackageShare('rm_sentry_decision'),
        'config',
        LaunchConfiguration('bt_xml'),
    ])

    # 参数
    use_sim_time = LaunchConfiguration('use_sim_time')

    # 节点定义
    decision_node = Node(
        package='rm_sentry_decision',
        executable='decision_node',
        name='decision_node',
        respawn=True,
        respawn_delay=3,
        parameters=[
            {'xml_path': bt_xml_path},
            {'use_sim_time': use_sim_time}
        ],
        output='screen'
    )

    return LaunchDescription([
        namespace_arg,
        use_sim_time_arg,
        bt_xml_arg,
        PushRosNamespace(LaunchConfiguration('namespace')),
        decision_node
    ])
