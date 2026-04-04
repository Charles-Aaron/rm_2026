#----------------------------------------------------------------
# Generated CMake target import file for configuration "None".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "camera_info_manager::camera_info_manager" for configuration "None"
set_property(TARGET camera_info_manager::camera_info_manager APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(camera_info_manager::camera_info_manager PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "ament_index_cpp::ament_index_cpp;camera_calibration_parsers::camera_calibration_parsers;rcpputils::rcpputils"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/libcamera_info_manager.so"
  IMPORTED_SONAME_NONE "libcamera_info_manager.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS camera_info_manager::camera_info_manager )
list(APPEND _IMPORT_CHECK_FILES_FOR_camera_info_manager::camera_info_manager "${_IMPORT_PREFIX}/lib/libcamera_info_manager.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
