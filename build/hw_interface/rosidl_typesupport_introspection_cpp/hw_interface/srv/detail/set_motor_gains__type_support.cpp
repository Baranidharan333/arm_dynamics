// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "hw_interface/srv/detail/set_motor_gains__functions.h"
#include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void SetMotorGains_Request_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) hw_interface::srv::SetMotorGains_Request(_init);
}

void SetMotorGains_Request_fini_function(void * message_memory)
{
  auto typed_message = static_cast<hw_interface::srv::SetMotorGains_Request *>(message_memory);
  typed_message->~SetMotorGains_Request();
}

size_t size_function__SetMotorGains_Request__motor_ids(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<int32_t> *>(untyped_member);
  return member->size();
}

const void * get_const_function__SetMotorGains_Request__motor_ids(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<int32_t> *>(untyped_member);
  return &member[index];
}

void * get_function__SetMotorGains_Request__motor_ids(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<int32_t> *>(untyped_member);
  return &member[index];
}

void fetch_function__SetMotorGains_Request__motor_ids(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const int32_t *>(
    get_const_function__SetMotorGains_Request__motor_ids(untyped_member, index));
  auto & value = *reinterpret_cast<int32_t *>(untyped_value);
  value = item;
}

void assign_function__SetMotorGains_Request__motor_ids(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<int32_t *>(
    get_function__SetMotorGains_Request__motor_ids(untyped_member, index));
  const auto & value = *reinterpret_cast<const int32_t *>(untyped_value);
  item = value;
}

void resize_function__SetMotorGains_Request__motor_ids(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<int32_t> *>(untyped_member);
  member->resize(size);
}

size_t size_function__SetMotorGains_Request__kp(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__SetMotorGains_Request__kp(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__SetMotorGains_Request__kp(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__SetMotorGains_Request__kp(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__SetMotorGains_Request__kp(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__SetMotorGains_Request__kp(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__SetMotorGains_Request__kp(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__SetMotorGains_Request__kp(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

size_t size_function__SetMotorGains_Request__kd(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<float> *>(untyped_member);
  return member->size();
}

const void * get_const_function__SetMotorGains_Request__kd(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<float> *>(untyped_member);
  return &member[index];
}

void * get_function__SetMotorGains_Request__kd(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<float> *>(untyped_member);
  return &member[index];
}

void fetch_function__SetMotorGains_Request__kd(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const float *>(
    get_const_function__SetMotorGains_Request__kd(untyped_member, index));
  auto & value = *reinterpret_cast<float *>(untyped_value);
  value = item;
}

void assign_function__SetMotorGains_Request__kd(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<float *>(
    get_function__SetMotorGains_Request__kd(untyped_member, index));
  const auto & value = *reinterpret_cast<const float *>(untyped_value);
  item = value;
}

void resize_function__SetMotorGains_Request__kd(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<float> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember SetMotorGains_Request_message_member_array[3] = {
  {
    "motor_ids",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Request, motor_ids),  // bytes offset in struct
    nullptr,  // default value
    size_function__SetMotorGains_Request__motor_ids,  // size() function pointer
    get_const_function__SetMotorGains_Request__motor_ids,  // get_const(index) function pointer
    get_function__SetMotorGains_Request__motor_ids,  // get(index) function pointer
    fetch_function__SetMotorGains_Request__motor_ids,  // fetch(index, &value) function pointer
    assign_function__SetMotorGains_Request__motor_ids,  // assign(index, value) function pointer
    resize_function__SetMotorGains_Request__motor_ids  // resize(index) function pointer
  },
  {
    "kp",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Request, kp),  // bytes offset in struct
    nullptr,  // default value
    size_function__SetMotorGains_Request__kp,  // size() function pointer
    get_const_function__SetMotorGains_Request__kp,  // get_const(index) function pointer
    get_function__SetMotorGains_Request__kp,  // get(index) function pointer
    fetch_function__SetMotorGains_Request__kp,  // fetch(index, &value) function pointer
    assign_function__SetMotorGains_Request__kp,  // assign(index, value) function pointer
    resize_function__SetMotorGains_Request__kp  // resize(index) function pointer
  },
  {
    "kd",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Request, kd),  // bytes offset in struct
    nullptr,  // default value
    size_function__SetMotorGains_Request__kd,  // size() function pointer
    get_const_function__SetMotorGains_Request__kd,  // get_const(index) function pointer
    get_function__SetMotorGains_Request__kd,  // get(index) function pointer
    fetch_function__SetMotorGains_Request__kd,  // fetch(index, &value) function pointer
    assign_function__SetMotorGains_Request__kd,  // assign(index, value) function pointer
    resize_function__SetMotorGains_Request__kd  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers SetMotorGains_Request_message_members = {
  "hw_interface::srv",  // message namespace
  "SetMotorGains_Request",  // message name
  3,  // number of fields
  sizeof(hw_interface::srv::SetMotorGains_Request),
  false,  // has_any_key_member_
  SetMotorGains_Request_message_member_array,  // message members
  SetMotorGains_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  SetMotorGains_Request_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t SetMotorGains_Request_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &SetMotorGains_Request_message_members,
  get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Request__get_type_hash,
  &hw_interface__srv__SetMotorGains_Request__get_type_description,
  &hw_interface__srv__SetMotorGains_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace hw_interface


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>()
{
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Request_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Request)() {
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "array"
// already included above
// #include "cstddef"
// already included above
// #include "string"
// already included above
// #include "vector"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/field_types.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void SetMotorGains_Response_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) hw_interface::srv::SetMotorGains_Response(_init);
}

void SetMotorGains_Response_fini_function(void * message_memory)
{
  auto typed_message = static_cast<hw_interface::srv::SetMotorGains_Response *>(message_memory);
  typed_message->~SetMotorGains_Response();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember SetMotorGains_Response_message_member_array[2] = {
  {
    "success",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Response, success),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "message",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Response, message),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers SetMotorGains_Response_message_members = {
  "hw_interface::srv",  // message namespace
  "SetMotorGains_Response",  // message name
  2,  // number of fields
  sizeof(hw_interface::srv::SetMotorGains_Response),
  false,  // has_any_key_member_
  SetMotorGains_Response_message_member_array,  // message members
  SetMotorGains_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  SetMotorGains_Response_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t SetMotorGains_Response_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &SetMotorGains_Response_message_members,
  get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Response__get_type_hash,
  &hw_interface__srv__SetMotorGains_Response__get_type_description,
  &hw_interface__srv__SetMotorGains_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace hw_interface


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>()
{
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Response_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Response)() {
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "array"
// already included above
// #include "cstddef"
// already included above
// #include "string"
// already included above
// #include "vector"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/field_types.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

void SetMotorGains_Event_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) hw_interface::srv::SetMotorGains_Event(_init);
}

void SetMotorGains_Event_fini_function(void * message_memory)
{
  auto typed_message = static_cast<hw_interface::srv::SetMotorGains_Event *>(message_memory);
  typed_message->~SetMotorGains_Event();
}

size_t size_function__SetMotorGains_Event__request(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<hw_interface::srv::SetMotorGains_Request> *>(untyped_member);
  return member->size();
}

const void * get_const_function__SetMotorGains_Event__request(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<hw_interface::srv::SetMotorGains_Request> *>(untyped_member);
  return &member[index];
}

void * get_function__SetMotorGains_Event__request(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<hw_interface::srv::SetMotorGains_Request> *>(untyped_member);
  return &member[index];
}

void fetch_function__SetMotorGains_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const hw_interface::srv::SetMotorGains_Request *>(
    get_const_function__SetMotorGains_Event__request(untyped_member, index));
  auto & value = *reinterpret_cast<hw_interface::srv::SetMotorGains_Request *>(untyped_value);
  value = item;
}

void assign_function__SetMotorGains_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<hw_interface::srv::SetMotorGains_Request *>(
    get_function__SetMotorGains_Event__request(untyped_member, index));
  const auto & value = *reinterpret_cast<const hw_interface::srv::SetMotorGains_Request *>(untyped_value);
  item = value;
}

void resize_function__SetMotorGains_Event__request(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<hw_interface::srv::SetMotorGains_Request> *>(untyped_member);
  member->resize(size);
}

size_t size_function__SetMotorGains_Event__response(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<hw_interface::srv::SetMotorGains_Response> *>(untyped_member);
  return member->size();
}

const void * get_const_function__SetMotorGains_Event__response(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<hw_interface::srv::SetMotorGains_Response> *>(untyped_member);
  return &member[index];
}

void * get_function__SetMotorGains_Event__response(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<hw_interface::srv::SetMotorGains_Response> *>(untyped_member);
  return &member[index];
}

void fetch_function__SetMotorGains_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const hw_interface::srv::SetMotorGains_Response *>(
    get_const_function__SetMotorGains_Event__response(untyped_member, index));
  auto & value = *reinterpret_cast<hw_interface::srv::SetMotorGains_Response *>(untyped_value);
  value = item;
}

void assign_function__SetMotorGains_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<hw_interface::srv::SetMotorGains_Response *>(
    get_function__SetMotorGains_Event__response(untyped_member, index));
  const auto & value = *reinterpret_cast<const hw_interface::srv::SetMotorGains_Response *>(untyped_value);
  item = value;
}

void resize_function__SetMotorGains_Event__response(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<hw_interface::srv::SetMotorGains_Response> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember SetMotorGains_Event_message_member_array[3] = {
  {
    "info",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<service_msgs::msg::ServiceEventInfo>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Event, info),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "request",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>(),  // members of sub message
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Event, request),  // bytes offset in struct
    nullptr,  // default value
    size_function__SetMotorGains_Event__request,  // size() function pointer
    get_const_function__SetMotorGains_Event__request,  // get_const(index) function pointer
    get_function__SetMotorGains_Event__request,  // get(index) function pointer
    fetch_function__SetMotorGains_Event__request,  // fetch(index, &value) function pointer
    assign_function__SetMotorGains_Event__request,  // assign(index, value) function pointer
    resize_function__SetMotorGains_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>(),  // members of sub message
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(hw_interface::srv::SetMotorGains_Event, response),  // bytes offset in struct
    nullptr,  // default value
    size_function__SetMotorGains_Event__response,  // size() function pointer
    get_const_function__SetMotorGains_Event__response,  // get_const(index) function pointer
    get_function__SetMotorGains_Event__response,  // get(index) function pointer
    fetch_function__SetMotorGains_Event__response,  // fetch(index, &value) function pointer
    assign_function__SetMotorGains_Event__response,  // assign(index, value) function pointer
    resize_function__SetMotorGains_Event__response  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers SetMotorGains_Event_message_members = {
  "hw_interface::srv",  // message namespace
  "SetMotorGains_Event",  // message name
  3,  // number of fields
  sizeof(hw_interface::srv::SetMotorGains_Event),
  false,  // has_any_key_member_
  SetMotorGains_Event_message_member_array,  // message members
  SetMotorGains_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  SetMotorGains_Event_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t SetMotorGains_Event_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &SetMotorGains_Event_message_members,
  get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Event__get_type_hash,
  &hw_interface__srv__SetMotorGains_Event__get_type_description,
  &hw_interface__srv__SetMotorGains_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace hw_interface


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Event>()
{
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Event_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Event)() {
  return &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "rosidl_typesupport_introspection_cpp/visibility_control.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/service_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/service_type_support_decl.hpp"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_introspection_cpp
{

// this is intentionally not const to allow initialization later to prevent an initialization race
static ::rosidl_typesupport_introspection_cpp::ServiceMembers SetMotorGains_service_members = {
  "hw_interface::srv",  // service namespace
  "SetMotorGains",  // service name
  // the following fields are initialized below on first access
  // see get_service_type_support_handle<hw_interface::srv::SetMotorGains>()
  nullptr,  // request message
  nullptr,  // response message
  nullptr,  // event message
};

static const rosidl_service_type_support_t SetMotorGains_service_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &SetMotorGains_service_members,
  get_service_typesupport_handle_function,
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>(),
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>(),
  ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<hw_interface::srv::SetMotorGains>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<hw_interface::srv::SetMotorGains>,
  &hw_interface__srv__SetMotorGains__get_type_hash,
  &hw_interface__srv__SetMotorGains__get_type_description,
  &hw_interface__srv__SetMotorGains__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace srv

}  // namespace hw_interface


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<hw_interface::srv::SetMotorGains>()
{
  // get a handle to the value to be returned
  auto service_type_support =
    &::hw_interface::srv::rosidl_typesupport_introspection_cpp::SetMotorGains_service_type_support_handle;
  // get a non-const and properly typed version of the data void *
  auto service_members = const_cast<::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
    static_cast<const ::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
      service_type_support->data));
  // make sure all of the service_members are initialized
  // if they are not, initialize them
  if (
    service_members->request_members_ == nullptr ||
    service_members->response_members_ == nullptr ||
    service_members->event_members_ == nullptr)
  {
    // initialize the request_members_ with the static function from the external library
    service_members->request_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::hw_interface::srv::SetMotorGains_Request
      >()->data
      );
    // initialize the response_members_ with the static function from the external library
    service_members->response_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::hw_interface::srv::SetMotorGains_Response
      >()->data
      );
    // initialize the event_members_ with the static function from the external library
    service_members->event_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::hw_interface::srv::SetMotorGains_Event
      >()->data
      );
  }
  // finally return the properly initialized service_type_support handle
  return service_type_support;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains)() {
  return ::rosidl_typesupport_introspection_cpp::get_service_type_support_handle<hw_interface::srv::SetMotorGains>();
}

#ifdef __cplusplus
}
#endif
