// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status_array.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__BUILDER_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "hw_interface/msg/detail/motor_status_array__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace hw_interface
{

namespace msg
{

namespace builder
{

class Init_MotorStatusArray_motors
{
public:
  Init_MotorStatusArray_motors()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::hw_interface::msg::MotorStatusArray motors(::hw_interface::msg::MotorStatusArray::_motors_type arg)
  {
    msg_.motors = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hw_interface::msg::MotorStatusArray msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::hw_interface::msg::MotorStatusArray>()
{
  return hw_interface::msg::builder::Init_MotorStatusArray_motors();
}

}  // namespace hw_interface

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__BUILDER_HPP_
