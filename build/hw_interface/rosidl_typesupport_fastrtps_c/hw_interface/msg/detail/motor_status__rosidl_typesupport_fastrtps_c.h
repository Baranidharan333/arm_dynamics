// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice
#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "hw_interface/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "hw_interface/msg/detail/motor_status__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
bool cdr_serialize_hw_interface__msg__MotorStatus(
  const hw_interface__msg__MotorStatus * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
bool cdr_deserialize_hw_interface__msg__MotorStatus(
  eprosima::fastcdr::Cdr &,
  hw_interface__msg__MotorStatus * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
size_t get_serialized_size_hw_interface__msg__MotorStatus(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
size_t max_serialized_size_hw_interface__msg__MotorStatus(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
bool cdr_serialize_key_hw_interface__msg__MotorStatus(
  const hw_interface__msg__MotorStatus * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
size_t get_serialized_size_key_hw_interface__msg__MotorStatus(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
size_t max_serialized_size_key_hw_interface__msg__MotorStatus(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hw_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, hw_interface, msg, MotorStatus)();

#ifdef __cplusplus
}
#endif

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
