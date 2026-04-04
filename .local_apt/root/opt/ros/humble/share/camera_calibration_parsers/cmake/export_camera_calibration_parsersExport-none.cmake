#----------------------------------------------------------------
# Generated CMake target import file for configuration "None".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "camera_calibration_parsers::camera_calibration_parsers" for configuration "None"
set_property(TARGET camera_calibration_parsers::camera_calibration_parsers APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(camera_calibration_parsers::camera_calibration_parsers PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "rclcpp::rclcpp;rcpputils::rcpputils"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/libcamera_calibration_parsers.so"
  IMPORTED_SONAME_NONE "libcamera_calibration_parsers.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS camera_calibration_parsers::camera_calibration_parsers )
list(APPEND _IMPORT_CHECK_FILES_FOR_camera_calibration_parsers::camera_calibration_parsers "${_IMPORT_PREFIX}/lib/libcamera_calibration_parsers.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
