cmake_minimum_required(VERSION 3.8)
project(pointcloud2_deal)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL 1.12 REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

add_executable(pointcloud2_deal src/pointcloud2_deal.cpp)

target_include_directories(pointcloud2_deal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_compile_features(pointcloud2_deal PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(
  pointcloud2_deal
  "rclcpp"
  "sensor_msgs"
  "pcl_conversions"
)

target_link_libraries(pointcloud2_deal
${PCL_LIBRARIES}
)


install(TARGETS pointcloud2_deal
  DESTINATION lib/${PROJECT_NAME})

ament_export_dependencies(rosidl_default_runtime)
ament_export_include_directories(${PCL_INCLUDE_DIRS})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
