// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from actuator_msgs:msg/Actuators.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "actuator_msgs/msg/detail/actuators__rosidl_typesupport_introspection_c.h"
#include "actuator_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "actuator_msgs/msg/detail/actuators__functions.h"
#include "actuator_msgs/msg/detail/actuators__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `position`
// Member `velocity`
// Member `normalized`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  actuator_msgs__msg__Actuators__init(message_memory);
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_fini_function(void * message_memory)
{
  actuator_msgs__msg__Actuators__fini(message_memory);
}

size_t actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__position(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__position(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__position(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__position(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__position(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__position(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__position(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__position(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__velocity(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__velocity(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__velocity(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__velocity(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__velocity(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__velocity(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__velocity(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__velocity(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__normalized(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__normalized(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__normalized(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__normalized(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__normalized(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__normalized(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__normalized(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__normalized(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_member_array[4] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__Actuators, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__Actuators, position),  // bytes offset in struct
    NULL,  // default value
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__position,  // size() function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__position,  // get_const(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__position,  // get(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__position,  // fetch(index, &value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__position,  // assign(index, value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__position  // resize(index) function pointer
  },
  {
    "velocity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__Actuators, velocity),  // bytes offset in struct
    NULL,  // default value
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__velocity,  // size() function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__velocity,  // get_const(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__velocity,  // get(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__velocity,  // fetch(index, &value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__velocity,  // assign(index, value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__velocity  // resize(index) function pointer
  },
  {
    "normalized",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__Actuators, normalized),  // bytes offset in struct
    NULL,  // default value
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__size_function__Actuators__normalized,  // size() function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_const_function__Actuators__normalized,  // get_const(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__get_function__Actuators__normalized,  // get(index) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__fetch_function__Actuators__normalized,  // fetch(index, &value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__assign_function__Actuators__normalized,  // assign(index, value) function pointer
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__resize_function__Actuators__normalized  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_members = {
  "actuator_msgs__msg",  // message namespace
  "Actuators",  // message name
  4,  // number of fields
  sizeof(actuator_msgs__msg__Actuators),
  actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_member_array,  // message members
  actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_init_function,  // function to initialize message memory (memory has to be allocated)
  actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_type_support_handle = {
  0,
  &actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_actuator_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actuator_msgs, msg, Actuators)() {
  actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_type_support_handle.typesupport_identifier) {
    actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &actuator_msgs__msg__Actuators__rosidl_typesupport_introspection_c__Actuators_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
