#!/usr/bin/env bash
_local_apt_root="/home/nuc/rm_2026/.local_apt/root"
_local_ros_prefix="${_local_apt_root}/opt/ros/humble"
_local_usr_prefix="${_local_apt_root}/usr"

if [ -d "${_local_ros_prefix}" ]; then
  export CMAKE_PREFIX_PATH="${_local_ros_prefix}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
  export PATH="${_local_ros_prefix}/bin:${PATH}"
  export LD_LIBRARY_PATH="${_local_ros_prefix}/lib:${_local_ros_prefix}/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi

if [ -d "${_local_usr_prefix}" ]; then
  export CMAKE_PREFIX_PATH="${_local_usr_prefix}/lib/x86_64-linux-gnu/cmake:${_local_usr_prefix}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
  export PATH="${_local_usr_prefix}/bin:${PATH}"
  export LD_LIBRARY_PATH="${_local_usr_prefix}/lib/x86_64-linux-gnu:${_local_usr_prefix}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
  export PKG_CONFIG_PATH="${_local_usr_prefix}/lib/x86_64-linux-gnu/pkgconfig:${_local_usr_prefix}/lib/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
fi

export Qt5QuickControls2_DIR="${_local_usr_prefix}/lib/x86_64-linux-gnu/cmake/Qt5QuickControls2"
export Qt5QuickTemplates2_DIR="${_local_usr_prefix}/lib/x86_64-linux-gnu/cmake/Qt5QuickTemplates2"
