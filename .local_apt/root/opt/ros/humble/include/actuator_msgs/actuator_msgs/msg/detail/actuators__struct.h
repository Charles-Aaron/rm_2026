// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actuator_msgs:msg/Actuators.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__STRUCT_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__STRUCT_H_

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
// Member 'position'
// Member 'velocity'
// Member 'normalized'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/Actuators in the package actuator_msgs.
typedef struct actuator_msgs__msg__Actuators
{
  std_msgs__msg__Header header;
  /// This message defines lowest level commands to be sent to the actuator(s).
  /// Position of the actuators in [rad] for angular actuators
  /// and [m] for linear actuators.
  rosidl_runtime_c__double__Sequence position;
  /// Velocities of the actuators in [rad/s] for angular actuators
  /// and [m/s] for linear actuators.
  rosidl_runtime_c__double__Sequence velocity;
  /// Everything that does not fit the above,
  /// normalized between.
  rosidl_runtime_c__double__Sequence normalized;
} actuator_msgs__msg__Actuators;

// Struct for a sequence of actuator_msgs__msg__Actuators.
typedef struct actuator_msgs__msg__Actuators__Sequence
{
  actuator_msgs__msg__Actuators * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actuator_msgs__msg__Actuators__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__STRUCT_H_
