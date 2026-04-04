// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from bond:msg/Status.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__STATUS__TRAITS_HPP_
#define BOND__MSG__DETAIL__STATUS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "bond/msg/detail/status__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"

namespace bond
{

namespace msg
{

inline void to_flow_style_yaml(
  const Status & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: instance_id
  {
    out << "instance_id: ";
    rosidl_generator_traits::value_to_yaml(msg.instance_id, out);
    out << ", ";
  }

  // member: active
  {
    out << "active: ";
    rosidl_generator_traits::value_to_yaml(msg.active, out);
    out << ", ";
  }

  // member: heartbeat_timeout
  {
    out << "heartbeat_timeout: ";
    rosidl_generator_traits::value_to_yaml(msg.heartbeat_timeout, out);
    out << ", ";
  }

  // member: heartbeat_period
  {
    out << "heartbeat_period: ";
    rosidl_generator_traits::value_to_yaml(msg.heartbeat_period, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Status & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: instance_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "instance_id: ";
    rosidl_generator_traits::value_to_yaml(msg.instance_id, out);
    out << "\n";
  }

  // member: active
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "active: ";
    rosidl_generator_traits::value_to_yaml(msg.active, out);
    out << "\n";
  }

  // member: heartbeat_timeout
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "heartbeat_timeout: ";
    rosidl_generator_traits::value_to_yaml(msg.heartbeat_timeout, out);
    out << "\n";
  }

  // member: heartbeat_period
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "heartbeat_period: ";
    rosidl_generator_traits::value_to_yaml(msg.heartbeat_period, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Status & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace bond

namespace rosidl_generator_traits
{

[[deprecated("use bond::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const bond::msg::Status & msg,
  std::ostream & out, size_t indentation = 0)
{
  bond::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use bond::msg::to_yaml() instead")]]
inline std::string to_yaml(const bond::msg::Status & msg)
{
  return bond::msg::to_yaml(msg);
}

template<>
inline const char * data_type<bond::msg::Status>()
{
  return "bond::msg::Status";
}

template<>
inline const char * name<bond::msg::Status>()
{
  return "bond/msg/Status";
}

template<>
struct has_fixed_size<bond::msg::Status>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<bond::msg::Status>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<bond::msg::Status>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // BOND__MSG__DETAIL__STATUS__TRAITS_HPP_
