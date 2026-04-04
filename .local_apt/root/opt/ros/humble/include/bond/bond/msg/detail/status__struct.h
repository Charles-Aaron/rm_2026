// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from bond:msg/Status.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__STATUS__STRUCT_H_
#define BOND__MSG__DETAIL__STATUS__STRUCT_H_

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
// Member 'id'
// Member 'instance_id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Status in the package bond.
typedef struct bond__msg__Status
{
  std_msgs__msg__Header header;
  /// ID of the bond
  rosidl_runtime_c__String id;
  /// Unique ID for an individual in a bond
  rosidl_runtime_c__String instance_id;
  bool active;
  /// Including the timeouts for the bond makes it easier to debug mis-matches
  /// between the two sides.
  float heartbeat_timeout;
  float heartbeat_period;
} bond__msg__Status;

// Struct for a sequence of bond__msg__Status.
typedef struct bond__msg__Status__Sequence
{
  bond__msg__Status * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} bond__msg__Status__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // BOND__MSG__DETAIL__STATUS__STRUCT_H_
