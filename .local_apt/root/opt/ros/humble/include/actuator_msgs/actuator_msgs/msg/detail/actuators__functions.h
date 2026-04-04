// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from actuator_msgs:msg/Actuators.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__FUNCTIONS_H_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "actuator_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "actuator_msgs/msg/detail/actuators__struct.h"

/// Initialize msg/Actuators message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * actuator_msgs__msg__Actuators
 * )) before or use
 * actuator_msgs__msg__Actuators__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__Actuators__init(actuator_msgs__msg__Actuators * msg);

/// Finalize msg/Actuators message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__Actuators__fini(actuator_msgs__msg__Actuators * msg);

/// Create msg/Actuators message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * actuator_msgs__msg__Actuators__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__Actuators *
actuator_msgs__msg__Actuators__create();

/// Destroy msg/Actuators message.
/**
 * It calls
 * actuator_msgs__msg__Actuators__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__Actuators__destroy(actuator_msgs__msg__Actuators * msg);

/// Check for msg/Actuators message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__Actuators__are_equal(const actuator_msgs__msg__Actuators * lhs, const actuator_msgs__msg__Actuators * rhs);

/// Copy a msg/Actuators message.
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
actuator_msgs__msg__Actuators__copy(
  const actuator_msgs__msg__Actuators * input,
  actuator_msgs__msg__Actuators * output);

/// Initialize array of msg/Actuators messages.
/**
 * It allocates the memory for the number of elements and calls
 * actuator_msgs__msg__Actuators__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__Actuators__Sequence__init(actuator_msgs__msg__Actuators__Sequence * array, size_t size);

/// Finalize array of msg/Actuators messages.
/**
 * It calls
 * actuator_msgs__msg__Actuators__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__Actuators__Sequence__fini(actuator_msgs__msg__Actuators__Sequence * array);

/// Create array of msg/Actuators messages.
/**
 * It allocates the memory for the array and calls
 * actuator_msgs__msg__Actuators__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
actuator_msgs__msg__Actuators__Sequence *
actuator_msgs__msg__Actuators__Sequence__create(size_t size);

/// Destroy array of msg/Actuators messages.
/**
 * It calls
 * actuator_msgs__msg__Actuators__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
void
actuator_msgs__msg__Actuators__Sequence__destroy(actuator_msgs__msg__Actuators__Sequence * array);

/// Check for msg/Actuators message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actuator_msgs
bool
actuator_msgs__msg__Actuators__Sequence__are_equal(const actuator_msgs__msg__Actuators__Sequence * lhs, const actuator_msgs__msg__Actuators__Sequence * rhs);

/// Copy an array of msg/Actuators messages.
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
actuator_msgs__msg__Actuators__Sequence__copy(
  const actuator_msgs__msg__Actuators__Sequence * input,
  actuator_msgs__msg__Actuators__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__FUNCTIONS_H_
