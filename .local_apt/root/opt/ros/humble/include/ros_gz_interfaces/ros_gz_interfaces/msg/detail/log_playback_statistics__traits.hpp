// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from ros_gz_interfaces:msg/LogPlaybackStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__TRAITS_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "ros_gz_interfaces/msg/detail/log_playback_statistics__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'start_time'
// Member 'end_time'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace ros_gz_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const LogPlaybackStatistics & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: start_time
  {
    out << "start_time: ";
    to_flow_style_yaml(msg.start_time, out);
    out << ", ";
  }

  // member: end_time
  {
    out << "end_time: ";
    to_flow_style_yaml(msg.end_time, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const LogPlaybackStatistics & msg,
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

  // member: start_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "start_time:\n";
    to_block_style_yaml(msg.start_time, out, indentation + 2);
  }

  // member: end_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "end_time:\n";
    to_block_style_yaml(msg.end_time, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const LogPlaybackStatistics & msg, bool use_flow_style = false)
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

}  // namespace ros_gz_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use ros_gz_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ros_gz_interfaces::msg::LogPlaybackStatistics & msg,
  std::ostream & out, size_t indentation = 0)
{
  ros_gz_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ros_gz_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const ros_gz_interfaces::msg::LogPlaybackStatistics & msg)
{
  return ros_gz_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<ros_gz_interfaces::msg::LogPlaybackStatistics>()
{
  return "ros_gz_interfaces::msg::LogPlaybackStatistics";
}

template<>
inline const char * name<ros_gz_interfaces::msg::LogPlaybackStatistics>()
{
  return "ros_gz_interfaces/msg/LogPlaybackStatistics";
}

template<>
struct has_fixed_size<ros_gz_interfaces::msg::LogPlaybackStatistics>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value && has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<ros_gz_interfaces::msg::LogPlaybackStatistics>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value && has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<ros_gz_interfaces::msg::LogPlaybackStatistics>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__TRAITS_HPP_
