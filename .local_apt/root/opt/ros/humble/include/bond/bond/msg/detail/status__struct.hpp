// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from bond:msg/Status.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__STATUS__STRUCT_HPP_
#define BOND__MSG__DETAIL__STATUS__STRUCT_HPP_

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

#ifndef _WIN32
# define DEPRECATED__bond__msg__Status __attribute__((deprecated))
#else
# define DEPRECATED__bond__msg__Status __declspec(deprecated)
#endif

namespace bond
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Status_
{
  using Type = Status_<ContainerAllocator>;

  explicit Status_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->instance_id = "";
      this->active = false;
      this->heartbeat_timeout = 0.0f;
      this->heartbeat_period = 0.0f;
    }
  }

  explicit Status_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    id(_alloc),
    instance_id(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->instance_id = "";
      this->active = false;
      this->heartbeat_timeout = 0.0f;
      this->heartbeat_period = 0.0f;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _id_type id;
  using _instance_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _instance_id_type instance_id;
  using _active_type =
    bool;
  _active_type active;
  using _heartbeat_timeout_type =
    float;
  _heartbeat_timeout_type heartbeat_timeout;
  using _heartbeat_period_type =
    float;
  _heartbeat_period_type heartbeat_period;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__instance_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->instance_id = _arg;
    return *this;
  }
  Type & set__active(
    const bool & _arg)
  {
    this->active = _arg;
    return *this;
  }
  Type & set__heartbeat_timeout(
    const float & _arg)
  {
    this->heartbeat_timeout = _arg;
    return *this;
  }
  Type & set__heartbeat_period(
    const float & _arg)
  {
    this->heartbeat_period = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    bond::msg::Status_<ContainerAllocator> *;
  using ConstRawPtr =
    const bond::msg::Status_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<bond::msg::Status_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<bond::msg::Status_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      bond::msg::Status_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<bond::msg::Status_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      bond::msg::Status_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<bond::msg::Status_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<bond::msg::Status_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<bond::msg::Status_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__bond__msg__Status
    std::shared_ptr<bond::msg::Status_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__bond__msg__Status
    std::shared_ptr<bond::msg::Status_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Status_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->id != other.id) {
      return false;
    }
    if (this->instance_id != other.instance_id) {
      return false;
    }
    if (this->active != other.active) {
      return false;
    }
    if (this->heartbeat_timeout != other.heartbeat_timeout) {
      return false;
    }
    if (this->heartbeat_period != other.heartbeat_period) {
      return false;
    }
    return true;
  }
  bool operator!=(const Status_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Status_

// alias to use template instance with default allocator
using Status =
  bond::msg::Status_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace bond

#endif  // BOND__MSG__DETAIL__STATUS__STRUCT_HPP_
