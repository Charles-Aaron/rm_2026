#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-gui6::ignition-gui6" for configuration "RelWithDebInfo"
set_property(TARGET ignition-gui6::ignition-gui6 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-gui6::ignition-gui6 PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gui6.so.6.8.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-gui6.so.6"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-gui6::ignition-gui6 )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-gui6::ignition-gui6 "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-gui6.so.6.8.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
