// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ros_gz_interfaces:msg/LogPlaybackStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_H_
#define ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_H_

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
// Member 'start_time'
// Member 'end_time'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/LogPlaybackStatistics in the package ros_gz_interfaces.
typedef struct ros_gz_interfaces__msg__LogPlaybackStatistics
{
  std_msgs__msg__Header header;
  /// log start time
  builtin_interfaces__msg__Time start_time;
  /// log end time
  builtin_interfaces__msg__Time end_time;
} ros_gz_interfaces__msg__LogPlaybackStatistics;

// Struct for a sequence of ros_gz_interfaces__msg__LogPlaybackStatistics.
typedef struct ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence
{
  ros_gz_interfaces__msg__LogPlaybackStatistics * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_H_
