// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__TRAITS_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "ros_gz_interfaces/msg/detail/world_statistics__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'sim_time'
// Member 'pause_time'
// Member 'real_time'
// Member 'step_size'
#include "builtin_interfaces/msg/detail/time__traits.hpp"
// Member 'log_playback_statistics'
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__traits.hpp"

namespace ros_gz_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const WorldStatistics & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: sim_time
  {
    out << "sim_time: ";
    to_flow_style_yaml(msg.sim_time, out);
    out << ", ";
  }

  // member: pause_time
  {
    out << "pause_time: ";
    to_flow_style_yaml(msg.pause_time, out);
    out << ", ";
  }

  // member: real_time
  {
    out << "real_time: ";
    to_flow_style_yaml(msg.real_time, out);
    out << ", ";
  }

  // member: paused
  {
    out << "paused: ";
    rosidl_generator_traits::value_to_yaml(msg.paused, out);
    out << ", ";
  }

  // member: iterations
  {
    out << "iterations: ";
    rosidl_generator_traits::value_to_yaml(msg.iterations, out);
    out << ", ";
  }

  // member: model_count
  {
    out << "model_count: ";
    rosidl_generator_traits::value_to_yaml(msg.model_count, out);
    out << ", ";
  }

  // member: log_playback_statistics
  {
    out << "log_playback_statistics: ";
    to_flow_style_yaml(msg.log_playback_statistics, out);
    out << ", ";
  }

  // member: real_time_factor
  {
    out << "real_time_factor: ";
    rosidl_generator_traits::value_to_yaml(msg.real_time_factor, out);
    out << ", ";
  }

  // member: step_size
  {
    out << "step_size: ";
    to_flow_style_yaml(msg.step_size, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const WorldStatistics & msg,
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

  // member: sim_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "sim_time:\n";
    to_block_style_yaml(msg.sim_time, out, indentation + 2);
  }

  // member: pause_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pause_time:\n";
    to_block_style_yaml(msg.pause_time, out, indentation + 2);
  }

  // member: real_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "real_time:\n";
    to_block_style_yaml(msg.real_time, out, indentation + 2);
  }

  // member: paused
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "paused: ";
    rosidl_generator_traits::value_to_yaml(msg.paused, out);
    out << "\n";
  }

  // member: iterations
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "iterations: ";
    rosidl_generator_traits::value_to_yaml(msg.iterations, out);
    out << "\n";
  }

  // member: model_count
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "model_count: ";
    rosidl_generator_traits::value_to_yaml(msg.model_count, out);
    out << "\n";
  }

  // member: log_playback_statistics
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "log_playback_statistics:\n";
    to_block_style_yaml(msg.log_playback_statistics, out, indentation + 2);
  }

  // member: real_time_factor
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "real_time_factor: ";
    rosidl_generator_traits::value_to_yaml(msg.real_time_factor, out);
    out << "\n";
  }

  // member: step_size
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "step_size:\n";
    to_block_style_yaml(msg.step_size, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const WorldStatistics & msg, bool use_flow_style = false)
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
  const ros_gz_interfaces::msg::WorldStatistics & msg,
  std::ostream & out, size_t indentation = 0)
{
  ros_gz_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ros_gz_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const ros_gz_interfaces::msg::WorldStatistics & msg)
{
  return ros_gz_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<ros_gz_interfaces::msg::WorldStatistics>()
{
  return "ros_gz_interfaces::msg::WorldStatistics";
}

template<>
inline const char * name<ros_gz_interfaces::msg::WorldStatistics>()
{
  return "ros_gz_interfaces/msg/WorldStatistics";
}

template<>
struct has_fixed_size<ros_gz_interfaces::msg::WorldStatistics>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value && has_fixed_size<ros_gz_interfaces::msg::LogPlaybackStatistics>::value && has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<ros_gz_interfaces::msg::WorldStatistics>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value && has_bounded_size<ros_gz_interfaces::msg::LogPlaybackStatistics>::value && has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<ros_gz_interfaces::msg::WorldStatistics>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__TRAITS_HPP_
