// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from actuator_msgs:msg/ActuatorsLinearVelocity.idl
// generated code does not contain a copyright notice

#ifndef ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__STRUCT_HPP_
#define ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__actuator_msgs__msg__ActuatorsLinearVelocity __attribute__((deprecated))
#else
# define DEPRECATED__actuator_msgs__msg__ActuatorsLinearVelocity __declspec(deprecated)
#endif

namespace actuator_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ActuatorsLinearVelocity_
{
  using Type = ActuatorsLinearVelocity_<ContainerAllocator>;

  explicit ActuatorsLinearVelocity_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit ActuatorsLinearVelocity_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _velocity_type =
    std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>>;
  _velocity_type velocity;

  // setters for named parameter idiom
  Type & set__velocity(
    const std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>> & _arg)
  {
    this->velocity = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> *;
  using ConstRawPtr =
    const actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsLinearVelocity
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__actuator_msgs__msg__ActuatorsLinearVelocity
    std::shared_ptr<actuator_msgs::msg::ActuatorsLinearVelocity_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ActuatorsLinearVelocity_ & other) const
  {
    if (this->velocity != other.velocity) {
      return false;
    }
    return true;
  }
  bool operator!=(const ActuatorsLinearVelocity_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ActuatorsLinearVelocity_

// alias to use template instance with default allocator
using ActuatorsLinearVelocity =
  actuator_msgs::msg::ActuatorsLinearVelocity_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace actuator_msgs

#endif  // ACTUATOR_MSGS__MSG__DETAIL__ACTUATORS_LINEAR_VELOCITY__STRUCT_HPP_
