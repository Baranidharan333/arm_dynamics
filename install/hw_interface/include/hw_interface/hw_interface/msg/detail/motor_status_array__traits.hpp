// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status_array.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__TRAITS_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "hw_interface/msg/detail/motor_status_array__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'motors'
#include "hw_interface/msg/detail/motor_status__traits.hpp"

namespace hw_interface
{

namespace msg
{

inline void to_flow_style_yaml(
  const MotorStatusArray & msg,
  std::ostream & out)
{
  out << "{";
  // member: motors
  {
    if (msg.motors.size() == 0) {
      out << "motors: []";
    } else {
      out << "motors: [";
      size_t pending_items = msg.motors.size();
      for (auto item : msg.motors) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MotorStatusArray & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: motors
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motors.size() == 0) {
      out << "motors: []\n";
    } else {
      out << "motors:\n";
      for (auto item : msg.motors) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MotorStatusArray & msg, bool use_flow_style = false)
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

}  // namespace hw_interface

namespace rosidl_generator_traits
{

[[deprecated("use hw_interface::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const hw_interface::msg::MotorStatusArray & msg,
  std::ostream & out, size_t indentation = 0)
{
  hw_interface::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hw_interface::msg::to_yaml() instead")]]
inline std::string to_yaml(const hw_interface::msg::MotorStatusArray & msg)
{
  return hw_interface::msg::to_yaml(msg);
}

template<>
inline const char * data_type<hw_interface::msg::MotorStatusArray>()
{
  return "hw_interface::msg::MotorStatusArray";
}

template<>
inline const char * name<hw_interface::msg::MotorStatusArray>()
{
  return "hw_interface/msg/MotorStatusArray";
}

template<>
struct has_fixed_size<hw_interface::msg::MotorStatusArray>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<hw_interface::msg::MotorStatusArray>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<hw_interface::msg::MotorStatusArray>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__TRAITS_HPP_
