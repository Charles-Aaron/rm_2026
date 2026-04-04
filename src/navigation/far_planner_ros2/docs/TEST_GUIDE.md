# FAR Planner ROS 2 编译与测试指南

## 一、编译指南

### 1.1 环境要求

- ROS 2 Humble (推荐) 或 ROS 2 Foxy
- Ubuntu 20.04 或 22.04
- CMake 3.10+
- GCC 9+ 或 Clang 10+
- Eigen3
- PCL (Point Cloud Library)

### 1.2 编译步骤

```bash
# 1. 进入工作空间
cd ~/rm_2025

# 2. 安装额外依赖
sudo apt update
sudo apt install -y libeigen3-dev libpcl-dev libomp-dev

# 3. 编译包
colcon build --packages-select far_planner_ros2

# 4. source 环境
source install/setup.bash
```

### 1.3 可能的编译错误及解决方案

#### 错误1: 找不到 Eigen3

```bash
# 解决方案
sudo apt install libeigen3-dev
```

#### 错误2: 找不到 PCL

```bash
# 解决方案
sudo apt install libpcl-dev
```

#### 错误3: 编译时出现模板实例化错误

```bash
# 确保使用正确的 C++ 标准
# 在 CMakeLists.txt 中添加
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

---

## 二、测试指南

### 2.1 单元测试

#### 测试1: 可见性图基本功能

```bash
# 启动测试节点
ros2 run far_planner_ros2 test_visibility_graph
```

预期结果: 输出可见性图的构建和最短路径搜索测试结果

#### 测试2: 多边形提取

```bash
# 启动点云测试
ros2 run far_planner_ros2 test_polygon_extractor
```

预期结果: 从测试点云中提取障碍物多边形

### 2.2 集成测试

#### 测试3: 规划功能测试

```bash
# 1. 启动仿真环境
ros2 launch pb2025_nav_bringup simulation.launch.py

# 2. 启动 FAR Planner
ros2 launch far_planner_ros2 far_planner.launch.py

# 3. 设置目标点 (在 RViz 中使用 2D Nav Goal)
# 或使用命令行
ros2 topic pub /red_standard_robot1/goal_pose geometry_msgs/PoseStamped "{header: {stamp: {sec: 0, nanosec: 0}, frame_id: 'map'}, pose: {position: {x: 5.0, y: 5.0, z: 0.0}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}"
```

预期结果: 
- FAR Planner 订阅点云数据
- 接收目标点后进行路径规划
- 发布规划路径到 /red_standard_robot1/plan 话题
- 在 RViz 中显示规划路径

#### 测试4: 与 Nav2 集成测试

```bash
# 启动 Nav2 + FAR Planner 双规划器
ros2 launch far_planner_ros2 nav2_far_planner.launch.py
```

预期结果:
- Nav2 原有的 SmacPlannerHybrid 正常工作
- FAR Planner 作为快速响应规划器并行运行
- 可以通过参数配置切换使用哪个规划器

### 2.3 性能测试

#### 测试5: 规划时间测试

```bash
# 运行性能基准测试
python3 ~/far_planner_ros2/scripts/planner_switcher.py --benchmark
```

评估指标:
- 平均规划时间 (目标: <10ms)
- 最大规划时间
- 规划成功率

#### 测试6: CPU/内存占用测试

```bash
# 监控资源使用
top -p $(pgrep -f far_planner)

# 或使用 ros2 topic 监控
ros2 topic hz /red_standard_robot1/plan
```

评估指标:
- CPU 占用率 (目标: <20%)
- 内存占用 (目标: <100MB)

---

## 三、调试指南

### 3.1 常用调试命令

```bash
# 查看 FAR Planner 输出
ros2 run far_planner_ros2 far_planner_node

# 查看话题列表
ros2 topic list | grep -E "(plan|terrain|odom)"

# 查看规划路径
ros2 topic echo /red_standard_robot1/plan

# 查看点云数据
ros2 topic hz /red_standard_robot1/terrain_map
```

### 3.2 参数调整

关键参数说明:

| 参数 | 默认值 | 调整建议 |
|------|--------|----------|
| max_planning_time | 1.0s | 减小可提高响应速度 |
| robot_radius | 0.2m | 根据实际机器人尺寸调整 |
| visibility_range | 10.0m | 根据感知范围调整 |
| obstacle_threshold | 0.5 | 增大可减少误检 |

### 3.3 常见问题排查

#### 问题1: 规划失败，提示 "No path found"

可能原因:
1. 目标点不可达 (在障碍物中)
2. 点云数据未正确接收
3. 障碍物太多导致无可行路径

排查步骤:
```bash
# 1. 检查点云话题
ros2 topic hz /red_standard_robot1/terrain_map

# 2. 检查目标点设置
ros2 topic echo /red_standard_robot1/goal_pose

# 3. 检查障碍物数据
ros2 run rqt_plot rqt_plot /red_standard_robot1/terrain_map/width
```

#### 问题2: 规划路径不光滑

解决方案:
- 启用路径平滑: set enable_smoothing=true
- 增加平滑迭代次数

#### 问题3: CPU 占用过高

解决方案:
- 减小 visibility_range
- 增大 plan_resolution
- 降低规划频率

---

## 四、验证清单

### 4.1 功能验证

- [ ] 包成功编译
- [ ] 节点正常启动
- [ ] 点云数据正常接收
- [ ] 目标点正常接收
- [ ] 规划路径正常发布
- [ ] 与 Nav2 正常集成

### 4.2 性能验证

- [ ] 规划时间 <10ms (典型环境)
- [ ] CPU 占用 <20%
- [ ] 内存占用 <100MB
- [ ] 规划成功率 >95%

### 4.3 鲁棒性验证

- [ ] 动态障碍物处理正常
- [ ] 未知环境探索正常
- [ ] 长时间运行稳定
- [ ] 异常情况正确处理
