#!/bin/bash

# 启动决策节点脚本

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
    # shellcheck disable=SC1090
    source "$WORKSPACE_DIR/install/setup.bash"
fi

cd "$WORKSPACE_DIR"

echo "正在启动决策节点..."
ros2 launch rm_sentry_decision rm_sentry_decision.launch.py
