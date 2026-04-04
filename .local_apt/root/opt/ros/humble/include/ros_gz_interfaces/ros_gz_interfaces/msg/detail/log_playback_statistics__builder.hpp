// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ros_gz_interfaces:msg/LogPlaybackStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__BUILDER_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ros_gz_interfaces/msg/detail/log_playback_statistics__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ros_gz_interfaces
{

namespace msg
{

namespace builder
{

class Init_LogPlaybackStatistics_end_time
{
public:
  explicit Init_LogPlaybackStatistics_end_time(::ros_gz_interfaces::msg::LogPlaybackStatistics & msg)
  : msg_(msg)
  {}
  ::ros_gz_interfaces::msg::LogPlaybackStatistics end_time(::ros_gz_interfaces::msg::LogPlaybackStatistics::_end_time_type arg)
  {
    msg_.end_time = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ros_gz_interfaces::msg::LogPlaybackStatistics msg_;
};

class Init_LogPlaybackStatistics_start_time
{
public:
  explicit Init_LogPlaybackStatistics_start_time(::ros_gz_interfaces::msg::LogPlaybackStatistics & msg)
  : msg_(msg)
  {}
  Init_LogPlaybackStatistics_end_time start_time(::ros_gz_interfaces::msg::LogPlaybackStatistics::_start_time_type arg)
  {
    msg_.start_time = std::move(arg);
    return Init_LogPlaybackStatistics_end_time(msg_);
  }

private:
  ::ros_gz_interfaces::msg::LogPlaybackStatistics msg_;
};

class Init_LogPlaybackStatistics_header
{
public:
  Init_LogPlaybackStatistics_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_LogPlaybackStatistics_start_time header(::ros_gz_interfaces::msg::LogPlaybackStatistics::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_LogPlaybackStatistics_start_time(msg_);
  }

private:
  ::ros_gz_interfaces::msg::LogPlaybackStatistics msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::ros_gz_interfaces::msg::LogPlaybackStatistics>()
{
  return ros_gz_interfaces::msg::builder::Init_LogPlaybackStatistics_header();
}

}  // namespace ros_gz_interfaces

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__BUILDER_HPP_
