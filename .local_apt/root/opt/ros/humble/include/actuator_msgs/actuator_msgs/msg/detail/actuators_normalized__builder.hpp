// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/ActuatorsNormalized.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators_normalized__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_ActuatorsNormalized_normalized
{
public:
  explicit Init_ActuatorsNormalized_normalized(::actuator_msgs::msg::ActuatorsNormalized & msg)
  : msg_(msg)
  {}
  ::actuator_msgs::msg::ActuatorsNormalized normalized(::actuator_msgs::msg::ActuatorsNormalized::_normalized_type arg)
  {
    msg_.normalized = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsNormalized msg_;
};

class Init_ActuatorsNormalized_header
{
public:
  Init_ActuatorsNormalized_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ActuatorsNormalized_normalized header(::actuator_msgs::msg::ActuatorsNormalized::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_ActuatorsNormalized_normalized(msg_);
  }

private:
  ::actuator_msgs::msg::ActuatorsNormalized msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::ActuatorsNormalized>()
{
  return actuator_msgs::msg::builder::Init_ActuatorsNormalized_header();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__BUILDER_HPP_
