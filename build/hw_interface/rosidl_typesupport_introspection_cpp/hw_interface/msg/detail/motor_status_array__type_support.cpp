// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "hw_interface/msg/detail/motor_status_array__functions.h"
#include "hw_interface/msg/detail/motor_status_array__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace hw_interface
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void MotorStatusArray_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) hw_interface::msg::MotorStatusArray(_init);
}

void MotorStatusArray_fini_function(void * message_memory)
{
  auto typed_message = static_cast<hw_interface::msg::MotorStatusArray *>(message_memory);
  typed_message->~MotorStatusArray();
}

size_t size_function__MotorStatusArray__motors(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<hw_interface::msg::MotorStatus> *>(untyped_member);
  return member->size();
}

const void * get_const_function__MotorStatusArray__motors(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<hw_interface::msg::MotorStatus> *>(untyped_member);
  return &member[index];
}

void * get_function__MotorStatusArray__motors(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<hw_interface::msg::MotorStatus> *>(untyped_member);
  return &member[index];
}

void fetch_function__MotorStatusArray__motors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const hw_interface::msg::MotorStatus *>(
    get_const_function__MotorStatusArray__motors(untyped_member, index));
  auto & value = *reinterpret_cast<hw_interface::msg::MotorStatus *>(untyped_value);
  value = item;
}

void assign_function__MotorStatusArray__motors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<hw_interface::msg::MotorStatus *>(
    get_function__MotorStatusArray__motors(untyped_member, index));
  const auto & value = *reinterpret_cast<const hw_interface::msg::MotorStatus *>(untyped_value);
  item = value;
}

void resize_function__MotorStatusArray__motors(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<hw_interface::msg::MotorStatus> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember MotorStatusArray_message_member_array[1] = {
  {
    "motors",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<hw_interface::msg::MotorStatus>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface::msg::MotorStatusArray, motors),  // bytes offset in struct
    nullptr,  // default value
    size_function__MotorStatusArray__motors,  // size() function pointer
    get_const_function__MotorStatusArray__motors,  // get_const(index) function pointer
    get_function__MotorStatusArray__motors,  // get(index) function pointer
    fetch_function__MotorStatusArray__motors,  // fetch(index, &value) function pointer
    assign_function__MotorStatusArray__motors,  // assign(index, value) function pointer
    resize_function__MotorStatusArray__motors  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers MotorStatusArray_message_members = {
  "hw_interface::msg",  // message namespace
  "MotorStatusArray",  // message name
  1,  // number of fields
  sizeof(hw_interface::msg::MotorStatusArray),
  false,  // has_any_key_member_
  MotorStatusArray_message_member_array,  // message members
  MotorStatusArray_init_function,  // function to initialize message memory (memory has to be allocated)
  MotorStatusArray_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t MotorStatusArray_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &MotorStatusArray_message_members,
  get_message_typesupport_handle_function,
  &hw_interface__msg__MotorStatusArray__get_type_hash,
  &hw_interface__msg__MotorStatusArray__get_type_description,
  &hw_interface__msg__MotorStatusArray__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace hw_interface


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::msg::MotorStatusArray>()
{
  return &::hw_interface::msg::rosidl_typesupport_introspection_cpp::MotorStatusArray_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, msg, MotorStatusArray)() {
  return &::hw_interface::msg::rosidl_typesupport_introspection_cpp::MotorStatusArray_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
