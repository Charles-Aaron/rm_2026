#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-sensors6::ignition-sensors6-rgbd_camera" for configuration "RelWithDebInfo"
set_property(TARGET ignition-sensors6::ignition-sensors6-rgbd_camera APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-sensors6::ignition-sensors6-rgbd_camera PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELWITHDEBINFO "ignition-sensors6::ignition-sensors6-camera;ignition-msgs8::ignition-msgs8;ignition-transport11::ignition-transport11"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-sensors6-rgbd_camera.so.6.8.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-sensors6-rgbd_camera.so.6"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-sensors6::ignition-sensors6-rgbd_camera )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-sensors6::ignition-sensors6-rgbd_camera "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-sensors6-rgbd_camera.so.6.8.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
