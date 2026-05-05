 可以移植，而且你这个“桌面启动 UI”要特别注意：**它现在是写死路径的**，不是装到 ROS install 里自动找到的。

目前 UI 入口在这里：

[open_bt_config_ui.sh](/home/nuc/rm_2026/src/rm_de_4/rm_sentry_decision/scripts/open_bt_config_ui.sh:4)

```bash
PROJECT_DIR="/home/nuc/rm_2026/src/rm_de_4"
WORKSPACE_DIR="/home/nuc/rm_2026/src/rm_de_4"
```

桌面图标在这里：

[sentry_bt_config_ui.desktop](/home/nuc/rm_2026/src/rm_de_4/rm_sentry_decision/scripts/sentry_bt_config_ui.desktop:5)

```ini
Exec=/home/nuc/rm_2026/src/rm_de_4/rm_sentry_decision/scripts/open_bt_config_ui.sh
```

所以迁移分两种情况。

**情况 1：移植到同一路径**
如果目标机器还是：

```bash
/home/nuc/rm_2026/src/rm_de_4
```

那最简单，直接拷贝源码包，重新编译：

```bash
cd /home/nuc/rm_2026/src/rm_de_4
colcon build --packages-select rm_decision_interfaces rm_sentry_decision
source install/setup.bash
```

桌面 UI 基本不用改，只要把 `.desktop` 放到桌面并赋权：

```bash
cp rm_sentry_decision/scripts/sentry_bt_config_ui.desktop ~/Desktop/
chmod +x ~/Desktop/sentry_bt_config_ui.desktop
chmod +x rm_sentry_decision/scripts/open_bt_config_ui.sh
```

**情况 2：移植到不同路径**
比如换成：

```bash
/home/nuc/rm_ws/src/rm_de_4
```

那你必须改两处：

```bash
rm_sentry_decision/scripts/open_bt_config_ui.sh
rm_sentry_decision/scripts/sentry_bt_config_ui.desktop
```

把里面的 `/home/nuc/rm_2026/src/rm_de_4` 全部替换成新路径。

可以这样：

```bash
old="/home/nuc/rm_2026/src/rm_de_4"
new="/你的/新/workspace/路径"

sed -i "s|$old|$new|g" rm_sentry_decision/scripts/open_bt_config_ui.sh
sed -i "s|$old|$new|g" rm_sentry_decision/scripts/sentry_bt_config_ui.desktop
```

然后重新复制桌面图标：

```bash
cp rm_sentry_decision/scripts/sentry_bt_config_ui.desktop ~/Desktop/
chmod +x ~/Desktop/sentry_bt_config_ui.desktop
```

**建议迁移哪些东西**
不要拷贝 `build/ install/ log/`，容易带旧环境路径。只迁移源码：

```bash
rm_decision_interfaces/
rm_sentry_decision/
```

至少要包含这些新增/关键内容：

```text
rm_sentry_decision/src/data_manager.cpp
rm_sentry_decision/src/topics_to_blackboard.cpp
rm_sentry_decision/src/patrol_waypoints.cpp
rm_sentry_decision/src/need_supply.cpp
rm_sentry_decision/src/recovered_enough.cpp
rm_sentry_decision/src/send_goal.cpp
rm_sentry_decision/src/send_through_goals.cpp
rm_sentry_decision/config/rmuc_2025_conservative.xml
rm_sentry_decision/config/rmuc_2025_aggressive.xml
rm_sentry_decision/config/conservative_patrol.csv
rm_sentry_decision/CMakeLists.txt
rm_sentry_decision/package.xml
```

**目标机器依赖**
目标机需要有：

```bash
sudo apt install -y \
  ros-humble-behaviortree-cpp-v3 \
  ros-humble-nav2-msgs \
  ros-humble-tf2-ros \
  ros-humble-geometry-msgs \
  ros-humble-std-msgs \
  libyaml-cpp-dev
```

然后编译：

```bash
cd /你的/新/workspace/路径
source /opt/ros/humble/setup.bash
colcon build --packages-select rm_decision_interfaces rm_sentry_decision
source install/setup.bash
```

**一个重要提醒**
你现在这个桌面 UI 目前默认编辑/启动的是：

```python
BT_XML = "rmuc_2025_first_attack_patrol.xml"
```

也就是 UI 还不是默认控制我们刚融合的：

```text
rmuc_2025_conservative.xml
rmuc_2025_aggressive.xml
```

所以如果你想用桌面 UI 启动“融合版”，还需要把 UI 改成选择保守/激进，或者先简单改成默认保守版。最小改法是在 [bt_config_ui.py](/home/nuc/rm_2026/src/rm_de_4/rm_sentry_decision/scripts/bt_config_ui.py:24) 改：

```python
BT_XML = "rmuc_2025_conservative.xml"
PATROL_CSV = "conservative_patrol.csv"
```

这样桌面 UI 启动的就是当前融合保守版。当前 launch 默认也是保守版。
