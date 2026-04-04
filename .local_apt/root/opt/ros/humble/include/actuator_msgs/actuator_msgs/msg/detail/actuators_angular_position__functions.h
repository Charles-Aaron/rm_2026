// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from actuator_msgs:msg/ActuatorsAngularPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__FUNCTIONS_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "actuator_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "actuator_msgs/msg/detail/actuators_angular_position__struct.h"

/// Initialize msg/ActuatorsAngularPosition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * actuator_msgs__msg__ActuatorsAngularPosition
 * )) before or use
 * actuator_msgs__msg__ActuatorsAngularPosition__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__init(actuator_msgs__msg__ActuatorsAngularPosition * msg);

/// Finalize msg/ActuatorsAngularPosition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsAngularPosition__fini(actuator_msgs__msg__ActuatorsAngularPosition * msg);

/// Create msg/ActuatorsAngularPosition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * actuator_msgs__msg__ActuatorsAngularPosition__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__ActuatorsAngularPosition *
actuator_msgs__msg__ActuatorsAngularPosition__create();

/// Destroy msg/ActuatorsAngularPosition message.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsAngularPosition__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsAngularPosition__destroy(actuator_msgs__msg__ActuatorsAngularPosition * msg);

/// Check for msg/ActuatorsAngularPosition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__are_equal(const actuator_msgs__msg__ActuatorsAngularPosition * lhs, const actuator_msgs__msg__ActuatorsAngularPosition * rhs);

/// Copy a msg/ActuatorsAngularPosition message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__copy(
  const actuator_msgs__msg__ActuatorsAngularPosition * input,
  actuator_msgs__msg__ActuatorsAngularPosition * output);

/// Initialize array of msg/ActuatorsAngularPosition messages.
/**
 * It allocates the memory for the number of elements and calls
 * actuator_msgs__msg__ActuatorsAngularPosition__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__init(actuator_msgs__msg__ActuatorsAngularPosition__Sequence * array, size_t size);

/// Finalize array of msg/ActuatorsAngularPosition messages.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsAngularPosition__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__fini(actuator_msgs__msg__ActuatorsAngularPosition__Sequence * array);

/// Create array of msg/ActuatorsAngularPosition messages.
/**
 * It allocates the memory for the array and calls
 * actuator_msgs__msg__ActuatorsAngularPosition__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__ActuatorsAngularPosition__Sequence *
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__create(size_t size);

/// Destroy array of msg/ActuatorsAngularPosition messages.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsAngularPosition__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__destroy(actuator_msgs__msg__ActuatorsAngularPosition__Sequence * array);

/// Check for msg/ActuatorsAngularPosition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__are_equal(const actuator_msgs__msg__ActuatorsAngularPosition__Sequence * lhs, const actuator_msgs__msg__ActuatorsAngularPosition__Sequence * rhs);

/// Copy an array of msg/ActuatorsAngularPosition messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsAngularPosition__Sequence__copy(
  const actuator_msgs__msg__ActuatorsAngularPosition__Sequence * input,
  actuator_msgs__msg__ActuatorsAngularPosition__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__FUNCTIONS_H_
