cmake_minimum_required(VERSION 3.16)
project(scan_context_relocalization LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(localization_interfaces REQUIRED)
find_package(frame_broadcaster REQUIRED)
find_package(PCL REQUIRED COMPONENTS common io filters kdtree)
find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED COMPONENTS tbb)
find_package(OpenMP REQUIRED COMPONENTS CXX)
find_package(Threads REQUIRED)
find_path(NANOFLANN_INCLUDE_DIR nanoflann.hpp REQUIRED)
set(Z_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vendor/z_backend)
set(Z_SOURCES
  config.cpp covariance.cpp measurement.cpp pose_conditioning.cpp sampling.cpp
  descriptor.cpp keyframe_map.cpp map_bundle.cpp cloud_storage.cpp directory.cpp logger.cpp
  registration.cpp registration_solver.cpp registration_quality.cpp target_cache.cpp
  relocalization.cpp relocalization_hypotheses.cpp relocalization_verification.cpp
  scan_context/descriptor.cpp scan_context/storage.cpp
  gprobe/descriptor.cpp gprobe/geometry.cpp gprobe/retrieval.cpp gprobe/storage.cpp)
list(TRANSFORM Z_SOURCES PREPEND "${Z_ROOT}/src/")
add_library(z_relocalization_core STATIC ${Z_SOURCES})
target_include_directories(z_relocalization_core PUBLIC ${Z_ROOT}/include ${PCL_INCLUDE_DIRS}
  PRIVATE ${Z_ROOT}/src ${NANOFLANN_INCLUDE_DIR})
target_link_libraries(z_relocalization_core PUBLIC ${PCL_LIBRARIES} Eigen3::Eigen
  TBB::tbb OpenMP::OpenMP_CXX Threads::Threads)

add_executable(scan_context_probe src/probe.cpp)
ament_target_dependencies(scan_context_probe rclcpp sensor_msgs nav_msgs geometry_msgs
  pcl_conversions localization_interfaces visualization_msgs)
target_link_libraries(scan_context_probe z_relocalization_core)

add_executable(zlio_navigation src/navigation.cpp)
ament_target_dependencies(zlio_navigation rclcpp sensor_msgs nav_msgs geometry_msgs
  pcl_conversions localization_interfaces visualization_msgs tf2_ros std_msgs std_srvs frame_broadcaster)
target_link_libraries(zlio_navigation z_relocalization_core)
target_compile_options(zlio_navigation PRIVATE -Wall -Wextra -Wpedantic)

# Original upstream map generator (including its occlusion model and bundle format).
add_executable(build_pcd_map_bundle ${Z_ROOT}/tools/build.cpp
  ${Z_ROOT}/tools/generate.cpp ${Z_ROOT}/tools/bundle.cpp)
target_include_directories(build_pcd_map_bundle PRIVATE ${Z_ROOT}/tools)
target_link_libraries(build_pcd_map_bundle z_relocalization_core)
install(TARGETS scan_context_probe zlio_navigation build_pcd_map_bundle DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch rviz DESTINATION share/${PROJECT_NAME})
install(FILES vendor/z_backend/LICENSE
  DESTINATION share/${PROJECT_NAME})
ament_package()
