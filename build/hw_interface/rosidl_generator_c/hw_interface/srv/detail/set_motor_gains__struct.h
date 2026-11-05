// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/srv/set_motor_gains.h"


#ifndef HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__STRUCT_H_
#define HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'motor_ids'
// Member 'kp'
// Member 'kd'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in srv/SetMotorGains in the package hw_interface.
typedef struct hw_interface__srv__SetMotorGains_Request
{
  rosidl_runtime_c__int32__Sequence motor_ids;
  rosidl_runtime_c__float__Sequence kp;
  rosidl_runtime_c__float__Sequence kd;
} hw_interface__srv__SetMotorGains_Request;

// Struct for a sequence of hw_interface__srv__SetMotorGains_Request.
typedef struct hw_interface__srv__SetMotorGains_Request__Sequence
{
  hw_interface__srv__SetMotorGains_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hw_interface__srv__SetMotorGains_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/SetMotorGains in the package hw_interface.
typedef struct hw_interface__srv__SetMotorGains_Response
{
  bool success;
  rosidl_runtime_c__String message;
} hw_interface__srv__SetMotorGains_Response;

// Struct for a sequence of hw_interface__srv__SetMotorGains_Response.
typedef struct hw_interface__srv__SetMotorGains_Response__Sequence
{
  hw_interface__srv__SetMotorGains_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hw_interface__srv__SetMotorGains_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  hw_interface__srv__SetMotorGains_Event__request__MAX_SIZE = 1
};
// response
enum
{
  hw_interface__srv__SetMotorGains_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/SetMotorGains in the package hw_interface.
typedef struct hw_interface__srv__SetMotorGains_Event
{
  service_msgs__msg__ServiceEventInfo info;
  hw_interface__srv__SetMotorGains_Request__Sequence request;
  hw_interface__srv__SetMotorGains_Response__Sequence response;
} hw_interface__srv__SetMotorGains_Event;

// Struct for a sequence of hw_interface__srv__SetMotorGains_Event.
typedef struct hw_interface__srv__SetMotorGains_Event__Sequence
{
  hw_interface__srv__SetMotorGains_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hw_interface__srv__SetMotorGains_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__STRUCT_H_
