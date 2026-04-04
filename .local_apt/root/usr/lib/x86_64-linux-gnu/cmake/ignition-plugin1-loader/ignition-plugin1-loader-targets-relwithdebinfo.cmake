#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-plugin1::ignition-plugin1-loader" for configuration "RelWithDebInfo"
set_property(TARGET ignition-plugin1::ignition-plugin1-loader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-plugin1::ignition-plugin1-loader PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-plugin1-loader.so.1.4.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-plugin1-loader.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-plugin1::ignition-plugin1-loader )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-plugin1::ignition-plugin1-loader "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-plugin1-loader.so.1.4.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
