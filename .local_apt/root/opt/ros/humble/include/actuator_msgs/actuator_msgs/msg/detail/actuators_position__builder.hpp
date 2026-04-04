// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/ActuatorsPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators_position__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_ActuatorsPosition_linear
{
public:
  explicit Init_ActuatorsPosition_linear(::actuator_msgs::msg::ActuatorsPosition & msg)
  : msg_(msg)
  {}
  ::actuator_msgs::msg::ActuatorsPosition linear(::actuator_msgs::msg::ActuatorsPosition::_linear_type arg)
  {
    msg_.linear = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsPosition msg_;
};

class Init_ActuatorsPosition_angular
{
public:
  explicit Init_ActuatorsPosition_angular(::actuator_msgs::msg::ActuatorsPosition & msg)
  : msg_(msg)
  {}
  Init_ActuatorsPosition_linear angular(::actuator_msgs::msg::ActuatorsPosition::_angular_type arg)
  {
    msg_.angular = std::move(arg);
    return Init_ActuatorsPosition_linear(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsPosition msg_;
};

class Init_ActuatorsPosition_header
{
public:
  Init_ActuatorsPosition_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ActuatorsPosition_angular header(::actuator_msgs::msg::ActuatorsPosition::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_ActuatorsPosition_angular(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsPosition msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::ActuatorsPosition>()
{
  return actuator_msgs::msg::builder::Init_ActuatorsPosition_header();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_POSITION__BUILDER_HPP_
