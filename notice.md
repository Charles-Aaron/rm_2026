gravity: [8.101, 0.317 , -5.505]                         # gravity to be aligned
gravity_init: [8.101, 0.317 , -5.505]                    # preknown gravity in the first IMU body frame, use when imu_en is False or start from a non-stationary state
ros2 launch rmu_gazebo_simulator bringup_sim.launch.py

ros2 launch pb2025_nav_bringup rm_navigation_simulation_launch.py \
world:=rmuc_2025 \
slam:=False

export GSETTINGS_SCHEMA_DIR=/usr/share/glib-2.0/schemas/
export XDG_DATA_DIRS=/usr/share:/usr/local/share

ros2 launch pb2025_nav_bringup rm_navigation_simulation_launch.py \
slam:=True

ros2 run rviz2 rviz2  --ros-args -r /tf:=tf -r /tf_static:=tf_static -r  __ns:=/red_standard_robot1

ros2 launch pb2025_nav_bringup rm_navigation_reality_launch.py \
slam:=True \
use_robot_state_pub:=True

ros2 launch pb2025_robot_description robot_description_launch.py 

ros2 launch pb2025_nav_bringup rm_navigation_reality_launch.py \
world:=scans \
slam:=False \
use_robot_state_pub:=True

ros2 run nav2_map_server map_saver_cli -f scans  --ros-args -r __ns:=/red_standard_robot1

#rosbag录制代码
source install/setup.zsh

ros2 bag record -o sentry_$(date +%Y%m%d_%H%M%S) \
/serial/gimbal_joint_state \
/livox/imu \
/livox/lidar \
--compression-mode file --compression-format zstd -d 30

ros2 bag play <YOUR_ROSBAG>.bag --clock

ros2 launch pb2025_sentry_bringup bringup.launch.py \
world:=<YOUR_WORLD_NAME> \
use_composition:=False \
use_rviz:=True \
use_sim_time:=True \
use_hik_camera:=False \
use_robot_state_pub:=True


#1.实车建图导航操作流程:
#1.在rm_2026工作空间下打开终端,输入指令:
source ./install/setup.bash

ros2 launch pb2025_nav_bringup rm_sentry_reality_launch.py \
world:=scans \
slam:=True \
use_robot_state_pub:=True

slam:True代表开启的是建图模式
建图完成后，slam:False则开启导航模式
启动成功后，遥控小车对环境进行建图
建图完成后，直接crtl+c退出建图，pcd图将自动保存在pointlio文件中,pcd图的名称为：scans


#2.本仓库已移除 pcd2pgm 相关包。
#如仍需将 pcd 转为 pgm，请在仓库外部工具链中完成，再将生成好的地图导入当前导航流程。

将建立好的pgm和pcd图复制到rm_2026/src/navigation/pb2025_sentry_nav/pb_nav2_bringup中的pcd和map文件中去
最后重新构建整个工作空间，在rm_2026中输入下面指令：
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=release
#注：构建成功后，一定要将小车放回建图时的启动原点


#3.输入下面指令开始导航:
source ./install/setup.bash

ros2 launch pb2025_nav_bringup rm_sentry_reality_launch.py \
world:=scans \
slam:=False \
use_robot_state_pub:=True
在nav2_ros中使用下面指令：
source ./install/setup.bash

ros2 run ros2_stm communication
显示串口打开成功则可以开始在rviz中指定导航点
