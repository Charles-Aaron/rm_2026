#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-gazebo6::ignition-gazebo6-wind-effects-system" for configuration "RelWithDebInfo"
set_property(TARGET ignition-gazebo6::ignition-gazebo6-wind-effects-system APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-gazebo6::ignition-gazebo6-wind-effects-system PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gazebo6-wind-effects-system.so.6.16.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-gazebo6-wind-effects-system.so.6"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-gazebo6::ignition-gazebo6-wind-effects-system )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-gazebo6::ignition-gazebo6-wind-effects-system "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gazebo6-wind-effects-system.so.6.16.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
