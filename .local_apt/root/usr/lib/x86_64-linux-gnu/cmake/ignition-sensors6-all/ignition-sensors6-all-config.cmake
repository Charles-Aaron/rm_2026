# - Config to retrieve all components of the ignition-sensors6 package
#
# This should only be invoked by ignition-sensors6-config.cmake.
#
# To retrieve this meta-package, use:
# find_package(ignition-sensors6 COMPONENTS all)
#
# This creates the target ignition-sensors6::all which will link to all known
# components of ignition-sensors6, including the core library.
#
# This also creates the variable ignition-sensors6_ALL_LIBRARIES
#
################################################################################

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(ignition-sensors6_ALL_CONFIG_INCLUDED)
  return()
endif()
set(ignition-sensors6_ALL_CONFIG_INCLUDED TRUE)

if(NOT ignition-sensors6-all_FIND_QUIETLY)
  message(STATUS "Looking for all libraries of ignition-sensors6 -- found version 6.8.0")
endif()


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ignition-all-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../" ABSOLUTE)

# Use original install prefix when loaded through a "/usr move"
# cross-prefix symbolic link such as /lib -> /usr/lib.
get_filename_component(_realCurr "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(_realOrig "/usr/lib/x86_64-linux-gnu/cmake/ignition-sensors6-all" REALPATH)
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
find_dependency(ignition-sensors6 6.8.0 EXACT)

# Find the component libraries
find_dependency(ignition-sensors6-rendering 6.8.0 EXACT)
find_dependency(ignition-sensors6-camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-depth_camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-lidar 6.8.0 EXACT)
find_dependency(ignition-sensors6-gpu_lidar 6.8.0 EXACT)
find_dependency(ignition-sensors6-logical_camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-magnetometer 6.8.0 EXACT)
find_dependency(ignition-sensors6-imu 6.8.0 EXACT)
find_dependency(ignition-sensors6-altimeter 6.8.0 EXACT)
find_dependency(ignition-sensors6-air_pressure 6.8.0 EXACT)
find_dependency(ignition-sensors6-force_torque 6.8.0 EXACT)
find_dependency(ignition-sensors6-navsat 6.8.0 EXACT)
find_dependency(ignition-sensors6-rgbd_camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-thermal_camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-boundingbox_camera 6.8.0 EXACT)
find_dependency(ignition-sensors6-segmentation_camera 6.8.0 EXACT)

if(NOT TARGET ignition-sensors6::ignition-sensors6-all)
  include("${CMAKE_CURRENT_LIST_DIR}/ignition-sensors6-all-targets.cmake")

  add_library(ignition-sensors6::all INTERFACE IMPORTED)
  set_target_properties(ignition-sensors6::all PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-sensors6::ignition-sensors6-all")

endif()

# Create the "requested" target if it does not already exist
if(NOT TARGET ignition-sensors6::requested)
  add_library(ignition-sensors6::requested INTERFACE IMPORTED)
endif()

# Link the "all" target to the "requested" target
get_target_property(ign_requested_components ignition-sensors6::requested INTERFACE_LINK_LIBRARIES)
if(NOT ign_requested_components)
  set_target_properties(ignition-sensors6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-sensors6::ignition-sensors6-all")
else()
  set_target_properties(ignition-sensors6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "${ign_requested_components};ignition-sensors6::ignition-sensors6-all")
endif()

set(ignition-sensors6_ALL_LIBRARIES ignition-sensors6::ignition-sensors6-all)
