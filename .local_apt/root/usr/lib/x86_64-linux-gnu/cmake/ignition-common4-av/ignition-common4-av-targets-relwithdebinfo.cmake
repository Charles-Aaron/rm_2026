#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-common4::ignition-common4-av" for configuration "RelWithDebInfo"
set_property(TARGET ignition-common4::ignition-common4-av APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-common4::ignition-common4-av PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-common4-av.so.4.7.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-common4-av.so.4"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-common4::ignition-common4-av )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-common4::ignition-common4-av "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-common4-av.so.4.7.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
