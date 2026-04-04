#!/bin/bash

set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

if [ -f /opt/ros/humble/setup.bash ]; then
    # shellcheck disable=SC1091
    source /opt/ros/humble/setup.bash
fi

set -u

cd "$WORKSPACE_DIR"

export CMAKE_BUILD_PARALLEL_LEVEL=1
export MAKEFLAGS=-j1

echo "Building behaviortree_cpp_v3 in low-memory mode..."
colcon build \
  --packages-select behaviortree_cpp_v3 \
  --executor sequential \
  --parallel-workers 1 \
  --cmake-args \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_UNIT_TESTS=OFF \
    -DBUILD_TOOLS=OFF \
    -DBUILD_MANUAL_SELECTOR=OFF

echo "Building rm_sentry_decision dependency chain in low-memory mode..."
colcon build \
  --packages-up-to rm_sentry_decision \
  --executor sequential \
  --parallel-workers 1

echo "Low-memory build completed."
echo "Use: source $WORKSPACE_DIR/install/setup.bash"
