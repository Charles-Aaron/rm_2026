// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/ActuatorsVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators_velocity__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_ActuatorsVelocity_linear
{
public:
  explicit Init_ActuatorsVelocity_linear(::actuator_msgs::msg::ActuatorsVelocity & msg)
  : msg_(msg)
  {}
  ::actuator_msgs::msg::ActuatorsVelocity linear(::actuator_msgs::msg::ActuatorsVelocity::_linear_type arg)
  {
    msg_.linear = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsVelocity msg_;
};

class Init_ActuatorsVelocity_angular
{
public:
  explicit Init_ActuatorsVelocity_angular(::actuator_msgs::msg::ActuatorsVelocity & msg)
  : msg_(msg)
  {}
  Init_ActuatorsVelocity_linear angular(::actuator_msgs::msg::ActuatorsVelocity::_angular_type arg)
  {
    msg_.angular = std::move(arg);
    return Init_ActuatorsVelocity_linear(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsVelocity msg_;
};

class Init_ActuatorsVelocity_header
{
public:
  Init_ActuatorsVelocity_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ActuatorsVelocity_angular header(::actuator_msgs::msg::ActuatorsVelocity::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_ActuatorsVelocity_angular(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsVelocity msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::ActuatorsVelocity>()
{
  return actuator_msgs::msg::builder::Init_ActuatorsVelocity_header();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__BUILDER_HPP_
