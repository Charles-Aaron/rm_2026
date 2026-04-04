#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-gazebo6::ignition-gazebo6-gui" for configuration "RelWithDebInfo"
set_property(TARGET ignition-gazebo6::ignition-gazebo6-gui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-gazebo6::ignition-gazebo6-gui PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gazebo6-gui.so.6.16.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-gazebo6-gui.so.6"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-gazebo6::ignition-gazebo6-gui )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-gazebo6::ignition-gazebo6-gui "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gazebo6-gui.so.6.16.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
