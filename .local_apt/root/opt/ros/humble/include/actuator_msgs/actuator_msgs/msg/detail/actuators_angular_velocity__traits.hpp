// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from actuator_msgs:msg/ActuatorsAngularVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_VELOCITY__TRAITS_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_VELOCITY__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "actuator_msgs/msg/detail/actuators_angular_velocity__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace actuator_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ActuatorsAngularVelocity & msg,
  std::ostream & out)
{
  out << "{";
  // member: velocity
  {
    if (msg.velocity.size() == 0) {
      out << "velocity: []";
    } else {
      out << "velocity: [";
      size_t pending_items = msg.velocity.size();
      for (auto item : msg.velocity) {
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
  const ActuatorsAngularVelocity & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.velocity.size() == 0) {
      out << "velocity: []\n";
    } else {
      out << "velocity:\n";
      for (auto item : msg.velocity) {
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

inline std::string to_yaml(const ActuatorsAngularVelocity & msg, bool use_flow_style = false)
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
  const actuator_msgs::msg::ActuatorsAngularVelocity & msg,
  std::ostream & out, size_t indentation = 0)
{
  actuator_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use actuator_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const actuator_msgs::msg::ActuatorsAngularVelocity & msg)
{
  return actuator_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<actuator_msgs::msg::ActuatorsAngularVelocity>()
{
  return "actuator_msgs::msg::ActuatorsAngularVelocity";
}

template<>
inline const char * name<actuator_msgs::msg::ActuatorsAngularVelocity>()
{
  return "actuator_msgs/msg/ActuatorsAngularVelocity";
}

template<>
struct has_fixed_size<actuator_msgs::msg::ActuatorsAngularVelocity>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<actuator_msgs::msg::ActuatorsAngularVelocity>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<actuator_msgs::msg::ActuatorsAngularVelocity>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_ANGULAR_VELOCITY__TRAITS_HPP_
