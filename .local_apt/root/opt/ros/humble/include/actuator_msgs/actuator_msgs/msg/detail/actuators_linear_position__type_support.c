// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from actuator_msgs:msg/ActuatorsLinearPosition.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "actuator_msgs/msg/detail/actuators_linear_position__rosidl_typesupport_introspection_c.h"
#include "actuator_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "actuator_msgs/msg/detail/actuators_linear_position__functions.h"
#include "actuator_msgs/msg/detail/actuators_linear_position__struct.h"


// Include directives for member types
// Member `position`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  actuator_msgs__msg__ActuatorsLinearPosition__init(message_memory);
}

void actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_fini_function(void * message_memory)
{
  actuator_msgs__msg__ActuatorsLinearPosition__fini(message_memory);
}

size_t actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__size_function__ActuatorsLinearPosition__position(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_const_function__ActuatorsLinearPosition__position(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_function__ActuatorsLinearPosition__position(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__fetch_function__ActuatorsLinearPosition__position(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_const_function__ActuatorsLinearPosition__position(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__assign_function__ActuatorsLinearPosition__position(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_function__ActuatorsLinearPosition__position(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__resize_function__ActuatorsLinearPosition__position(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_member_array[1] = {
  {
    "position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__ActuatorsLinearPosition, position),  // bytes offset in struct
    NULL,  // default value
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__size_function__ActuatorsLinearPosition__position,  // size() function pointer
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_const_function__ActuatorsLinearPosition__position,  // get_const(index) function pointer
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__get_function__ActuatorsLinearPosition__position,  // get(index) function pointer
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__fetch_function__ActuatorsLinearPosition__position,  // fetch(index, &value) function pointer
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__assign_function__ActuatorsLinearPosition__position,  // assign(index, value) function pointer
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__resize_function__ActuatorsLinearPosition__position  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_members = {
  "actuator_msgs__msg",  // message namespace
  "ActuatorsLinearPosition",  // message name
  1,  // number of fields
  sizeof(actuator_msgs__msg__ActuatorsLinearPosition),
  actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_member_array,  // message members
  actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_init_function,  // function to initialize message memory (memory has to be allocated)
  actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_type_support_handle = {
  0,
  &actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_actuator_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actuator_msgs, msg, ActuatorsLinearPosition)() {
  if (!actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_type_support_handle.typesupport_identifier) {
    actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &actuator_msgs__msg__ActuatorsLinearPosition__rosidl_typesupport_introspection_c__ActuatorsLinearPosition_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
