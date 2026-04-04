// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_H_
#define ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'sim_time'
// Member 'pause_time'
// Member 'real_time'
// Member 'step_size'
#include "builtin_interfaces/msg/detail/time__struct.h"
// Member 'log_playback_statistics'
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__struct.h"

/// Struct defined in msg/WorldStatistics in the package ros_gz_interfaces.
/**
  * optional header data
 */
typedef struct ros_gz_interfaces__msg__WorldStatistics
{
  std_msgs__msg__Header header;
  /// Current simulation time
  builtin_interfaces__msg__Time sim_time;
  /// Total time spent paused
  builtin_interfaces__msg__Time pause_time;
  /// Current real time
  builtin_interfaces__msg__Time real_time;
  /// Whether currently paused
  bool paused;
  /// Current iteration count
  uint64_t iterations;
  /// Total number of models in the world
  int32_t model_count;
  /// Statistics for log playback
  ros_gz_interfaces__msg__LogPlaybackStatistics log_playback_statistics;
  /// This factor expresses how much real time elapses with each step
  /// of simulation time.
  double real_time_factor;
  /// Iteration step size. It's zero when paused.
  builtin_interfaces__msg__Time step_size;
} ros_gz_interfaces__msg__WorldStatistics;

// Struct for a sequence of ros_gz_interfaces__msg__WorldStatistics.
typedef struct ros_gz_interfaces__msg__WorldStatistics__Sequence
{
  ros_gz_interfaces__msg__WorldStatistics * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ros_gz_interfaces__msg__WorldStatistics__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_H_
