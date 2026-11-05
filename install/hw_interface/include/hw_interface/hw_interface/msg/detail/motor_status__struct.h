// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status.h"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_H_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'arm_name'
// Member 'error_name'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/MotorStatus in the package hw_interface.
typedef struct hw_interface__msg__MotorStatus
{
  rosidl_runtime_c__String arm_name;
  int32_t id;
  int32_t error;
  rosidl_runtime_c__String error_name;
  float mos_temp;
  float rotor_temp;
} hw_interface__msg__MotorStatus;

// Struct for a sequence of hw_interface__msg__MotorStatus.
typedef struct hw_interface__msg__MotorStatus__Sequence
{
  hw_interface__msg__MotorStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hw_interface__msg__MotorStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_H_
