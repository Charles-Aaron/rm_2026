#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-rendering6::ignition-rendering6" for configuration "RelWithDebInfo"
set_property(TARGET ignition-rendering6::ignition-rendering6 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-rendering6::ignition-rendering6 PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-rendering6.so.6.6.3"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-rendering6.so.6"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-rendering6::ignition-rendering6 )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-rendering6::ignition-rendering6 "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-rendering6.so.6.6.3" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
