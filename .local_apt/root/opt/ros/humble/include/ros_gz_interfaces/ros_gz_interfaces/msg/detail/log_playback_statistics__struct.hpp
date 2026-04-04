// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ros_gz_interfaces:msg/LogPlaybackStatistics.idl
// generated code does not contain a copyright notice

#ifndef ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_HPP_
#define ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_HPP_

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
// Member 'start_time'
// Member 'end_time'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__ros_gz_interfaces__msg__LogPlaybackStatistics __attribute__((deprecated))
#else
# define DEPRECATED__ros_gz_interfaces__msg__LogPlaybackStatistics __declspec(deprecated)
#endif

namespace ros_gz_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct LogPlaybackStatistics_
{
  using Type = LogPlaybackStatistics_<ContainerAllocator>;

  explicit LogPlaybackStatistics_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    start_time(_init),
    end_time(_init)
  {
    (void)_init;
  }

  explicit LogPlaybackStatistics_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    start_time(_alloc, _init),
    end_time(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _start_time_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _start_time_type start_time;
  using _end_time_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _end_time_type end_time;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__start_time(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->start_time = _arg;
    return *this;
  }
  Type & set__end_time(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->end_time = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> *;
  using ConstRawPtr =
    const ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ros_gz_interfaces__msg__LogPlaybackStatistics
    std::shared_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ros_gz_interfaces__msg__LogPlaybackStatistics
    std::shared_ptr<ros_gz_interfaces::msg::LogPlaybackStatistics_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const LogPlaybackStatistics_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->start_time != other.start_time) {
      return false;
    }
    if (this->end_time != other.end_time) {
      return false;
    }
    return true;
  }
  bool operator!=(const LogPlaybackStatistics_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct LogPlaybackStatistics_

// alias to use template instance with default allocator
using LogPlaybackStatistics =
  ros_gz_interfaces::msg::LogPlaybackStatistics_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace ros_gz_interfaces

#endif  // ROS_GZ_INTERFACES__MSG__DETAIL__LOG_PLAYBACK_STATISTICS__STRUCT_HPP_
