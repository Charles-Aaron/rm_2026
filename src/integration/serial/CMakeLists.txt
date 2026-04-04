cmake_minimum_required(VERSION 3.8)
project(serial)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

if(APPLE)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(FOUNDATION_LIBRARY Foundation)
endif()

set(serial_SRCS
  src/serial.cc
  include/serial/serial.h
  include/serial/v8stdint.h
)

if(APPLE)
  list(APPEND serial_SRCS src/impl/unix.cc)
  list(APPEND serial_SRCS src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
  list(APPEND serial_SRCS src/impl/unix.cc)
  list(APPEND serial_SRCS src/impl/list_ports/list_ports_linux.cc)
else()
  list(APPEND serial_SRCS src/impl/win.cc)
  list(APPEND serial_SRCS src/impl/list_ports/list_ports_win.cc)
endif()

add_library(${PROJECT_NAME} ${serial_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PUBLIC rt pthread)
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC setupapi)
endif()

add_executable(serial_example examples/serial_example.cc)
target_link_libraries(serial_example PRIVATE ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS serial_example
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include)


ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_package()
