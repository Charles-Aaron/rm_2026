import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node, PushRosNamespace
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # 命名空间参数
    namespace_arg = DeclareLaunchArgument(
        'namespace',
        default_value='red_standard_robot1',
        description='Robot namespace'
    )

    # 获取配置文件路径
    bt_config_dir = os.path.join(get_package_share_directory('rm_sentry_decision'), 'config')
    bt_xml_path = os.path.join(bt_config_dir, 'rmuc_2025.xml')

    # 参数
    use_sim_time = LaunchConfiguration('use_sim_time', default='False')

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
        remappings=[
            ('/tf', 'tf'),
            ('/tf_static', 'tf_static'),
        ],
        output='screen'
    )

    return LaunchDescription([
        namespace_arg,
        PushRosNamespace(LaunchConfiguration('namespace')),
        decision_node
    ])
