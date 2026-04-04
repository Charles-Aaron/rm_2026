// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actuator_msgs:msg/ActuatorsLinearPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'position'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/ActuatorsLinearPosition in the package actuator_msgs.
/**
  * This message defines lowest level commands to be sent
  * to the actuator(s) for linear position.
 */
typedef struct actuator_msgs__msg__ActuatorsLinearPosition
{
  /// Position of the actuators in
  rosidl_runtime_c__double__Sequence position;
} actuator_msgs__msg__ActuatorsLinearPosition;

// Struct for a sequence of actuator_msgs__msg__ActuatorsLinearPosition.
typedef struct actuator_msgs__msg__ActuatorsLinearPosition__Sequence
{
  actuator_msgs__msg__ActuatorsLinearPosition * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actuator_msgs__msg__ActuatorsLinearPosition__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_H_
