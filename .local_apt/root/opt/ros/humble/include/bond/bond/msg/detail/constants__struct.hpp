// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from bond:msg/Constants.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__CONSTANTS__STRUCT_HPP_
#define BOND__MSG__DETAIL__CONSTANTS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__bond__msg__Constants __attribute__((deprecated))
#else
# define DEPRECATED__bond__msg__Constants __declspec(deprecated)
#endif

namespace bond
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Constants_
{
  using Type = Constants_<ContainerAllocator>;

  explicit Constants_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit Constants_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  // field types and members
  using _structure_needs_at_least_one_member_type =
    uint8_t;
  _structure_needs_at_least_one_member_type structure_needs_at_least_one_member;


  // constant declarations
  static constexpr float DEAD_PUBLISH_PERIOD =
    0.05;
  static constexpr float DEFAULT_CONNECT_TIMEOUT =
    10.0;
  static constexpr float DEFAULT_HEARTBEAT_TIMEOUT =
    4.0;
  static constexpr float DEFAULT_DISCONNECT_TIMEOUT =
    2.0;
  static constexpr float DEFAULT_HEARTBEAT_PERIOD =
    1.0;
  static const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> DISABLE_HEARTBEAT_TIMEOUT_PARAM;

  // pointer types
  using RawPtr =
    bond::msg::Constants_<ContainerAllocator> *;
  using ConstRawPtr =
    const bond::msg::Constants_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<bond::msg::Constants_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<bond::msg::Constants_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      bond::msg::Constants_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<bond::msg::Constants_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      bond::msg::Constants_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<bond::msg::Constants_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<bond::msg::Constants_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<bond::msg::Constants_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__bond__msg__Constants
    std::shared_ptr<bond::msg::Constants_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__bond__msg__Constants
    std::shared_ptr<bond::msg::Constants_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Constants_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const Constants_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Constants_

// alias to use template instance with default allocator
using Constants =
  bond::msg::Constants_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr float Constants_<ContainerAllocator>::DEAD_PUBLISH_PERIOD;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr float Constants_<ContainerAllocator>::DEFAULT_CONNECT_TIMEOUT;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr float Constants_<ContainerAllocator>::DEFAULT_HEARTBEAT_TIMEOUT;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr float Constants_<ContainerAllocator>::DEFAULT_DISCONNECT_TIMEOUT;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr float Constants_<ContainerAllocator>::DEFAULT_HEARTBEAT_PERIOD;
#endif  // __cplusplus < 201703L
template<typename ContainerAllocator>
const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>
Constants_<ContainerAllocator>::DISABLE_HEARTBEAT_TIMEOUT_PARAM = "/bond_disable_heartbeat_timeout";

}  // namespace msg

}  // namespace bond

#endif  // BOND__MSG__DETAIL__CONSTANTS__STRUCT_HPP_
