// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from actuator_msgs:msg/ActuatorsPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__FUNCTIONS_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "actuator_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "actuator_msgs/msg/detail/actuators_position__struct.h"

/// Initialize msg/ActuatorsPosition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * actuator_msgs__msg__ActuatorsPosition
 * )) before or use
 * actuator_msgs__msg__ActuatorsPosition__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsPosition__init(actuator_msgs__msg__ActuatorsPosition * msg);

/// Finalize msg/ActuatorsPosition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsPosition__fini(actuator_msgs__msg__ActuatorsPosition * msg);

/// Create msg/ActuatorsPosition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * actuator_msgs__msg__ActuatorsPosition__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__ActuatorsPosition *
actuator_msgs__msg__ActuatorsPosition__create();

/// Destroy msg/ActuatorsPosition message.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsPosition__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsPosition__destroy(actuator_msgs__msg__ActuatorsPosition * msg);

/// Check for msg/ActuatorsPosition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsPosition__are_equal(const actuator_msgs__msg__ActuatorsPosition * lhs, const actuator_msgs__msg__ActuatorsPosition * rhs);

/// Copy a msg/ActuatorsPosition message.
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
actuator_msgs__msg__ActuatorsPosition__copy(
  const actuator_msgs__msg__ActuatorsPosition * input,
  actuator_msgs__msg__ActuatorsPosition * output);

/// Initialize array of msg/ActuatorsPosition messages.
/**
 * It allocates the memory for the number of elements and calls
 * actuator_msgs__msg__ActuatorsPosition__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsPosition__Sequence__init(actuator_msgs__msg__ActuatorsPosition__Sequence * array, size_t size);

/// Finalize array of msg/ActuatorsPosition messages.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsPosition__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsPosition__Sequence__fini(actuator_msgs__msg__ActuatorsPosition__Sequence * array);

/// Create array of msg/ActuatorsPosition messages.
/**
 * It allocates the memory for the array and calls
 * actuator_msgs__msg__ActuatorsPosition__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__ActuatorsPosition__Sequence *
actuator_msgs__msg__ActuatorsPosition__Sequence__create(size_t size);

/// Destroy array of msg/ActuatorsPosition messages.
/**
 * It calls
 * actuator_msgs__msg__ActuatorsPosition__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__ActuatorsPosition__Sequence__destroy(actuator_msgs__msg__ActuatorsPosition__Sequence * array);

/// Check for msg/ActuatorsPosition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__ActuatorsPosition__Sequence__are_equal(const actuator_msgs__msg__ActuatorsPosition__Sequence * lhs, const actuator_msgs__msg__ActuatorsPosition__Sequence * rhs);

/// Copy an array of msg/ActuatorsPosition messages.
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
actuator_msgs__msg__ActuatorsPosition__Sequence__copy(
  const actuator_msgs__msg__ActuatorsPosition__Sequence * input,
  actuator_msgs__msg__ActuatorsPosition__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__FUNCTIONS_H_
