import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
import launch

def generate_launch_description():
    # 获取当前包的路径
    package_path = get_package_share_directory('rm_bt')
    rm_bt_path = os.path.join(package_path,'config')
    #创建一个节点描述
    rm_bt_node = Node(
        package='rm_bt',  # 节点所在的包名
        executable='rm_bt',  # 可执行文件的名称
        name='rm_bt_tree',  # 节点的名称
        cwd=rm_bt_path  # 在修正后的路径下执行可执行文件
    )
    return LaunchDescription([
        rm_bt_node
    ])

