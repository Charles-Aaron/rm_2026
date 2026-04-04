// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__BUILDER_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ros_gz_interfaces/msg/detail/world_statistics__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ros_gz_interfaces
{

namespace msg
{

namespace builder
{

class Init_WorldStatistics_step_size
{
public:
  explicit Init_WorldStatistics_step_size(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  ::ros_gz_interfaces::msg::WorldStatistics step_size(::ros_gz_interfaces::msg::WorldStatistics::_step_size_type arg)
  {
    msg_.step_size = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_real_time_factor
{
public:
  explicit Init_WorldStatistics_real_time_factor(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_step_size real_time_factor(::ros_gz_interfaces::msg::WorldStatistics::_real_time_factor_type arg)
  {
    msg_.real_time_factor = std::move(arg);
    return Init_WorldStatistics_step_size(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_log_playback_statistics
{
public:
  explicit Init_WorldStatistics_log_playback_statistics(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_real_time_factor log_playback_statistics(::ros_gz_interfaces::msg::WorldStatistics::_log_playback_statistics_type arg)
  {
    msg_.log_playback_statistics = std::move(arg);
    return Init_WorldStatistics_real_time_factor(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_model_count
{
public:
  explicit Init_WorldStatistics_model_count(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_log_playback_statistics model_count(::ros_gz_interfaces::msg::WorldStatistics::_model_count_type arg)
  {
    msg_.model_count = std::move(arg);
    return Init_WorldStatistics_log_playback_statistics(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_iterations
{
public:
  explicit Init_WorldStatistics_iterations(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_model_count iterations(::ros_gz_interfaces::msg::WorldStatistics::_iterations_type arg)
  {
    msg_.iterations = std::move(arg);
    return Init_WorldStatistics_model_count(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_paused
{
public:
  explicit Init_WorldStatistics_paused(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_iterations paused(::ros_gz_interfaces::msg::WorldStatistics::_paused_type arg)
  {
    msg_.paused = std::move(arg);
    return Init_WorldStatistics_iterations(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_real_time
{
public:
  explicit Init_WorldStatistics_real_time(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_paused real_time(::ros_gz_interfaces::msg::WorldStatistics::_real_time_type arg)
  {
    msg_.real_time = std::move(arg);
    return Init_WorldStatistics_paused(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_pause_time
{
public:
  explicit Init_WorldStatistics_pause_time(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_real_time pause_time(::ros_gz_interfaces::msg::WorldStatistics::_pause_time_type arg)
  {
    msg_.pause_time = std::move(arg);
    return Init_WorldStatistics_real_time(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_sim_time
{
public:
  explicit Init_WorldStatistics_sim_time(::ros_gz_interfaces::msg::WorldStatistics & msg)
  : msg_(msg)
  {}
  Init_WorldStatistics_pause_time sim_time(::ros_gz_interfaces::msg::WorldStatistics::_sim_time_type arg)
  {
    msg_.sim_time = std::move(arg);
    return Init_WorldStatistics_pause_time(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

class Init_WorldStatistics_header
{
public:
  Init_WorldStatistics_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_WorldStatistics_sim_time header(::ros_gz_interfaces::msg::WorldStatistics::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_WorldStatistics_sim_time(msg_);
  }

private:
  ::ros_gz_interfaces::msg::WorldStatistics msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::ros_gz_interfaces::msg::WorldStatistics>()
{
  return ros_gz_interfaces::msg::builder::Init_WorldStatistics_header();
}

}  // namespace ros_gz_interfaces

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__BUILDER_HPP_
