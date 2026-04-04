# - Config to retrieve all components of the ignition-gui6 package
#
# This should only be invoked by ignition-gui6-config.cmake.
#
# To retrieve this meta-package, use:
# find_package(ignition-gui6 COMPONENTS all)
#
# This creates the target ignition-gui6::all which will link to all known
# components of ignition-gui6, including the core library.
#
# This also creates the variable ignition-gui6_ALL_LIBRARIES
#
################################################################################

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(ignition-gui6_ALL_CONFIG_INCLUDED)
  return()
endif()
set(ignition-gui6_ALL_CONFIG_INCLUDED TRUE)

if(NOT ignition-gui6-all_FIND_QUIETLY)
  message(STATUS "Looking for all libraries of ignition-gui6 -- found version 6.8.0")
endif()


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ignition-all-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../" ABSOLUTE)

# Use original install prefix when loaded through a "/usr move"
# cross-prefix symbolic link such as /lib -> /usr/lib.
get_filename_component(_realCurr "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(_realOrig "/usr/lib/x86_64-linux-gnu/cmake/ignition-gui6-all" REALPATH)
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
find_dependency(ignition-gui6 6.8.0 EXACT)

# Find the component libraries


if(NOT TARGET ignition-gui6::ignition-gui6-all)
  include("${CMAKE_CURRENT_LIST_DIR}/ignition-gui6-all-targets.cmake")

  add_library(ignition-gui6::all INTERFACE IMPORTED)
  set_target_properties(ignition-gui6::all PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-gui6::ignition-gui6-all")

endif()

# Create the "requested" target if it does not already exist
if(NOT TARGET ignition-gui6::requested)
  add_library(ignition-gui6::requested INTERFACE IMPORTED)
endif()

# Link the "all" target to the "requested" target
get_target_property(ign_requested_components ignition-gui6::requested INTERFACE_LINK_LIBRARIES)
if(NOT ign_requested_components)
  set_target_properties(ignition-gui6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-gui6::ignition-gui6-all")
else()
  set_target_properties(ignition-gui6::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "${ign_requested_components};ignition-gui6::ignition-gui6-all")
endif()

set(ignition-gui6_ALL_LIBRARIES ignition-gui6::ignition-gui6-all)
