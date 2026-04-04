from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, PushRosNamespace  # 确保从 launch_ros.actions 导入

def generate_launch_description():
    # 声明命名空间参数
    namespace_arg = DeclareLaunchArgument(
        'namespace',
        default_value='red_standard_robot1',
        description='Robot namespace'
    )

    # 创建 LaunchDescription 对象
    ld = LaunchDescription([
        namespace_arg,
        PushRosNamespace(LaunchConfiguration('namespace'))  # 应用命名空间
    ])

    # 定义节点
    navigate_tree_node = Node(
        package='navigate_tree',
        executable='bt_main',  # 确保与你的实际可执行文件名一致
        name='bt_navigation_node',
        # namespace=LaunchConfiguration('namespace'),  # 注释掉这一行
        remappings=[
            ('/tf', 'tf'),
            ('/tf_static', 'tf_static'),
        ],
        output='screen',
        parameters=[
            {'autostart': True},
            {'use_sim_time': False}
        ]
    )
    ld.add_action(navigate_tree_node)

    return ld