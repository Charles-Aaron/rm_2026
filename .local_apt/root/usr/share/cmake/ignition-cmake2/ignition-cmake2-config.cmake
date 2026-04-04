# - Config file for ignition-cmake (not for any other ignition libraries).
#
# To find and load ignition-cmake modules into your project, simply invoke:
#
# find_package(ignition-cmake2)
#
# That will define the variable ignition-cmake2_FOUND, and will open up access
# to all the cmake-modules and find-modules that are provided by ignition-cmake.

# We explicitly set the desired cmake version to ensure that the policy settings
# of users or of toolchains do not result in the wrong behavior for our modules.
# Note that the call to find_package(~) will PUSH a new policy stack before
# taking on these version settings, and then that stack will POP after the
# find_package(~) has exited, so this will not affect the cmake policy settings
# of a caller.
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(ignition-cmake2_CONFIG_INCLUDED)
  return()
endif()
set(ignition-cmake2_CONFIG_INCLUDED TRUE)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ignition-cmake-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################

if(NOT TARGET ignition-cmake2::ignition-cmake2-utilities)
  include("${CMAKE_CURRENT_LIST_DIR}/ignition-cmake2-utilities-targets.cmake")

  # Create a simplified imported target name for the utilities library
  add_library(ignition-cmake2::utilities INTERFACE IMPORTED)
  set_target_properties(ignition-cmake2::utilities PROPERTIES
    INTERFACE_LINK_LIBRARIES ignition-cmake2::ignition-cmake2-utilities)

endif()

# We should only perform these operations if either
#  1. ign-cmake has not been included yet, or
#  2. we need a higher version of ign-cmake than the last one that was included
#
# The only time we should need to switch to a higher version of ign-cmake is
# when importing targets for dependencies, i.e. using the find-modules of
# ign-cmake. All find-modules that are available in older versions of ign-cmake
# should be available in newer versions of ign-cmake, so this should not create
# any conflicts.
if( NOT IGNITION_CMAKE_VERSION_MAJOR
    OR (IGNITION_CMAKE_VERSION_MAJOR LESS 2) )

  #--------------------------------------
  # Create a variable to indicate what version of ignition-cmake we are using.
  # This variable does not follow the usual cmake naming convention because it
  # is a non-standard package variable.
  set(IGNITION_CMAKE_VERSION_MAJOR 2)

  #--------------------------------------
  # Initialize the IGNITION_CMAKE_DIR variable with the location of the cmake
  # directory that sits next to this find-module.
  set(IGNITION_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake2")

  #--------------------------------------
  # Add the location of this package's cmake directory to the CMAKE_MODULE_PATH
  list(APPEND CMAKE_MODULE_PATH "${IGNITION_CMAKE_DIR}")

  #--------------------------------------
  # include the master IgnCMake module
  include(IgnCMake)

  set(IGNITION_CMAKE_DOXYGEN_DIR "${PACKAGE_PREFIX_DIR}/share/ignition/ignition-cmake2/doxygen")
  set(IGNITION_CMAKE_CODECHECK_DIR "${PACKAGE_PREFIX_DIR}/share/ignition/ignition-cmake2/codecheck")
  set(IGNITION_CMAKE_BENCHMARK_DIR "${PACKAGE_PREFIX_DIR}/share/ignition/ignition-cmake2/benchmark")
  set(IGNITION_CMAKE_TOOLS_DIR "${PACKAGE_PREFIX_DIR}/share/ignition/ignition-cmake2/tools")

endif()
