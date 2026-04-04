// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actuator_msgs:msg/ActuatorsNormalized.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__STRUCT_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__STRUCT_H_

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
// Member 'normalized'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/ActuatorsNormalized in the package actuator_msgs.
typedef struct actuator_msgs__msg__ActuatorsNormalized
{
  std_msgs__msg__Header header;
  /// This message defines lowest level commands to be sent
  /// to the actuator(s) for normalized outputs.
  /// Normalized between.
  rosidl_runtime_c__double__Sequence normalized;
} actuator_msgs__msg__ActuatorsNormalized;

// Struct for a sequence of actuator_msgs__msg__ActuatorsNormalized.
typedef struct actuator_msgs__msg__ActuatorsNormalized__Sequence
{
  actuator_msgs__msg__ActuatorsNormalized * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actuator_msgs__msg__ActuatorsNormalized__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__STRUCT_H_
