// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from ros_gz_interfaces:msg/LogPlaybackStatistics.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__rosidl_typesupport_introspection_c.h"
#include "ros_gz_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__functions.h"
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `start_time`
// Member `end_time`
#include "builtin_interfaces/msg/time.h"
// Member `start_time`
// Member `end_time`
#include "builtin_interfaces/msg/detail/time__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ros_gz_interfaces__msg__LogPlaybackStatistics__init(message_memory);
}

void ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_fini_function(void * message_memory)
{
  ros_gz_interfaces__msg__LogPlaybackStatistics__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_gz_interfaces__msg__LogPlaybackStatistics, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "start_time",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_gz_interfaces__msg__LogPlaybackStatistics, start_time),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "end_time",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_gz_interfaces__msg__LogPlaybackStatistics, end_time),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_members = {
  "ros_gz_interfaces__msg",  // message namespace
  "LogPlaybackStatistics",  // message name
  3,  // number of fields
  sizeof(ros_gz_interfaces__msg__LogPlaybackStatistics),
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_member_array,  // message members
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_init_function,  // function to initialize message memory (memory has to be allocated)
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_type_support_handle = {
  0,
  &ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ros_gz_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_gz_interfaces, msg, LogPlaybackStatistics)() {
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Time)();
  ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Time)();
  if (!ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_type_support_handle.typesupport_identifier) {
    ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ros_gz_interfaces__msg__LogPlaybackStatistics__rosidl_typesupport_introspection_c__LogPlaybackStatistics_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
