// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actuator_msgs:msg/ActuatorsVelocity.idl
// generated code does not contain a copyright notice
#include "actuator_msgs/msg/detail/actuators_velocity__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `angular`
#include "actuator_msgs/msg/detail/actuators_angular_velocity__functions.h"
// Member `linear`
#include "actuator_msgs/msg/detail/actuators_linear_velocity__functions.h"

bool
actuator_msgs__msg__ActuatorsVelocity__init(actuator_msgs__msg__ActuatorsVelocity * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    actuator_msgs__msg__ActuatorsVelocity__fini(msg);
    return false;
  }
  // angular
  if (!actuator_msgs__msg__ActuatorsAngularVelocity__init(&msg->angular)) {
    actuator_msgs__msg__ActuatorsVelocity__fini(msg);
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearVelocity__init(&msg->linear)) {
    actuator_msgs__msg__ActuatorsVelocity__fini(msg);
    return false;
  }
  return true;
}

void
actuator_msgs__msg__ActuatorsVelocity__fini(actuator_msgs__msg__ActuatorsVelocity * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // angular
  actuator_msgs__msg__ActuatorsAngularVelocity__fini(&msg->angular);
  // linear
  actuator_msgs__msg__ActuatorsLinearVelocity__fini(&msg->linear);
}

bool
actuator_msgs__msg__ActuatorsVelocity__are_equal(const actuator_msgs__msg__ActuatorsVelocity * lhs, const actuator_msgs__msg__ActuatorsVelocity * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // angular
  if (!actuator_msgs__msg__ActuatorsAngularVelocity__are_equal(
      &(lhs->angular), &(rhs->angular)))
  {
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearVelocity__are_equal(
      &(lhs->linear), &(rhs->linear)))
  {
    return false;
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsVelocity__copy(
  const actuator_msgs__msg__ActuatorsVelocity * input,
  actuator_msgs__msg__ActuatorsVelocity * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // angular
  if (!actuator_msgs__msg__ActuatorsAngularVelocity__copy(
      &(input->angular), &(output->angular)))
  {
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearVelocity__copy(
      &(input->linear), &(output->linear)))
  {
    return false;
  }
  return true;
}

actuator_msgs__msg__ActuatorsVelocity *
actuator_msgs__msg__ActuatorsVelocity__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsVelocity * msg = (actuator_msgs__msg__ActuatorsVelocity *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsVelocity), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actuator_msgs__msg__ActuatorsVelocity));
  bool success = actuator_msgs__msg__ActuatorsVelocity__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actuator_msgs__msg__ActuatorsVelocity__destroy(actuator_msgs__msg__ActuatorsVelocity * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actuator_msgs__msg__ActuatorsVelocity__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actuator_msgs__msg__ActuatorsVelocity__Sequence__init(actuator_msgs__msg__ActuatorsVelocity__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsVelocity * data = NULL;

  if (size) {
    data = (actuator_msgs__msg__ActuatorsVelocity *)allocator.zero_allocate(size, sizeof(actuator_msgs__msg__ActuatorsVelocity), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actuator_msgs__msg__ActuatorsVelocity__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actuator_msgs__msg__ActuatorsVelocity__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
actuator_msgs__msg__ActuatorsVelocity__Sequence__fini(actuator_msgs__msg__ActuatorsVelocity__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      actuator_msgs__msg__ActuatorsVelocity__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

actuator_msgs__msg__ActuatorsVelocity__Sequence *
actuator_msgs__msg__ActuatorsVelocity__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsVelocity__Sequence * array = (actuator_msgs__msg__ActuatorsVelocity__Sequence *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsVelocity__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actuator_msgs__msg__ActuatorsVelocity__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actuator_msgs__msg__ActuatorsVelocity__Sequence__destroy(actuator_msgs__msg__ActuatorsVelocity__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actuator_msgs__msg__ActuatorsVelocity__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actuator_msgs__msg__ActuatorsVelocity__Sequence__are_equal(const actuator_msgs__msg__ActuatorsVelocity__Sequence * lhs, const actuator_msgs__msg__ActuatorsVelocity__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsVelocity__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsVelocity__Sequence__copy(
  const actuator_msgs__msg__ActuatorsVelocity__Sequence * input,
  actuator_msgs__msg__ActuatorsVelocity__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actuator_msgs__msg__ActuatorsVelocity);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actuator_msgs__msg__ActuatorsVelocity * data =
      (actuator_msgs__msg__ActuatorsVelocity *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actuator_msgs__msg__ActuatorsVelocity__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actuator_msgs__msg__ActuatorsVelocity__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsVelocity__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
