// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from bond:msg/Status.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__STATUS__BUILDER_HPP_
#define BOND__MSG__DETAIL__STATUS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "bond/msg/detail/status__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace bond
{

namespace msg
{

namespace builder
{

class Init_Status_heartbeat_period
{
public:
  explicit Init_Status_heartbeat_period(::bond::msg::Status & msg)
  : msg_(msg)
  {}
  ::bond::msg::Status heartbeat_period(::bond::msg::Status::_heartbeat_period_type arg)
  {
    msg_.heartbeat_period = std::move(arg);
    return std::move(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

class Init_Status_heartbeat_timeout
{
public:
  explicit Init_Status_heartbeat_timeout(::bond::msg::Status & msg)
  : msg_(msg)
  {}
  Init_Status_heartbeat_period heartbeat_timeout(::bond::msg::Status::_heartbeat_timeout_type arg)
  {
    msg_.heartbeat_timeout = std::move(arg);
    return Init_Status_heartbeat_period(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

class Init_Status_active
{
public:
  explicit Init_Status_active(::bond::msg::Status & msg)
  : msg_(msg)
  {}
  Init_Status_heartbeat_timeout active(::bond::msg::Status::_active_type arg)
  {
    msg_.active = std::move(arg);
    return Init_Status_heartbeat_timeout(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

class Init_Status_instance_id
{
public:
  explicit Init_Status_instance_id(::bond::msg::Status & msg)
  : msg_(msg)
  {}
  Init_Status_active instance_id(::bond::msg::Status::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Status_active(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

class Init_Status_id
{
public:
  explicit Init_Status_id(::bond::msg::Status & msg)
  : msg_(msg)
  {}
  Init_Status_instance_id id(::bond::msg::Status::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Status_instance_id(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

class Init_Status_header
{
public:
  Init_Status_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Status_id header(::bond::msg::Status::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Status_id(msg_);
  }

private:
  ::bond::msg::Status msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::bond::msg::Status>()
{
  return bond::msg::builder::Init_Status_header();
}

}  // namespace bond

#endif  // BOND__MSG__DETAIL__STATUS__BUILDER_HPP_
