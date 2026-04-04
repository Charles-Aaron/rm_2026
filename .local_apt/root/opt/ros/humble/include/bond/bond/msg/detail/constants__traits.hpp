// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from bond:msg/Constants.idl
// generated code does not contain a copyright notice

#ifndef BOND__MSG__DETAIL__CONSTANTS__TRAITS_HPP_
#define BOND__MSG__DETAIL__CONSTANTS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "bond/msg/detail/constants__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace bond
{

namespace msg
{

inline void to_flow_style_yaml(
  const Constants & msg,
  std::ostream & out)
{
  (void)msg;
  out << "null";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Constants & msg,
  std::ostream & out, size_t indentation = 0)
{
  (void)msg;
  (void)indentation;
  out << "null\n";
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Constants & msg, bool use_flow_style = false)
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

}  // namespace bond

namespace rosidl_generator_traits
{

[[deprecated("use bond::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const bond::msg::Constants & msg,
  std::ostream & out, size_t indentation = 0)
{
  bond::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use bond::msg::to_yaml() instead")]]
inline std::string to_yaml(const bond::msg::Constants & msg)
{
  return bond::msg::to_yaml(msg);
}

template<>
inline const char * data_type<bond::msg::Constants>()
{
  return "bond::msg::Constants";
}

template<>
inline const char * name<bond::msg::Constants>()
{
  return "bond/msg/Constants";
}

template<>
struct has_fixed_size<bond::msg::Constants>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<bond::msg::Constants>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<bond::msg::Constants>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // BOND__MSG__DETAIL__CONSTANTS__TRAITS_HPP_
