// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/ActuatorsLinearVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators_linear_velocity__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_ActuatorsLinearVelocity_velocity
{
public:
  Init_ActuatorsLinearVelocity_velocity()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::actuator_msgs::msg::ActuatorsLinearVelocity velocity(::actuator_msgs::msg::ActuatorsLinearVelocity::_velocity_type arg)
  {
    msg_.velocity = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsLinearVelocity msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::ActuatorsLinearVelocity>()
{
  return actuator_msgs::msg::builder::Init_ActuatorsLinearVelocity_velocity();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__BUILDER_HPP_
