// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actuator_msgs:msg/ActuatorsPosition.idl
// generated code does not contain a copyright notice
#include "actuator_msgs/msg/detail/actuators_position__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `angular`
#include "actuator_msgs/msg/detail/actuators_angular_position__functions.h"
// Member `linear`
#include "actuator_msgs/msg/detail/actuators_linear_position__functions.h"

bool
actuator_msgs__msg__ActuatorsPosition__init(actuator_msgs__msg__ActuatorsPosition * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    actuator_msgs__msg__ActuatorsPosition__fini(msg);
    return false;
  }
  // angular
  if (!actuator_msgs__msg__ActuatorsAngularPosition__init(&msg->angular)) {
    actuator_msgs__msg__ActuatorsPosition__fini(msg);
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearPosition__init(&msg->linear)) {
    actuator_msgs__msg__ActuatorsPosition__fini(msg);
    return false;
  }
  return true;
}

void
actuator_msgs__msg__ActuatorsPosition__fini(actuator_msgs__msg__ActuatorsPosition * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // angular
  actuator_msgs__msg__ActuatorsAngularPosition__fini(&msg->angular);
  // linear
  actuator_msgs__msg__ActuatorsLinearPosition__fini(&msg->linear);
}

bool
actuator_msgs__msg__ActuatorsPosition__are_equal(const actuator_msgs__msg__ActuatorsPosition * lhs, const actuator_msgs__msg__ActuatorsPosition * rhs)
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
  if (!actuator_msgs__msg__ActuatorsAngularPosition__are_equal(
      &(lhs->angular), &(rhs->angular)))
  {
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearPosition__are_equal(
      &(lhs->linear), &(rhs->linear)))
  {
    return false;
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsPosition__copy(
  const actuator_msgs__msg__ActuatorsPosition * input,
  actuator_msgs__msg__ActuatorsPosition * output)
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
  if (!actuator_msgs__msg__ActuatorsAngularPosition__copy(
      &(input->angular), &(output->angular)))
  {
    return false;
  }
  // linear
  if (!actuator_msgs__msg__ActuatorsLinearPosition__copy(
      &(input->linear), &(output->linear)))
  {
    return false;
  }
  return true;
}

actuator_msgs__msg__ActuatorsPosition *
actuator_msgs__msg__ActuatorsPosition__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsPosition * msg = (actuator_msgs__msg__ActuatorsPosition *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsPosition), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actuator_msgs__msg__ActuatorsPosition));
  bool success = actuator_msgs__msg__ActuatorsPosition__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actuator_msgs__msg__ActuatorsPosition__destroy(actuator_msgs__msg__ActuatorsPosition * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actuator_msgs__msg__ActuatorsPosition__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actuator_msgs__msg__ActuatorsPosition__Sequence__init(actuator_msgs__msg__ActuatorsPosition__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsPosition * data = NULL;

  if (size) {
    data = (actuator_msgs__msg__ActuatorsPosition *)allocator.zero_allocate(size, sizeof(actuator_msgs__msg__ActuatorsPosition), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actuator_msgs__msg__ActuatorsPosition__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actuator_msgs__msg__ActuatorsPosition__fini(&data[i - 1]);
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
actuator_msgs__msg__ActuatorsPosition__Sequence__fini(actuator_msgs__msg__ActuatorsPosition__Sequence * array)
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
      actuator_msgs__msg__ActuatorsPosition__fini(&array->data[i]);
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

actuator_msgs__msg__ActuatorsPosition__Sequence *
actuator_msgs__msg__ActuatorsPosition__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsPosition__Sequence * array = (actuator_msgs__msg__ActuatorsPosition__Sequence *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsPosition__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actuator_msgs__msg__ActuatorsPosition__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actuator_msgs__msg__ActuatorsPosition__Sequence__destroy(actuator_msgs__msg__ActuatorsPosition__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actuator_msgs__msg__ActuatorsPosition__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actuator_msgs__msg__ActuatorsPosition__Sequence__are_equal(const actuator_msgs__msg__ActuatorsPosition__Sequence * lhs, const actuator_msgs__msg__ActuatorsPosition__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsPosition__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsPosition__Sequence__copy(
  const actuator_msgs__msg__ActuatorsPosition__Sequence * input,
  actuator_msgs__msg__ActuatorsPosition__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actuator_msgs__msg__ActuatorsPosition);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actuator_msgs__msg__ActuatorsPosition * data =
      (actuator_msgs__msg__ActuatorsPosition *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actuator_msgs__msg__ActuatorsPosition__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actuator_msgs__msg__ActuatorsPosition__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsPosition__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
