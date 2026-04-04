// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actuator_msgs:msg/ActuatorsLinearPosition.idl
// generated code does not contain a copyright notice
#include "actuator_msgs/msg/detail/actuators_linear_position__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `position`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
actuator_msgs__msg__ActuatorsLinearPosition__init(actuator_msgs__msg__ActuatorsLinearPosition * msg)
{
  if (!msg) {
    return false;
  }
  // position
  if (!rosidl_runtime_c__double__Sequence__init(&msg->position, 0)) {
    actuator_msgs__msg__ActuatorsLinearPosition__fini(msg);
    return false;
  }
  return true;
}

void
actuator_msgs__msg__ActuatorsLinearPosition__fini(actuator_msgs__msg__ActuatorsLinearPosition * msg)
{
  if (!msg) {
    return;
  }
  // position
  rosidl_runtime_c__double__Sequence__fini(&msg->position);
}

bool
actuator_msgs__msg__ActuatorsLinearPosition__are_equal(const actuator_msgs__msg__ActuatorsLinearPosition * lhs, const actuator_msgs__msg__ActuatorsLinearPosition * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // position
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->position), &(rhs->position)))
  {
    return false;
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsLinearPosition__copy(
  const actuator_msgs__msg__ActuatorsLinearPosition * input,
  actuator_msgs__msg__ActuatorsLinearPosition * output)
{
  if (!input || !output) {
    return false;
  }
  // position
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->position), &(output->position)))
  {
    return false;
  }
  return true;
}

actuator_msgs__msg__ActuatorsLinearPosition *
actuator_msgs__msg__ActuatorsLinearPosition__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsLinearPosition * msg = (actuator_msgs__msg__ActuatorsLinearPosition *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsLinearPosition), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actuator_msgs__msg__ActuatorsLinearPosition));
  bool success = actuator_msgs__msg__ActuatorsLinearPosition__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actuator_msgs__msg__ActuatorsLinearPosition__destroy(actuator_msgs__msg__ActuatorsLinearPosition * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actuator_msgs__msg__ActuatorsLinearPosition__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__init(actuator_msgs__msg__ActuatorsLinearPosition__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsLinearPosition * data = NULL;

  if (size) {
    data = (actuator_msgs__msg__ActuatorsLinearPosition *)allocator.zero_allocate(size, sizeof(actuator_msgs__msg__ActuatorsLinearPosition), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actuator_msgs__msg__ActuatorsLinearPosition__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actuator_msgs__msg__ActuatorsLinearPosition__fini(&data[i - 1]);
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
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__fini(actuator_msgs__msg__ActuatorsLinearPosition__Sequence * array)
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
      actuator_msgs__msg__ActuatorsLinearPosition__fini(&array->data[i]);
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

actuator_msgs__msg__ActuatorsLinearPosition__Sequence *
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actuator_msgs__msg__ActuatorsLinearPosition__Sequence * array = (actuator_msgs__msg__ActuatorsLinearPosition__Sequence *)allocator.allocate(sizeof(actuator_msgs__msg__ActuatorsLinearPosition__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actuator_msgs__msg__ActuatorsLinearPosition__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__destroy(actuator_msgs__msg__ActuatorsLinearPosition__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actuator_msgs__msg__ActuatorsLinearPosition__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__are_equal(const actuator_msgs__msg__ActuatorsLinearPosition__Sequence * lhs, const actuator_msgs__msg__ActuatorsLinearPosition__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsLinearPosition__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actuator_msgs__msg__ActuatorsLinearPosition__Sequence__copy(
  const actuator_msgs__msg__ActuatorsLinearPosition__Sequence * input,
  actuator_msgs__msg__ActuatorsLinearPosition__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actuator_msgs__msg__ActuatorsLinearPosition);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actuator_msgs__msg__ActuatorsLinearPosition * data =
      (actuator_msgs__msg__ActuatorsLinearPosition *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actuator_msgs__msg__ActuatorsLinearPosition__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actuator_msgs__msg__ActuatorsLinearPosition__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actuator_msgs__msg__ActuatorsLinearPosition__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
