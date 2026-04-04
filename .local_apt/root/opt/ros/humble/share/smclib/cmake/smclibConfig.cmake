# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_smclib_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED smclib_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(smclib_FOUND FALSE)
  elseif(NOT smclib_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(smclib_FOUND FALSE)
  endif()
  return()
endif()
set(_smclib_CONFIG_INCLUDED TRUE)

# output package information
if(NOT smclib_FIND_QUIETLY)
  message(STATUS "Found smclib: 4.1.4 (${smclib_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'smclib' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${smclib_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(smclib_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "ament_cmake_export_include_directories-extras.cmake;ament_cmake_export_targets-extras.cmake")
foreach(_extra ${_extras})
  include("${smclib_DIR}/${_extra}")
endforeach()
