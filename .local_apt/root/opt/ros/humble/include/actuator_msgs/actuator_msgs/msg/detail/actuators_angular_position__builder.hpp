// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/ActuatorsAngularPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators_angular_position__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_ActuatorsAngularPosition_position
{
public:
  Init_ActuatorsAngularPosition_position()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::actuator_msgs::msg::ActuatorsAngularPosition position(::actuator_msgs::msg::ActuatorsAngularPosition::_position_type arg)
  {
    msg_.position = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsAngularPosition msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::ActuatorsAngularPosition>()
{
  return actuator_msgs::msg::builder::Init_ActuatorsAngularPosition_position();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_POSITION__BUILDER_HPP_
