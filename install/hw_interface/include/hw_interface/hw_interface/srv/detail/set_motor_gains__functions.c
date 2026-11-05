// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice
#include "hw_interface/srv/detail/set_motor_gains__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `motor_ids`
// Member `kp`
// Member `kd`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
hw_interface__srv__SetMotorGains_Request__init(hw_interface__srv__SetMotorGains_Request * msg)
{
  if (!msg) {
    return false;
  }
  // motor_ids
  if (!rosidl_runtime_c__int32__Sequence__init(&msg->motor_ids, 0)) {
    hw_interface__srv__SetMotorGains_Request__fini(msg);
    return false;
  }
  // kp
  if (!rosidl_runtime_c__float__Sequence__init(&msg->kp, 0)) {
    hw_interface__srv__SetMotorGains_Request__fini(msg);
    return false;
  }
  // kd
  if (!rosidl_runtime_c__float__Sequence__init(&msg->kd, 0)) {
    hw_interface__srv__SetMotorGains_Request__fini(msg);
    return false;
  }
  return true;
}

void
hw_interface__srv__SetMotorGains_Request__fini(hw_interface__srv__SetMotorGains_Request * msg)
{
  if (!msg) {
    return;
  }
  // motor_ids
  rosidl_runtime_c__int32__Sequence__fini(&msg->motor_ids);
  // kp
  rosidl_runtime_c__float__Sequence__fini(&msg->kp);
  // kd
  rosidl_runtime_c__float__Sequence__fini(&msg->kd);
}

bool
hw_interface__srv__SetMotorGains_Request__are_equal(const hw_interface__srv__SetMotorGains_Request * lhs, const hw_interface__srv__SetMotorGains_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // motor_ids
  if (!rosidl_runtime_c__int32__Sequence__are_equal(
      &(lhs->motor_ids), &(rhs->motor_ids)))
  {
    return false;
  }
  // kp
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->kp), &(rhs->kp)))
  {
    return false;
  }
  // kd
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->kd), &(rhs->kd)))
  {
    return false;
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Request__copy(
  const hw_interface__srv__SetMotorGains_Request * input,
  hw_interface__srv__SetMotorGains_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // motor_ids
  if (!rosidl_runtime_c__int32__Sequence__copy(
      &(input->motor_ids), &(output->motor_ids)))
  {
    return false;
  }
  // kp
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->kp), &(output->kp)))
  {
    return false;
  }
  // kd
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->kd), &(output->kd)))
  {
    return false;
  }
  return true;
}

hw_interface__srv__SetMotorGains_Request *
hw_interface__srv__SetMotorGains_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Request * msg = (hw_interface__srv__SetMotorGains_Request *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hw_interface__srv__SetMotorGains_Request));
  bool success = hw_interface__srv__SetMotorGains_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hw_interface__srv__SetMotorGains_Request__destroy(hw_interface__srv__SetMotorGains_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hw_interface__srv__SetMotorGains_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hw_interface__srv__SetMotorGains_Request__Sequence__init(hw_interface__srv__SetMotorGains_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Request)) {
      return false;
    }
    data = (hw_interface__srv__SetMotorGains_Request *)allocator.zero_allocate(size, sizeof(hw_interface__srv__SetMotorGains_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hw_interface__srv__SetMotorGains_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hw_interface__srv__SetMotorGains_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
hw_interface__srv__SetMotorGains_Request__Sequence__fini(hw_interface__srv__SetMotorGains_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      hw_interface__srv__SetMotorGains_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

hw_interface__srv__SetMotorGains_Request__Sequence *
hw_interface__srv__SetMotorGains_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Request__Sequence * array = (hw_interface__srv__SetMotorGains_Request__Sequence *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hw_interface__srv__SetMotorGains_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hw_interface__srv__SetMotorGains_Request__Sequence__destroy(hw_interface__srv__SetMotorGains_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hw_interface__srv__SetMotorGains_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hw_interface__srv__SetMotorGains_Request__Sequence__are_equal(const hw_interface__srv__SetMotorGains_Request__Sequence * lhs, const hw_interface__srv__SetMotorGains_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Request__Sequence__copy(
  const hw_interface__srv__SetMotorGains_Request__Sequence * input,
  hw_interface__srv__SetMotorGains_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(hw_interface__srv__SetMotorGains_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hw_interface__srv__SetMotorGains_Request * data =
      (hw_interface__srv__SetMotorGains_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hw_interface__srv__SetMotorGains_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hw_interface__srv__SetMotorGains_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `message`
#include "rosidl_runtime_c/string_functions.h"

bool
hw_interface__srv__SetMotorGains_Response__init(hw_interface__srv__SetMotorGains_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    hw_interface__srv__SetMotorGains_Response__fini(msg);
    return false;
  }
  return true;
}

void
hw_interface__srv__SetMotorGains_Response__fini(hw_interface__srv__SetMotorGains_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // message
  rosidl_runtime_c__String__fini(&msg->message);
}

bool
hw_interface__srv__SetMotorGains_Response__are_equal(const hw_interface__srv__SetMotorGains_Response * lhs, const hw_interface__srv__SetMotorGains_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Response__copy(
  const hw_interface__srv__SetMotorGains_Response * input,
  hw_interface__srv__SetMotorGains_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  return true;
}

hw_interface__srv__SetMotorGains_Response *
hw_interface__srv__SetMotorGains_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Response * msg = (hw_interface__srv__SetMotorGains_Response *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hw_interface__srv__SetMotorGains_Response));
  bool success = hw_interface__srv__SetMotorGains_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hw_interface__srv__SetMotorGains_Response__destroy(hw_interface__srv__SetMotorGains_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hw_interface__srv__SetMotorGains_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hw_interface__srv__SetMotorGains_Response__Sequence__init(hw_interface__srv__SetMotorGains_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Response)) {
      return false;
    }
    data = (hw_interface__srv__SetMotorGains_Response *)allocator.zero_allocate(size, sizeof(hw_interface__srv__SetMotorGains_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hw_interface__srv__SetMotorGains_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hw_interface__srv__SetMotorGains_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
hw_interface__srv__SetMotorGains_Response__Sequence__fini(hw_interface__srv__SetMotorGains_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      hw_interface__srv__SetMotorGains_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

hw_interface__srv__SetMotorGains_Response__Sequence *
hw_interface__srv__SetMotorGains_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Response__Sequence * array = (hw_interface__srv__SetMotorGains_Response__Sequence *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hw_interface__srv__SetMotorGains_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hw_interface__srv__SetMotorGains_Response__Sequence__destroy(hw_interface__srv__SetMotorGains_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hw_interface__srv__SetMotorGains_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hw_interface__srv__SetMotorGains_Response__Sequence__are_equal(const hw_interface__srv__SetMotorGains_Response__Sequence * lhs, const hw_interface__srv__SetMotorGains_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Response__Sequence__copy(
  const hw_interface__srv__SetMotorGains_Response__Sequence * input,
  hw_interface__srv__SetMotorGains_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(hw_interface__srv__SetMotorGains_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hw_interface__srv__SetMotorGains_Response * data =
      (hw_interface__srv__SetMotorGains_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hw_interface__srv__SetMotorGains_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hw_interface__srv__SetMotorGains_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"

bool
hw_interface__srv__SetMotorGains_Event__init(hw_interface__srv__SetMotorGains_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    hw_interface__srv__SetMotorGains_Event__fini(msg);
    return false;
  }
  // request
  if (!hw_interface__srv__SetMotorGains_Request__Sequence__init(&msg->request, 0)) {
    hw_interface__srv__SetMotorGains_Event__fini(msg);
    return false;
  }
  // response
  if (!hw_interface__srv__SetMotorGains_Response__Sequence__init(&msg->response, 0)) {
    hw_interface__srv__SetMotorGains_Event__fini(msg);
    return false;
  }
  return true;
}

void
hw_interface__srv__SetMotorGains_Event__fini(hw_interface__srv__SetMotorGains_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  hw_interface__srv__SetMotorGains_Request__Sequence__fini(&msg->request);
  // response
  hw_interface__srv__SetMotorGains_Response__Sequence__fini(&msg->response);
}

bool
hw_interface__srv__SetMotorGains_Event__are_equal(const hw_interface__srv__SetMotorGains_Event * lhs, const hw_interface__srv__SetMotorGains_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!hw_interface__srv__SetMotorGains_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!hw_interface__srv__SetMotorGains_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Event__copy(
  const hw_interface__srv__SetMotorGains_Event * input,
  hw_interface__srv__SetMotorGains_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!hw_interface__srv__SetMotorGains_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!hw_interface__srv__SetMotorGains_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

hw_interface__srv__SetMotorGains_Event *
hw_interface__srv__SetMotorGains_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Event * msg = (hw_interface__srv__SetMotorGains_Event *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hw_interface__srv__SetMotorGains_Event));
  bool success = hw_interface__srv__SetMotorGains_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hw_interface__srv__SetMotorGains_Event__destroy(hw_interface__srv__SetMotorGains_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hw_interface__srv__SetMotorGains_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hw_interface__srv__SetMotorGains_Event__Sequence__init(hw_interface__srv__SetMotorGains_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Event)) {
      return false;
    }
    data = (hw_interface__srv__SetMotorGains_Event *)allocator.zero_allocate(size, sizeof(hw_interface__srv__SetMotorGains_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hw_interface__srv__SetMotorGains_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hw_interface__srv__SetMotorGains_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
hw_interface__srv__SetMotorGains_Event__Sequence__fini(hw_interface__srv__SetMotorGains_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      hw_interface__srv__SetMotorGains_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

hw_interface__srv__SetMotorGains_Event__Sequence *
hw_interface__srv__SetMotorGains_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__srv__SetMotorGains_Event__Sequence * array = (hw_interface__srv__SetMotorGains_Event__Sequence *)allocator.allocate(sizeof(hw_interface__srv__SetMotorGains_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hw_interface__srv__SetMotorGains_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hw_interface__srv__SetMotorGains_Event__Sequence__destroy(hw_interface__srv__SetMotorGains_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hw_interface__srv__SetMotorGains_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hw_interface__srv__SetMotorGains_Event__Sequence__are_equal(const hw_interface__srv__SetMotorGains_Event__Sequence * lhs, const hw_interface__srv__SetMotorGains_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hw_interface__srv__SetMotorGains_Event__Sequence__copy(
  const hw_interface__srv__SetMotorGains_Event__Sequence * input,
  hw_interface__srv__SetMotorGains_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(hw_interface__srv__SetMotorGains_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(hw_interface__srv__SetMotorGains_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hw_interface__srv__SetMotorGains_Event * data =
      (hw_interface__srv__SetMotorGains_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hw_interface__srv__SetMotorGains_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hw_interface__srv__SetMotorGains_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hw_interface__srv__SetMotorGains_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
