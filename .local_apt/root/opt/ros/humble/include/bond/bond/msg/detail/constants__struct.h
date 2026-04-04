// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from bond:msg/Constants.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__CONSTANTS__STRUCT_H_
#define BOND__MSG__DETAIL__CONSTANTS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Constant 'DEAD_PUBLISH_PERIOD'.
static const float bond__msg__Constants__DEAD_PUBLISH_PERIOD = 0.05f;

/// Constant 'DEFAULT_CONNECT_TIMEOUT'.
static const float bond__msg__Constants__DEFAULT_CONNECT_TIMEOUT = 10.0f;

/// Constant 'DEFAULT_HEARTBEAT_TIMEOUT'.
static const float bond__msg__Constants__DEFAULT_HEARTBEAT_TIMEOUT = 4.0f;

/// Constant 'DEFAULT_DISCONNECT_TIMEOUT'.
static const float bond__msg__Constants__DEFAULT_DISCONNECT_TIMEOUT = 2.0f;

/// Constant 'DEFAULT_HEARTBEAT_PERIOD'.
static const float bond__msg__Constants__DEFAULT_HEARTBEAT_PERIOD = 1.0f;

/// Constant 'DISABLE_HEARTBEAT_TIMEOUT_PARAM'.
static const char * const bond__msg__Constants__DISABLE_HEARTBEAT_TIMEOUT_PARAM = "/bond_disable_heartbeat_timeout";

/// Struct defined in msg/Constants in the package bond.
typedef struct bond__msg__Constants
{
  uint8_t structure_needs_at_least_one_member;
} bond__msg__Constants;

// Struct for a sequence of bond__msg__Constants.
typedef struct bond__msg__Constants__Sequence
{
  bond__msg__Constants * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} bond__msg__Constants__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // BOND__MSG__DETAIL__CONSTANTS__STRUCT_H_
