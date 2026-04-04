// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from actuator_msgs:msg/ActuatorsLinearPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__TRAITS_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "actuator_msgs/msg/detail/actuators_linear_position__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace actuator_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ActuatorsLinearPosition & msg,
  std::ostream & out)
{
  out << "{";
  // member: position
  {
    if (msg.position.size() == 0) {
      out << "position: []";
    } else {
      out << "position: [";
      size_t pending_items = msg.position.size();
      for (auto item : msg.position) {
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
  const ActuatorsLinearPosition & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.position.size() == 0) {
      out << "position: []\n";
    } else {
      out << "position:\n";
      for (auto item : msg.position) {
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

inline std::string to_yaml(const ActuatorsLinearPosition & msg, bool use_flow_style = false)
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
  const actuator_msgs::msg::ActuatorsLinearPosition & msg,
  std::ostream & out, size_t indentation = 0)
{
  actuator_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use actuator_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const actuator_msgs::msg::ActuatorsLinearPosition & msg)
{
  return actuator_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<actuator_msgs::msg::ActuatorsLinearPosition>()
{
  return "actuator_msgs::msg::ActuatorsLinearPosition";
}

template<>
inline const char * name<actuator_msgs::msg::ActuatorsLinearPosition>()
{
  return "actuator_msgs/msg/ActuatorsLinearPosition";
}

template<>
struct has_fixed_size<actuator_msgs::msg::ActuatorsLinearPosition>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<actuator_msgs::msg::ActuatorsLinearPosition>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<actuator_msgs::msg::ActuatorsLinearPosition>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__TRAITS_HPP_
