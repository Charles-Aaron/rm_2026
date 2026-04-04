#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-transport11::ignition-transport11-log" for configuration "RelWithDebInfo"
set_property(TARGET ignition-transport11::ignition-transport11-log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-transport11::ignition-transport11-log PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-transport11-log.so.11.4.1"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-transport11-log.so.11"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-transport11::ignition-transport11-log )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-transport11::ignition-transport11-log "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-transport11-log.so.11.4.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
