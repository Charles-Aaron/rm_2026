#!/usr/bin/env bash
set -e

PROJECT_DIR="/home/nuc/rm_2026/src/rm_de_4"
WORKSPACE_DIR="/home/nuc/rm_2026/src/rm_de_4"

for setup in /opt/ros/*/setup.bash; do
  if [ -f "$setup" ]; then
    # shellcheck disable=SC1090
    source "$setup"
    break
  fi
done

if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
  # shellcheck disable=SC1091
  source "$WORKSPACE_DIR/install/setup.bash"
fi

cd "$PROJECT_DIR"
echo "正在启动：哨兵行为树配置 UI"
echo "这个终端会显示当前运行的 UI 和 ROS 节点信息。"
echo "浏览器会自动打开；如果 8765 被占用，会自动换到下一个空端口。"
echo "关闭这个终端会停止 UI。"
python3 rm_sentry_decision/scripts/bt_config_ui.py --open-browser
