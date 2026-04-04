// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from actuator_msgs:msg/Actuators.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__BUILDER_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "actuator_msgs/msg/detail/actuators__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace actuator_msgs
{

namespace msg
{

namespace builder
{

class Init_Actuators_normalized
{
public:
  explicit Init_Actuators_normalized(::actuator_msgs::msg::Actuators & msg)
  : msg_(msg)
  {}
  ::actuator_msgs::msg::Actuators normalized(::actuator_msgs::msg::Actuators::_normalized_type arg)
  {
    msg_.normalized = std::move(arg);
    return std::move(msg_);
  }

private:
  ::actuator_msgs::msg::Actuators msg_;
};

class Init_Actuators_velocity
{
public:
  explicit Init_Actuators_velocity(::actuator_msgs::msg::Actuators & msg)
  : msg_(msg)
  {}
  Init_Actuators_normalized velocity(::actuator_msgs::msg::Actuators::_velocity_type arg)
  {
    msg_.velocity = std::move(arg);
    return Init_Actuators_normalized(msg_);
  }

private:
  ::actuator_msgs::msg::Actuators msg_;
};

class Init_Actuators_position
{
public:
  explicit Init_Actuators_position(::actuator_msgs::msg::Actuators & msg)
  : msg_(msg)
  {}
  Init_Actuators_velocity position(::actuator_msgs::msg::Actuators::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_Actuators_velocity(msg_);
  }

private:
  ::actuator_msgs::msg::Actuators msg_;
};

class Init_Actuators_header
{
public:
  Init_Actuators_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Actuators_position header(::actuator_msgs::msg::Actuators::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Actuators_position(msg_);
  }

private:
  ::actuator_msgs::msg::Actuators msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::actuator_msgs::msg::Actuators>()
{
  return actuator_msgs::msg::builder::Init_Actuators_header();
}

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS__BUILDER_HPP_
