// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status_array.h"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_H_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'motors'
#include "hw_interface/msg/detail/motor_status__struct.h"

/// Struct defined in msg/MotorStatusArray in the package hw_interface.
typedef struct hw_interface__msg__MotorStatusArray
{
  hw_interface__msg__MotorStatus__Sequence motors;
} hw_interface__msg__MotorStatusArray;

// Struct for a sequence of hw_interface__msg__MotorStatusArray.
typedef struct hw_interface__msg__MotorStatusArray__Sequence
{
  hw_interface__msg__MotorStatusArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hw_interface__msg__MotorStatusArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_H_
