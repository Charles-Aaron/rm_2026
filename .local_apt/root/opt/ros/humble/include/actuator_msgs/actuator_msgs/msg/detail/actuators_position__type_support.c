// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from actuator_msgs:msg/ActuatorsPosition.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "actuator_msgs/msg/detail/actuators_position__rosidl_typesupport_introspection_c.h"
#include "actuator_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "actuator_msgs/msg/detail/actuators_position__functions.h"
#include "actuator_msgs/msg/detail/actuators_position__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `angular`
#include "actuator_msgs/msg/actuators_angular_position.h"
// Member `angular`
#include "actuator_msgs/msg/detail/actuators_angular_position__rosidl_typesupport_introspection_c.h"
// Member `linear`
#include "actuator_msgs/msg/actuators_linear_position.h"
// Member `linear`
#include "actuator_msgs/msg/detail/actuators_linear_position__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  actuator_msgs__msg__ActuatorsPosition__init(message_memory);
}

void actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_fini_function(void * message_memory)
{
  actuator_msgs__msg__ActuatorsPosition__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__ActuatorsPosition, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "angular",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__ActuatorsPosition, angular),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "linear",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs__msg__ActuatorsPosition, linear),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_members = {
  "actuator_msgs__msg",  // message namespace
  "ActuatorsPosition",  // message name
  3,  // number of fields
  sizeof(actuator_msgs__msg__ActuatorsPosition),
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_member_array,  // message members
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_init_function,  // function to initialize message memory (memory has to be allocated)
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_type_support_handle = {
  0,
  &actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_actuator_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actuator_msgs, msg, ActuatorsPosition)() {
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actuator_msgs, msg, ActuatorsAngularPosition)();
  actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actuator_msgs, msg, ActuatorsLinearPosition)();
  if (!actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_type_support_handle.typesupport_identifier) {
    actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &actuator_msgs__msg__ActuatorsPosition__rosidl_typesupport_introspection_c__ActuatorsPosition_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
