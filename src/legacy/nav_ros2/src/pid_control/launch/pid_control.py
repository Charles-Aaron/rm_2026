from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='pid_control',
            executable='pid_control',
            name='robot_ctrl',
            respawn=True,
            output='screen',
            parameters=[
                {'global_frame': 'map'},
                {'plan_frequency': 30},
                {'max_x_speed': 2.5},
                {'max_y_speed': 2.5},
                {'set_yaw_speed': 0.0},
                {'goal_dist_tolerance': 0.20},
                {'prune_ahead_distance': 1.2},
                {'p_value': 1.0},
                {'i_value': 1.0},
                {'d_value': 1.0}
            ],
            
        )
    ])