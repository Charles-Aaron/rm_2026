// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from actuator_msgs:msg/ActuatorsVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_HPP_

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
// Member 'angular'
#include "actuator_msgs/msg/detail/actuators_angular_velocity__struct.hpp"
// Member 'linear'
#include "actuator_msgs/msg/detail/actuators_linear_velocity__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__actuator_msgs__msg__ActuatorsVelocity __attribute__((deprecated))
#else
# define DEPRECATED__actuator_msgs__msg__ActuatorsVelocity __declspec(deprecated)
#endif

namespace actuator_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ActuatorsVelocity_
{
  using Type = ActuatorsVelocity_<ContainerAllocator>;

  explicit ActuatorsVelocity_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    angular(_init),
    linear(_init)
  {
    (void)_init;
  }

  explicit ActuatorsVelocity_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    angular(_alloc, _init),
    linear(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _angular_type =
    actuator_msgs::msg::ActuatorsAngularVelocity_<ContainerAllocator>;
  _angular_type angular;
  using _linear_type =
    actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>;
  _linear_type linear;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__angular(
    const actuator_msgs::msg::ActuatorsAngularVelocity_<ContainerAllocator> & _arg)
  {
    this->angular = _arg;
    return *this;
  }
  Type & set__linear(
    const actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> & _arg)
  {
    this->linear = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> *;
  using ConstRawPtr =
    const actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsVelocity
    std::shared_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsVelocity
    std::shared_ptr<actuator_msgs::msg::ActuatorsVelocity_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ActuatorsVelocity_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->angular != other.angular) {
      return false;
    }
    if (this->linear != other.linear) {
      return false;
    }
    return true;
  }
  bool operator!=(const ActuatorsVelocity_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ActuatorsVelocity_

// alias to use template instance with default allocator
using ActuatorsVelocity =
  actuator_msgs::msg::ActuatorsVelocity_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_VELOCITY__STRUCT_HPP_
