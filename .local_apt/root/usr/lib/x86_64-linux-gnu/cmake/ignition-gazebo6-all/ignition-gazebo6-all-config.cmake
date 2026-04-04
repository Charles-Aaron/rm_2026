# - Config to retrieve all components of the ignition-gazebo6 package
#
# This should only be invoked by ignition-gazebo6-config.cmake.
#
# To retrieve this meta-package, use:
# find_package(ignition-gazebo6 COMPONENTS all)
#
# This creates the target ignition-gazebo6::all which will link to all known
# components of ignition-gazebo6, including the core library.
#
# This also creates the variable ignition-gazebo6_ALL_LIBRARIES
#
################################################################################

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(ignition-gazebo6_ALL_CONFIG_INCLUDED)
  return()
endif()
set(ignition-gazebo6_ALL_CONFIG_INCLUDED TRUE)

if(NOT ignition-gazebo6-all_FIND_QUIETLY)
  message(STATUS "Looking for all libraries of ignition-gazebo6 -- found version 6.16.0")
endif()


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ignition-all-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../" ABSOLUTE)

# Use original install prefix when loaded through a "/usr move"
# cross-prefix symbolic link such as /lib -> /usr/lib.
get_filename_component(_realCurr "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(_realOrig "/usr/lib/x86_64-linux-gnu/cmake/ignition-gazebo6-all" REALPATH)
if(_realCurr STREQUAL _realOrig)
  set(PACKAGE_PREFIX_DIR "/usr")
endif()
unset(_realOrig)
unset(_realCurr)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# Get access to the find_dependency utility
include(CMakeFindDependencyMacro)

# Find the core library
find_dependency(ignition-gazebo6 6.16.0 EXACT)

# Find the component libraries
find_dependency(ignition-gazebo6-rendering 6.16.0 EXACT)
find_dependency(ignition-gazebo6-gui 6.16.0 EXACT)
find_dependency(ignition-gazebo6-ackermann-steering-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-air-pressure-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-altimeter-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-apply-joint-force-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-apply-link-wrench-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-linearbatteryplugin-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-breadcrumbs-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-buoyancy-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-buoyancy-engine-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-collada-world-exporter-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-comms-endpoint-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-contact-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-camera-video-recorder-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-detachable-joint-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-diff-drive-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-elevator-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-follow-actor-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-forcetorque-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-hydrodynamics-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-imu-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-joint-controller-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-joint-position-controller-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-joint-state-publisher-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-joint-trajectory-controller-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-kinetic-energy-monitor-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-label-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-lift-drag-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-log-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-log-video-recorder-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-logicalaudiosensorplugin-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-logical-camera-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-magnetometer-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-model-photo-shoot-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-mecanum-drive-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-multicopter-motor-model-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-multicopter-control-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-navsat-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-odometry-publisher-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-opticaltactileplugin-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-particle-emitter-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-particle-emitter2-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-performer-detector-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-perfect-comms-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-physics-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-pose-publisher-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-rf-comms-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-scene-broadcaster-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-sensors-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-shader-param-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-thermal-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-thermal-sensor-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-thruster-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-touchplugin-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-track-controller-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-tracked-vehicle-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-trajectory-follower-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-triggered-publisher-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-user-commands-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-velocity-control-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-wheel-slip-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-wind-effects-system 6.16.0 EXACT)
find_dependency(ignition-gazebo6-ign 6.16.0 EXACT)

if(NOT TARGET ignition-gazebo6::ignition-gazebo6-all)
  include("${CMAKE_CURRENT_LIST_DIR}/ignition-gazebo6-all-targets.cmake")

  add_library(ignition-gazebo6::all INTERFACE IMPORTED)
  set_target_properties(ignition-gazebo6::all PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-gazebo6::ignition-gazebo6-all")

endif()

# Create the "requested" target if it does not already exist
if(NOT TARGET ignition-gazebo6::requested)
  add_library(ignition-gazebo6::requested INTERFACE IMPORTED)
endif()

# Link the "all" target to the "requested" target
get_target_property(ign_requested_components ignition-gazebo6::requested INTERFACE_LINK_LIBRARIES)
if(NOT ign_requested_components)
  set_target_properties(ignition-gazebo6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-gazebo6::ignition-gazebo6-all")
else()
  set_target_properties(ignition-gazebo6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "${ign_requested_components};ignition-gazebo6::ignition-gazebo6-all")
endif()

set(ignition-gazebo6_ALL_LIBRARIES ignition-gazebo6::ignition-gazebo6-all)
