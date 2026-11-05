// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "hw_interface/msg/detail/motor_status_array__rosidl_typesupport_introspection_c.h"
#include "hw_interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "hw_interface/msg/detail/motor_status_array__functions.h"
#include "hw_interface/msg/detail/motor_status_array__struct.h"


// Include directives for member types
// Member `motors`
#include "hw_interface/msg/motor_status.h"
// Member `motors`
#include "hw_interface/msg/detail/motor_status__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  hw_interface__msg__MotorStatusArray__init(message_memory);
}

void hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_fini_function(void * message_memory)
{
  hw_interface__msg__MotorStatusArray__fini(message_memory);
}

size_t hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__size_function__MotorStatusArray__motors(
  const void * untyped_member)
{
  const hw_interface__msg__MotorStatus__Sequence * member =
    (const hw_interface__msg__MotorStatus__Sequence *)(untyped_member);
  return member->size;
}

const void * hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_const_function__MotorStatusArray__motors(
  const void * untyped_member, size_t index)
{
  const hw_interface__msg__MotorStatus__Sequence * member =
    (const hw_interface__msg__MotorStatus__Sequence *)(untyped_member);
  return &member->data[index];
}

void * hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_function__MotorStatusArray__motors(
  void * untyped_member, size_t index)
{
  hw_interface__msg__MotorStatus__Sequence * member =
    (hw_interface__msg__MotorStatus__Sequence *)(untyped_member);
  return &member->data[index];
}

void hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__fetch_function__MotorStatusArray__motors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const hw_interface__msg__MotorStatus * item =
    ((const hw_interface__msg__MotorStatus *)
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_const_function__MotorStatusArray__motors(untyped_member, index));
  hw_interface__msg__MotorStatus * value =
    (hw_interface__msg__MotorStatus *)(untyped_value);
  *value = *item;
}

void hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__assign_function__MotorStatusArray__motors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  hw_interface__msg__MotorStatus * item =
    ((hw_interface__msg__MotorStatus *)
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_function__MotorStatusArray__motors(untyped_member, index));
  const hw_interface__msg__MotorStatus * value =
    (const hw_interface__msg__MotorStatus *)(untyped_value);
  *item = *value;
}

bool hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__resize_function__MotorStatusArray__motors(
  void * untyped_member, size_t size)
{
  hw_interface__msg__MotorStatus__Sequence * member =
    (hw_interface__msg__MotorStatus__Sequence *)(untyped_member);
  hw_interface__msg__MotorStatus__Sequence__fini(member);
  return hw_interface__msg__MotorStatus__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_member_array[1] = {
  {
    "motors",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(hw_interface__msg__MotorStatusArray, motors),  // bytes offset in struct
    NULL,  // default value
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__size_function__MotorStatusArray__motors,  // size() function pointer
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_const_function__MotorStatusArray__motors,  // get_const(index) function pointer
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__get_function__MotorStatusArray__motors,  // get(index) function pointer
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__fetch_function__MotorStatusArray__motors,  // fetch(index, &value) function pointer
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__assign_function__MotorStatusArray__motors,  // assign(index, value) function pointer
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__resize_function__MotorStatusArray__motors  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_members = {
  "hw_interface__msg",  // message namespace
  "MotorStatusArray",  // message name
  1,  // number of fields
  sizeof(hw_interface__msg__MotorStatusArray),
  false,  // has_any_key_member_
  hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_member_array,  // message members
  hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_init_function,  // function to initialize message memory (memory has to be allocated)
  hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_type_support_handle = {
  0,
  &hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_members,
  get_message_typesupport_handle_function,
  &hw_interface__msg__MotorStatusArray__get_type_hash,
  &hw_interface__msg__MotorStatusArray__get_type_description,
  &hw_interface__msg__MotorStatusArray__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_hw_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, hw_interface, msg, MotorStatusArray)() {
  hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, hw_interface, msg, MotorStatus)();
  if (!hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_type_support_handle.typesupport_identifier) {
    hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &hw_interface__msg__MotorStatusArray__rosidl_typesupport_introspection_c__MotorStatusArray_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
