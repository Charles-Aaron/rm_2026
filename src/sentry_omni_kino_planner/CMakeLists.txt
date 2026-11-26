cmake_minimum_required(VERSION 3.16)
project(sentry_omni_kino_planner)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(diagnostic_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(yaml-cpp REQUIRED)
find_package(Threads REQUIRED)

add_library(tdt_kino STATIC
  vendor/tdt_nav/yastar.cpp
  vendor/tdt_nav/kinodynamicAstar.cpp)
target_compile_features(tdt_kino PUBLIC cxx_std_20)
target_include_directories(tdt_kino PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/tdt_nav>
  $<INSTALL_INTERFACE:include/sentry_omni_kino_planner/tdt_nav>)
target_include_directories(tdt_kino SYSTEM PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tdt_kino PUBLIC Eigen3::Eigen ${OpenCV_LIBS} Threads::Threads)
set_target_properties(tdt_kino PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(omni_kino_core src/grid_map.cpp src/planner.cpp src/astar.cpp)
target_compile_features(omni_kino_core PUBLIC cxx_std_20)
target_include_directories(omni_kino_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(omni_kino_core PUBLIC Eigen3::Eigen PRIVATE tdt_kino yaml-cpp)
target_include_directories(omni_kino_core SYSTEM PRIVATE vendor)
set_target_properties(omni_kino_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(omni_kino_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS omni_kino_core tdt_kino EXPORT omni_kino_targets
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(FILES vendor/tdt_nav/yastar.hpp DESTINATION include/sentry_omni_kino_planner/tdt_nav)
ament_export_targets(omni_kino_targets HAS_LIBRARY_TARGET)
ament_export_dependencies(Eigen3 OpenCV yaml-cpp Threads)
install(FILES LICENSE DESTINATION share/${PROJECT_NAME})
install(DIRECTORY vendor/tdt_nav/ DESTINATION share/${PROJECT_NAME}/vendor/tdt_nav
  FILES_MATCHING PATTERN "LICENSE")

ament_package()
