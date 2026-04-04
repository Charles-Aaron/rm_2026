# - Config file for the ignition-common4 package.
#
# For finding and loading ignition-common4 from your project, type:
#
# find_package(ignition-common4)
#
# This creates the following targets:
#
#   Core library target                     - ignition-common4::ignition-common4
#   Alternative target name                 - ignition-common4::core
#   Core library + all requested components - ignition-common4::requested
#
# Use target_link_libraries() to link your library or executable to one of the
# above targets.
#
# We also provide the following variables for backwards compatibility, but use
# of these is discouraged:
#
#   ignition-common4_CORE_LIBRARY - Core library (actually contains ignition-common4::core)
#   ignition-common4_LIBRARIES    - All libraries to link against (actually contains ignition-common4::requested).
#   ignition-common4_INCLUDE_DIRS - Include directories for ignition-common4 and its dependencies.
#
# We will also set ignition-common4_FOUND to indicate that the package was found.
#
################################################################################

# We explicitly set the desired cmake version to ensure that the policy settings
# of users or of toolchains do not result in the wrong behavior for our modules.
# Note that the call to find_package(~) will PUSH a new policy stack before
# taking on these version settings, and then that stack will POP after the
# find_package(~) has exited, so this will not affect the cmake policy settings
# of a caller.
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(NOT ignition-common4_FIND_QUIETLY)
  message(STATUS "Looking for ignition-common4 -- found version 4.7.0")
endif()

if(ignition-common4_CONFIG_INCLUDED)

  # Check that each of the components requested in this call to find_package(~)
  # have already been found.
  set(ignition-common4_all_requested_components_found true)
  foreach(component ${ignition-common4_FIND_COMPONENTS})
    if(NOT ignition-common4-${component}_FOUND)
      set(ignition-common4_all_requested_components_found false)
      break()
    endif()
  endforeach()

  # If this ignition-common4-config.cmake file has been called before,
  # and all the requested components are already found, then exit early.
  if(ignition-common4_all_requested_components_found)
    return()
  endif()
endif()
set(ignition-common4_CONFIG_INCLUDED TRUE)

# Get access to the find_dependency utility
include(CMakeFindDependencyMacro)
# Find ignition-cmake, because we need its modules in order to find the rest of
# our dependencies.
find_dependency(ignition-cmake2)

# Set the REQUIRED flag for the find_package(~) calls on this project's
# dependencies.
if(ignition-common4_FIND_REQUIRED)
  set(ign_package_required REQUIRED)
else()
  set(ign_package_required "")
endif()

# Set the QUIET flag for the find_package(~) calls on this project's
# dependencies.
if(ignition-common4_FIND_QUIETLY)
  set(ign_package_quiet QUIET)
else()
  set(ign_package_quiet "")
endif()

# --------------------------------
# Find each required dependency of this project (if nothing is below, then the
# project has no external dependencies). We use find_package(~) instead of
# find_dependency(~) here so that we can support COMPONENT arguments.
#
# TODO: When we migrate to cmake-3.9+, change these to find_dependency(~),
#       because at that point the find_dependency(~) function will support
#       the COMPONENT argument.
if(NOT ignition-common4_FIND_QUIETLY)
  message(STATUS "Searching for dependencies of ignition-common4")
endif()
find_package(DL ${ign_package_quiet} ${ign_package_required})
find_package(UUID ${ign_package_quiet} ${ign_package_required})
# --------------------------------


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ignition-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../" ABSOLUTE)

# Use original install prefix when loaded through a "/usr move"
# cross-prefix symbolic link such as /lib -> /usr/lib.
get_filename_component(_realCurr "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(_realOrig "/usr/lib/x86_64-linux-gnu/cmake/ignition-common4" REALPATH)
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

if(NOT TARGET ignition-common4::ignition-common4)
  include("${CMAKE_CURRENT_LIST_DIR}/ignition-common4-targets.cmake")

  # Create a simplified imported target name for the core library.
  # You can link to this target instead of the core library.
  add_library(ignition-common4::core INTERFACE IMPORTED)
  set_target_properties(ignition-common4::core PROPERTIES
    INTERFACE_LINK_LIBRARIES ignition-common4::ignition-common4)
  # Note: In a future version of cmake, we can replace this with an ALIAS target

  # In case someone tries to link against the plain library name, we want to
  # intercept that and have them link against a target instead. This is
  # effectively the same as ignition-common4::core, but it does not have the
  # benefit of unambiguously being a target name.
  add_library(ignition-common4 INTERFACE IMPORTED)
  set_target_properties(ignition-common4 PROPERTIES
    INTERFACE_LINK_LIBRARIES ignition-common4::ignition-common4)

endif()

# Create the "all" target if it does not already exist
if(NOT TARGET ignition-common4::requested)
  add_library(ignition-common4::requested INTERFACE IMPORTED)
endif()

# Link the core library to the "all" target. We set the property explicitly
# because target_link_libraries cannot be called on an imported target.
get_target_property(ign_all_components ignition-common4::requested INTERFACE_LINK_LIBRARIES)
if(NOT ign_all_components)
  # If ignition-common4::requested has not been given any libraries yet, then cmake will
  # set ign_all_components to ign_all_components-NOTFOUND, which is something we
  # should NOT pass into the INTERFACE_LINK_LIBRARIES property.
  set_target_properties(ignition-common4::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "ignition-common4::ignition-common4")
else()
  set_target_properties(ignition-common4::requested PROPERTIES
    INTERFACE_LINK_LIBRARIES "${ign_all_components};ignition-common4::ignition-common4")
endif()

# Package variables. Note that ignition-common4_LIBRARIES and ignition-common4_CORE_LIBRARY
# contain imported targets, so ignition-common4_INCLUDE_DIRS is never needed.
set(ignition-common4_CORE_LIBRARY ignition-common4::core)
set(ignition-common4_LIBRARIES ignition-common4::requested)
set_and_check(ignition-common4_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include/ignition/common4")

# Backwards compatibility variables
set(IGNITION-COMMON_LIBRARIES ${ignition-common4_LIBRARIES})
set(IGNITION-COMMON_INCLUDE_DIRS ${ignition-common4_INCLUDE_DIRS})

# This macro is used by ignition-cmake to automatically configure the pkgconfig
# files for ignition projects.
ign_pkg_config_entry(ignition-common4 "ignition-common4")

# Find each of the components requested by find_package(~)
foreach(component ${ignition-common4_FIND_COMPONENTS})

  if(NOT ignition-common4_FIND_QUIETLY)
    message(STATUS "Searching for <ignition-common4> component [${component}]")
  endif()

  if(ignition-common4_FIND_REQUIRED_${component})

    # Find the component package using find_dependency(~). If the user specified
    # REQUIRED or QUIET, those will automatically get forwarded to
    # find_dependency(~)
    find_dependency(ignition-common4-${component} 4.7.0 EXACT)

  else()

    # If this is an optional component, use find_package(~) instead of
    # find_dependency(~) so we can ensure that the use of REQUIRED does not get
    # forwarded to it.
    find_package(ignition-common4-${component} 4.7.0 EXACT ${ign_package_quiet})

  endif()

endforeach()

# Specify the doxygen tag file
set(IGNITION-COMMON_DOXYGEN_TAGFILE "${PACKAGE_PREFIX_DIR}/share/ignition/ignition-common4/ignition-common4.tag.xml")

# Specify the API url. This is where the doxygen tag file will resolve URLS to.
set(IGNITION-COMMON_API_URL "https://gazebosim.org/api/common/4")
