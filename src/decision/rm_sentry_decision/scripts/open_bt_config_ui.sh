#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

find_workspace_setup() {
  local dir="$SCRIPT_DIR"
  while [ "$dir" != "/" ]; do
    if [ -f "$dir/install/setup.bash" ]; then
      echo "$dir/install/setup.bash"
      return 0
    fi
    if [ -f "$dir/local_setup.bash" ] && [ -d "$dir/share/rm_sentry_decision" ]; then
      echo "$dir/local_setup.bash"
      return 0
    fi
    if [ -f "$dir/setup.bash" ] && [ -d "$dir/share/rm_sentry_decision" ]; then
      echo "$dir/setup.bash"
      return 0
    fi
    dir="$(dirname "$dir")"
  done
  return 1
}

for setup in /opt/ros/*/setup.bash; do
  if [ -f "$setup" ]; then
    # shellcheck disable=SC1090
    source "$setup"
    break
  fi
done

if workspace_setup="$(find_workspace_setup)"; then
  # shellcheck disable=SC1091
  source "$workspace_setup"
fi

cd "$SCRIPT_DIR"
echo "正在启动：哨兵行为树配置 UI"
echo "这个终端会显示当前运行的 UI 和 ROS 节点信息。"
echo "浏览器会自动打开；如果 8765 被占用，会自动换到下一个空端口。"
echo "关闭这个终端会停止 UI。"
python3 "$SCRIPT_DIR/bt_config_ui.py" --open-browser
