// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice
#include "ros_gz_interfaces/msg/detail/world_statistics__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `sim_time`
// Member `pause_time`
// Member `real_time`
// Member `step_size`
#include "builtin_interfaces/msg/detail/time__functions.h"
// Member `log_playback_statistics`
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__functions.h"

bool
ros_gz_interfaces__msg__WorldStatistics__init(ros_gz_interfaces__msg__WorldStatistics * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  // sim_time
  if (!builtin_interfaces__msg__Time__init(&msg->sim_time)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  // pause_time
  if (!builtin_interfaces__msg__Time__init(&msg->pause_time)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  // real_time
  if (!builtin_interfaces__msg__Time__init(&msg->real_time)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  // paused
  // iterations
  // model_count
  // log_playback_statistics
  if (!ros_gz_interfaces__msg__LogPlaybackStatistics__init(&msg->log_playback_statistics)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  // real_time_factor
  // step_size
  if (!builtin_interfaces__msg__Time__init(&msg->step_size)) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
    return false;
  }
  return true;
}

void
ros_gz_interfaces__msg__WorldStatistics__fini(ros_gz_interfaces__msg__WorldStatistics * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // sim_time
  builtin_interfaces__msg__Time__fini(&msg->sim_time);
  // pause_time
  builtin_interfaces__msg__Time__fini(&msg->pause_time);
  // real_time
  builtin_interfaces__msg__Time__fini(&msg->real_time);
  // paused
  // iterations
  // model_count
  // log_playback_statistics
  ros_gz_interfaces__msg__LogPlaybackStatistics__fini(&msg->log_playback_statistics);
  // real_time_factor
  // step_size
  builtin_interfaces__msg__Time__fini(&msg->step_size);
}

bool
ros_gz_interfaces__msg__WorldStatistics__are_equal(const ros_gz_interfaces__msg__WorldStatistics * lhs, const ros_gz_interfaces__msg__WorldStatistics * rhs)
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
  // sim_time
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->sim_time), &(rhs->sim_time)))
  {
    return false;
  }
  // pause_time
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->pause_time), &(rhs->pause_time)))
  {
    return false;
  }
  // real_time
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->real_time), &(rhs->real_time)))
  {
    return false;
  }
  // paused
  if (lhs->paused != rhs->paused) {
    return false;
  }
  // iterations
  if (lhs->iterations != rhs->iterations) {
    return false;
  }
  // model_count
  if (lhs->model_count != rhs->model_count) {
    return false;
  }
  // log_playback_statistics
  if (!ros_gz_interfaces__msg__LogPlaybackStatistics__are_equal(
      &(lhs->log_playback_statistics), &(rhs->log_playback_statistics)))
  {
    return false;
  }
  // real_time_factor
  if (lhs->real_time_factor != rhs->real_time_factor) {
    return false;
  }
  // step_size
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->step_size), &(rhs->step_size)))
  {
    return false;
  }
  return true;
}

bool
ros_gz_interfaces__msg__WorldStatistics__copy(
  const ros_gz_interfaces__msg__WorldStatistics * input,
  ros_gz_interfaces__msg__WorldStatistics * output)
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
  // sim_time
  if (!builtin_interfaces__msg__Time__copy(
      &(input->sim_time), &(output->sim_time)))
  {
    return false;
  }
  // pause_time
  if (!builtin_interfaces__msg__Time__copy(
      &(input->pause_time), &(output->pause_time)))
  {
    return false;
  }
  // real_time
  if (!builtin_interfaces__msg__Time__copy(
      &(input->real_time), &(output->real_time)))
  {
    return false;
  }
  // paused
  output->paused = input->paused;
  // iterations
  output->iterations = input->iterations;
  // model_count
  output->model_count = input->model_count;
  // log_playback_statistics
  if (!ros_gz_interfaces__msg__LogPlaybackStatistics__copy(
      &(input->log_playback_statistics), &(output->log_playback_statistics)))
  {
    return false;
  }
  // real_time_factor
  output->real_time_factor = input->real_time_factor;
  // step_size
  if (!builtin_interfaces__msg__Time__copy(
      &(input->step_size), &(output->step_size)))
  {
    return false;
  }
  return true;
}

ros_gz_interfaces__msg__WorldStatistics *
ros_gz_interfaces__msg__WorldStatistics__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_gz_interfaces__msg__WorldStatistics * msg = (ros_gz_interfaces__msg__WorldStatistics *)allocator.allocate(sizeof(ros_gz_interfaces__msg__WorldStatistics), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(ros_gz_interfaces__msg__WorldStatistics));
  bool success = ros_gz_interfaces__msg__WorldStatistics__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
ros_gz_interfaces__msg__WorldStatistics__destroy(ros_gz_interfaces__msg__WorldStatistics * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    ros_gz_interfaces__msg__WorldStatistics__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
ros_gz_interfaces__msg__WorldStatistics__Sequence__init(ros_gz_interfaces__msg__WorldStatistics__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_gz_interfaces__msg__WorldStatistics * data = NULL;

  if (size) {
    data = (ros_gz_interfaces__msg__WorldStatistics *)allocator.zero_allocate(size, sizeof(ros_gz_interfaces__msg__WorldStatistics), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = ros_gz_interfaces__msg__WorldStatistics__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        ros_gz_interfaces__msg__WorldStatistics__fini(&data[i - 1]);
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
ros_gz_interfaces__msg__WorldStatistics__Sequence__fini(ros_gz_interfaces__msg__WorldStatistics__Sequence * array)
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
      ros_gz_interfaces__msg__WorldStatistics__fini(&array->data[i]);
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

ros_gz_interfaces__msg__WorldStatistics__Sequence *
ros_gz_interfaces__msg__WorldStatistics__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_gz_interfaces__msg__WorldStatistics__Sequence * array = (ros_gz_interfaces__msg__WorldStatistics__Sequence *)allocator.allocate(sizeof(ros_gz_interfaces__msg__WorldStatistics__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = ros_gz_interfaces__msg__WorldStatistics__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
ros_gz_interfaces__msg__WorldStatistics__Sequence__destroy(ros_gz_interfaces__msg__WorldStatistics__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    ros_gz_interfaces__msg__WorldStatistics__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
ros_gz_interfaces__msg__WorldStatistics__Sequence__are_equal(const ros_gz_interfaces__msg__WorldStatistics__Sequence * lhs, const ros_gz_interfaces__msg__WorldStatistics__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!ros_gz_interfaces__msg__WorldStatistics__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
ros_gz_interfaces__msg__WorldStatistics__Sequence__copy(
  const ros_gz_interfaces__msg__WorldStatistics__Sequence * input,
  ros_gz_interfaces__msg__WorldStatistics__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(ros_gz_interfaces__msg__WorldStatistics);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    ros_gz_interfaces__msg__WorldStatistics * data =
      (ros_gz_interfaces__msg__WorldStatistics *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!ros_gz_interfaces__msg__WorldStatistics__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          ros_gz_interfaces__msg__WorldStatistics__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!ros_gz_interfaces__msg__WorldStatistics__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
