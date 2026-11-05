// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/srv/set_motor_gains.hpp"


#ifndef HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__BUILDER_HPP_
#define HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace hw_interface
{

namespace srv
{

namespace builder
{

class Init_SetMotorGains_Request_kd
{
public:
  explicit Init_SetMotorGains_Request_kd(::hw_interface::srv::SetMotorGains_Request & msg)
  : msg_(msg)
  {}
  ::hw_interface::srv::SetMotorGains_Request kd(::hw_interface::srv::SetMotorGains_Request::_kd_type arg)
  {
    msg_.kd = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Request msg_;
};

class Init_SetMotorGains_Request_kp
{
public:
  explicit Init_SetMotorGains_Request_kp(::hw_interface::srv::SetMotorGains_Request & msg)
  : msg_(msg)
  {}
  Init_SetMotorGains_Request_kd kp(::hw_interface::srv::SetMotorGains_Request::_kp_type arg)
  {
    msg_.kp = std::move(arg);
    return Init_SetMotorGains_Request_kd(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Request msg_;
};

class Init_SetMotorGains_Request_motor_ids
{
public:
  Init_SetMotorGains_Request_motor_ids()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetMotorGains_Request_kp motor_ids(::hw_interface::srv::SetMotorGains_Request::_motor_ids_type arg)
  {
    msg_.motor_ids = std::move(arg);
    return Init_SetMotorGains_Request_kp(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::hw_interface::srv::SetMotorGains_Request>()
{
  return hw_interface::srv::builder::Init_SetMotorGains_Request_motor_ids();
}

}  // namespace hw_interface


namespace hw_interface
{

namespace srv
{

namespace builder
{

class Init_SetMotorGains_Response_message
{
public:
  explicit Init_SetMotorGains_Response_message(::hw_interface::srv::SetMotorGains_Response & msg)
  : msg_(msg)
  {}
  ::hw_interface::srv::SetMotorGains_Response message(::hw_interface::srv::SetMotorGains_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Response msg_;
};

class Init_SetMotorGains_Response_success
{
public:
  Init_SetMotorGains_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetMotorGains_Response_message success(::hw_interface::srv::SetMotorGains_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_SetMotorGains_Response_message(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::hw_interface::srv::SetMotorGains_Response>()
{
  return hw_interface::srv::builder::Init_SetMotorGains_Response_success();
}

}  // namespace hw_interface


namespace hw_interface
{

namespace srv
{

namespace builder
{

class Init_SetMotorGains_Event_response
{
public:
  explicit Init_SetMotorGains_Event_response(::hw_interface::srv::SetMotorGains_Event & msg)
  : msg_(msg)
  {}
  ::hw_interface::srv::SetMotorGains_Event response(::hw_interface::srv::SetMotorGains_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Event msg_;
};

class Init_SetMotorGains_Event_request
{
public:
  explicit Init_SetMotorGains_Event_request(::hw_interface::srv::SetMotorGains_Event & msg)
  : msg_(msg)
  {}
  Init_SetMotorGains_Event_response request(::hw_interface::srv::SetMotorGains_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SetMotorGains_Event_response(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Event msg_;
};

class Init_SetMotorGains_Event_info
{
public:
  Init_SetMotorGains_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetMotorGains_Event_request info(::hw_interface::srv::SetMotorGains_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SetMotorGains_Event_request(msg_);
  }

private:
  ::hw_interface::srv::SetMotorGains_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::hw_interface::srv::SetMotorGains_Event>()
{
  return hw_interface::srv::builder::Init_SetMotorGains_Event_info();
}

}  // namespace hw_interface

#endif  // HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__BUILDER_HPP_
