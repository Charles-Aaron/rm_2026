#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sdformat12::sdformat12" for configuration "RelWithDebInfo"
set_property(TARGET sdformat12::sdformat12 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(sdformat12::sdformat12 PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libsdformat12.so.12.7.2"
  IMPORTED_SONAME_RELWITHDEBINFO "libsdformat12.so.12"
  )

list(APPEND _IMPORT_CHECK_TARGETS sdformat12::sdformat12 )
list(APPEND _IMPORT_CHECK_FILES_FOR_sdformat12::sdformat12 "${_IMPORT_PREFIX}/lib/x86_64-linux-gnu/libsdformat12.so.12.7.2" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
