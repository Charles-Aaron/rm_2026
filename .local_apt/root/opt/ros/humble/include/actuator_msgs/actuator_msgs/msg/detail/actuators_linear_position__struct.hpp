// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from actuator_msgs:msg/ActuatorsLinearPosition.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__actuator_msgs__msg__ActuatorsLinearPosition __attribute__((deprecated))
#else
# define DEPRECATED__actuator_msgs__msg__ActuatorsLinearPosition __declspec(deprecated)
#endif

namespace actuator_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ActuatorsLinearPosition_
{
  using Type = ActuatorsLinearPosition_<ContainerAllocator>;

  explicit ActuatorsLinearPosition_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit ActuatorsLinearPosition_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _position_type =
    std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>>;
  _position_type position;

  // setters for named parameter idiom
  Type & set__position(
    const std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>> & _arg)
  {
    this->position = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> *;
  using ConstRawPtr =
    const actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsLinearPosition
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsLinearPosition
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearPosition_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ActuatorsLinearPosition_ & other) const
  {
    if (this->position != other.position) {
      return false;
    }
    return true;
  }
  bool operator!=(const ActuatorsLinearPosition_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ActuatorsLinearPosition_

// alias to use template instance with default allocator
using ActuatorsLinearPosition =
  actuator_msgs::msg::ActuatorsLinearPosition_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_POSITION__STRUCT_HPP_
