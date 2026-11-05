// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__TRAITS_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "hw_interface/msg/detail/motor_status__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace hw_interface
{

namespace msg
{

inline void to_flow_style_yaml(
  const MotorStatus & msg,
  std::ostream & out)
{
  out << "{";
  // member: arm_name
  {
    out << "arm_name: ";
    rosidl_generator_traits::value_to_yaml(msg.arm_name, out);
    out << ", ";
  }

  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: error
  {
    out << "error: ";
    rosidl_generator_traits::value_to_yaml(msg.error, out);
    out << ", ";
  }

  // member: error_name
  {
    out << "error_name: ";
    rosidl_generator_traits::value_to_yaml(msg.error_name, out);
    out << ", ";
  }

  // member: mos_temp
  {
    out << "mos_temp: ";
    rosidl_generator_traits::value_to_yaml(msg.mos_temp, out);
    out << ", ";
  }

  // member: rotor_temp
  {
    out << "rotor_temp: ";
    rosidl_generator_traits::value_to_yaml(msg.rotor_temp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MotorStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: arm_name
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "arm_name: ";
    rosidl_generator_traits::value_to_yaml(msg.arm_name, out);
    out << "\n";
  }

  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: error
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "error: ";
    rosidl_generator_traits::value_to_yaml(msg.error, out);
    out << "\n";
  }

  // member: error_name
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "error_name: ";
    rosidl_generator_traits::value_to_yaml(msg.error_name, out);
    out << "\n";
  }

  // member: mos_temp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "mos_temp: ";
    rosidl_generator_traits::value_to_yaml(msg.mos_temp, out);
    out << "\n";
  }

  // member: rotor_temp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "rotor_temp: ";
    rosidl_generator_traits::value_to_yaml(msg.rotor_temp, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MotorStatus & msg, bool use_flow_style = false)
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
  const hw_interface::msg::MotorStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  hw_interface::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hw_interface::msg::to_yaml() instead")]]
inline std::string to_yaml(const hw_interface::msg::MotorStatus & msg)
{
  return hw_interface::msg::to_yaml(msg);
}

template<>
inline const char * data_type<hw_interface::msg::MotorStatus>()
{
  return "hw_interface::msg::MotorStatus";
}

template<>
inline const char * name<hw_interface::msg::MotorStatus>()
{
  return "hw_interface/msg/MotorStatus";
}

template<>
struct has_fixed_size<hw_interface::msg::MotorStatus>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<hw_interface::msg::MotorStatus>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<hw_interface::msg::MotorStatus>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__TRAITS_HPP_
