// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from actuator_msgs:msg/ActuatorsPosition.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "actuator_msgs/msg/detail/actuators_position__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace actuator_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void ActuatorsPosition_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) actuator_msgs::msg::ActuatorsPosition(_init);
}

void ActuatorsPosition_fini_function(void * message_memory)
{
  auto typed_message = static_cast<actuator_msgs::msg::ActuatorsPosition *>(message_memory);
  typed_message->~ActuatorsPosition();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember ActuatorsPosition_message_member_array[3] = {
  {
    "header",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<std_msgs::msg::Header>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs::msg::ActuatorsPosition, header),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "angular",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<actuator_msgs::msg::ActuatorsAngularPosition>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs::msg::ActuatorsPosition, angular),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "linear",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<actuator_msgs::msg::ActuatorsLinearPosition>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actuator_msgs::msg::ActuatorsPosition, linear),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers ActuatorsPosition_message_members = {
  "actuator_msgs::msg",  // message namespace
  "ActuatorsPosition",  // message name
  3,  // number of fields
  sizeof(actuator_msgs::msg::ActuatorsPosition),
  ActuatorsPosition_message_member_array,  // message members
  ActuatorsPosition_init_function,  // function to initialize message memory (memory has to be allocated)
  ActuatorsPosition_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t ActuatorsPosition_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &ActuatorsPosition_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace actuator_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<actuator_msgs::msg::ActuatorsPosition>()
{
  return &::actuator_msgs::msg::rosidl_typesupport_introspection_cpp::ActuatorsPosition_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, actuator_msgs, msg, ActuatorsPosition)() {
  return &::actuator_msgs::msg::rosidl_typesupport_introspection_cpp::ActuatorsPosition_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
