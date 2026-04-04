// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ros_gz_interfaces:msg/WorldStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"
// Member 'sim_time'
// Member 'pause_time'
// Member 'real_time'
// Member 'step_size'
#include "builtin_interfaces/msg/detail/time__struct.hpp"
// Member 'log_playback_statistics'
#include "ros_gz_interfaces/msg/detail/log_playback_statistics__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ros_gz_interfaces__msg__WorldStatistics __attribute__((deprecated))
#else
# define DEPRECATED__ros_gz_interfaces__msg__WorldStatistics __declspec(deprecated)
#endif

namespace ros_gz_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct WorldStatistics_
{
  using Type = WorldStatistics_<ContainerAllocator>;

  explicit WorldStatistics_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    sim_time(_init),
    pause_time(_init),
    real_time(_init),
    log_playback_statistics(_init),
    step_size(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->paused = false;
      this->iterations = 0ull;
      this->model_count = 0l;
      this->real_time_factor = 0.0;
    }
  }

  explicit WorldStatistics_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    sim_time(_alloc, _init),
    pause_time(_alloc, _init),
    real_time(_alloc, _init),
    log_playback_statistics(_alloc, _init),
    step_size(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->paused = false;
      this->iterations = 0ull;
      this->model_count = 0l;
      this->real_time_factor = 0.0;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _sim_time_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _sim_time_type sim_time;
  using _pause_time_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _pause_time_type pause_time;
  using _real_time_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _real_time_type real_time;
  using _paused_type =
    bool;
  _paused_type paused;
  using _iterations_type =
    uint64_t;
  _iterations_type iterations;
  using _model_count_type =
    int32_t;
  _model_count_type model_count;
  using _log_playback_statistics_type =
    ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>;
  _log_playback_statistics_type log_playback_statistics;
  using _real_time_factor_type =
    double;
  _real_time_factor_type real_time_factor;
  using _step_size_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _step_size_type step_size;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__sim_time(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->sim_time = _arg;
    return *this;
  }
  Type & set__pause_time(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->pause_time = _arg;
    return *this;
  }
  Type & set__real_time(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->real_time = _arg;
    return *this;
  }
  Type & set__paused(
    const bool & _arg)
  {
    this->paused = _arg;
    return *this;
  }
  Type & set__iterations(
    const uint64_t & _arg)
  {
    this->iterations = _arg;
    return *this;
  }
  Type & set__model_count(
    const int32_t & _arg)
  {
    this->model_count = _arg;
    return *this;
  }
  Type & set__log_playback_statistics(
    const ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> & _arg)
  {
    this->log_playback_statistics = _arg;
    return *this;
  }
  Type & set__real_time_factor(
    const double & _arg)
  {
    this->real_time_factor = _arg;
    return *this;
  }
  Type & set__step_size(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->step_size = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> *;
  using ConstRawPtr =
    const ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ros_gz_interfaces__msg__WorldStatistics
    std::shared_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ros_gz_interfaces__msg__WorldStatistics
    std::shared_ptr<ros_gz_interfaces::msg::WorldStatistics_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const WorldStatistics_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->sim_time != other.sim_time) {
      return false;
    }
    if (this->pause_time != other.pause_time) {
      return false;
    }
    if (this->real_time != other.real_time) {
      return false;
    }
    if (this->paused != other.paused) {
      return false;
    }
    if (this->iterations != other.iterations) {
      return false;
    }
    if (this->model_count != other.model_count) {
      return false;
    }
    if (this->log_playback_statistics != other.log_playback_statistics) {
      return false;
    }
    if (this->real_time_factor != other.real_time_factor) {
      return false;
    }
    if (this->step_size != other.step_size) {
      return false;
    }
    return true;
  }
  bool operator!=(const WorldStatistics_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct WorldStatistics_

// alias to use template instance with default allocator
using WorldStatistics =
  ros_gz_interfaces::msg::WorldStatistics_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace ros_gz_interfaces

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__WORLD_STATISTICS__STRUCT_HPP_
