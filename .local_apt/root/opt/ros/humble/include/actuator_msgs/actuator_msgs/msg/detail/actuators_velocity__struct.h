// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actuator_msgs:msg/ActuatorsVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_H_

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
// Member 'angular'
#include "actuator_msgs/msg/detail/actuators_angular_velocity__struct.h"
// Member 'linear'
#include "actuator_msgs/msg/detail/actuators_linear_velocity__struct.h"

/// Struct defined in msg/ActuatorsVelocity in the package actuator_msgs.
typedef struct actuator_msgs__msg__ActuatorsVelocity
{
  std_msgs__msg__Header header;
  /// This message defines lowest level commands to be sent
  /// to the actuator(s) for velocity.
  /// Velocity of the actuators in
  actuator_msgs__msg__ActuatorsAngularVelocity angular;
  /// Velocity of the actuators in
  actuator_msgs__msg__ActuatorsLinearVelocity linear;
} actuator_msgs__msg__ActuatorsVelocity;

// Struct for a sequence of actuator_msgs__msg__ActuatorsVelocity.
typedef struct actuator_msgs__msg__ActuatorsVelocity__Sequence
{
  actuator_msgs__msg__ActuatorsVelocity * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actuator_msgs__msg__ActuatorsVelocity__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_H_
