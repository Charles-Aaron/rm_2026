#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ignition-physics5::ignition-physics5-tpelib" for configuration "RelWithDebInfo"
set_property(TARGET ignition-physics5::ignition-physics5-tpelib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ignition-physics5::ignition-physics5-tpelib PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-physics5-tpelib.so.5.3.2"
  IMPORTED_SONAME_RELWITHDEBINFO "libignition-physics5-tpelib.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS ignition-physics5::ignition-physics5-tpelib )
list(APPEND _IMPORT_CHECK_FILES_FOR_ignition-physics5::ignition-physics5-tpelib "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libignition-physics5-tpelib.so.5.3.2" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
