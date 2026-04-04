#----------------------------------------------------------------
# Generated CMake target import file for configuration "None".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bondcpp::bondcpp" for configuration "None"
set_property(TARGET bondcpp::bondcpp APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(bondcpp::bondcpp PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/libbondcpp.so"
  IMPORTED_SONAME_NONE "libbondcpp.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS bondcpp::bondcpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_bondcpp::bondcpp "${_IMPORT_PREFIX}/lib/libbondcpp.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
