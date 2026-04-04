// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from actuator_msgs:msg/ActuatorsNormalized.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__TRAITS_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "actuator_msgs/msg/detail/actuators_normalized__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"

namespace actuator_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ActuatorsNormalized & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: normalized
  {
    if (msg.normalized.size() == 0) {
      out << "normalized: []";
    } else {
      out << "normalized: [";
      size_t pending_items = msg.normalized.size();
      for (auto item : msg.normalized) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ActuatorsNormalized & msg,
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

  // member: normalized
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.normalized.size() == 0) {
      out << "normalized: []\n";
    } else {
      out << "normalized:\n";
      for (auto item : msg.normalized) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ActuatorsNormalized & msg, bool use_flow_style = false)
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

}  // namespace actuator_msgs

namespace rosidl_generator_traits
{

[[deprecated("use actuator_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const actuator_msgs::msg::ActuatorsNormalized & msg,
  std::ostream & out, size_t indentation = 0)
{
  actuator_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use actuator_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const actuator_msgs::msg::ActuatorsNormalized & msg)
{
  return actuator_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<actuator_msgs::msg::ActuatorsNormalized>()
{
  return "actuator_msgs::msg::ActuatorsNormalized";
}

template<>
inline const char * name<actuator_msgs::msg::ActuatorsNormalized>()
{
  return "actuator_msgs/msg/ActuatorsNormalized";
}

template<>
struct has_fixed_size<actuator_msgs::msg::ActuatorsNormalized>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<actuator_msgs::msg::ActuatorsNormalized>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<actuator_msgs::msg::ActuatorsNormalized>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_NORMALIZED__TRAITS_HPP_
