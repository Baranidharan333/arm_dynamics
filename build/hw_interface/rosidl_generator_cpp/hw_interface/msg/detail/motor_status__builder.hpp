// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__BUILDER_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "hw_interface/msg/detail/motor_status__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace hw_interface
{

namespace msg
{

namespace builder
{

class Init_MotorStatus_rotor_temp
{
public:
  explicit Init_MotorStatus_rotor_temp(::hw_interface::msg::MotorStatus & msg)
  : msg_(msg)
  {}
  ::hw_interface::msg::MotorStatus rotor_temp(::hw_interface::msg::MotorStatus::_rotor_temp_type arg)
  {
    msg_.rotor_temp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

class Init_MotorStatus_mos_temp
{
public:
  explicit Init_MotorStatus_mos_temp(::hw_interface::msg::MotorStatus & msg)
  : msg_(msg)
  {}
  Init_MotorStatus_rotor_temp mos_temp(::hw_interface::msg::MotorStatus::_mos_temp_type arg)
  {
    msg_.mos_temp = std::move(arg);
    return Init_MotorStatus_rotor_temp(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

class Init_MotorStatus_error_name
{
public:
  explicit Init_MotorStatus_error_name(::hw_interface::msg::MotorStatus & msg)
  : msg_(msg)
  {}
  Init_MotorStatus_mos_temp error_name(::hw_interface::msg::MotorStatus::_error_name_type arg)
  {
    msg_.error_name = std::move(arg);
    return Init_MotorStatus_mos_temp(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

class Init_MotorStatus_error
{
public:
  explicit Init_MotorStatus_error(::hw_interface::msg::MotorStatus & msg)
  : msg_(msg)
  {}
  Init_MotorStatus_error_name error(::hw_interface::msg::MotorStatus::_error_type arg)
  {
    msg_.error = std::move(arg);
    return Init_MotorStatus_error_name(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

class Init_MotorStatus_id
{
public:
  explicit Init_MotorStatus_id(::hw_interface::msg::MotorStatus & msg)
  : msg_(msg)
  {}
  Init_MotorStatus_error id(::hw_interface::msg::MotorStatus::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_MotorStatus_error(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

class Init_MotorStatus_arm_name
{
public:
  Init_MotorStatus_arm_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MotorStatus_id arm_name(::hw_interface::msg::MotorStatus::_arm_name_type arg)
  {
    msg_.arm_name = std::move(arg);
    return Init_MotorStatus_id(msg_);
  }

private:
  ::hw_interface::msg::MotorStatus msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::hw_interface::msg::MotorStatus>()
{
  return hw_interface::msg::builder::Init_MotorStatus_arm_name();
}

}  // namespace hw_interface

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__BUILDER_HPP_
