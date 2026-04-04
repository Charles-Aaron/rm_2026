// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from bond:msg/Status.idl
// generated code does not contain a copyright notice
#include "bond/msg/detail/status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `id`
// Member `instance_id`
#include "rosidl_runtime_c/string_functions.h"

bool
bond__msg__Status__init(bond__msg__Status * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    bond__msg__Status__fini(msg);
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    bond__msg__Status__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    bond__msg__Status__fini(msg);
    return false;
  }
  // active
  // heartbeat_timeout
  // heartbeat_period
  return true;
}

void
bond__msg__Status__fini(bond__msg__Status * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // id
  rosidl_runtime_c__String__fini(&msg->id);
  // instance_id
  rosidl_runtime_c__String__fini(&msg->instance_id);
  // active
  // heartbeat_timeout
  // heartbeat_period
}

bool
bond__msg__Status__are_equal(const bond__msg__Status * lhs, const bond__msg__Status * rhs)
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
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->instance_id), &(rhs->instance_id)))
  {
    return false;
  }
  // active
  if (lhs->active != rhs->active) {
    return false;
  }
  // heartbeat_timeout
  if (lhs->heartbeat_timeout != rhs->heartbeat_timeout) {
    return false;
  }
  // heartbeat_period
  if (lhs->heartbeat_period != rhs->heartbeat_period) {
    return false;
  }
  return true;
}

bool
bond__msg__Status__copy(
  const bond__msg__Status * input,
  bond__msg__Status * output)
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
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__copy(
      &(input->instance_id), &(output->instance_id)))
  {
    return false;
  }
  // active
  output->active = input->active;
  // heartbeat_timeout
  output->heartbeat_timeout = input->heartbeat_timeout;
  // heartbeat_period
  output->heartbeat_period = input->heartbeat_period;
  return true;
}

bond__msg__Status *
bond__msg__Status__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  bond__msg__Status * msg = (bond__msg__Status *)allocator.allocate(sizeof(bond__msg__Status), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(bond__msg__Status));
  bool success = bond__msg__Status__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
bond__msg__Status__destroy(bond__msg__Status * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    bond__msg__Status__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
bond__msg__Status__Sequence__init(bond__msg__Status__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  bond__msg__Status * data = NULL;

  if (size) {
    data = (bond__msg__Status *)allocator.zero_allocate(size, sizeof(bond__msg__Status), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = bond__msg__Status__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        bond__msg__Status__fini(&data[i - 1]);
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
bond__msg__Status__Sequence__fini(bond__msg__Status__Sequence * array)
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
      bond__msg__Status__fini(&array->data[i]);
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

bond__msg__Status__Sequence *
bond__msg__Status__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  bond__msg__Status__Sequence * array = (bond__msg__Status__Sequence *)allocator.allocate(sizeof(bond__msg__Status__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = bond__msg__Status__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
bond__msg__Status__Sequence__destroy(bond__msg__Status__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    bond__msg__Status__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
bond__msg__Status__Sequence__are_equal(const bond__msg__Status__Sequence * lhs, const bond__msg__Status__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!bond__msg__Status__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
bond__msg__Status__Sequence__copy(
  const bond__msg__Status__Sequence * input,
  bond__msg__Status__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(bond__msg__Status);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    bond__msg__Status * data =
      (bond__msg__Status *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!bond__msg__Status__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          bond__msg__Status__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!bond__msg__Status__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
