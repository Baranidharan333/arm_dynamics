// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice
#include "hw_interface/msg/detail/motor_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `arm_name`
// Member `error_name`
#include "rosidl_runtime_c/string_functions.h"

bool
hw_interface__msg__MotorStatus__init(hw_interface__msg__MotorStatus * msg)
{
  if (!msg) {
    return false;
  }
  // arm_name
  if (!rosidl_runtime_c__String__init(&msg->arm_name)) {
    hw_interface__msg__MotorStatus__fini(msg);
    return false;
  }
  // id
  // error
  // error_name
  if (!rosidl_runtime_c__String__init(&msg->error_name)) {
    hw_interface__msg__MotorStatus__fini(msg);
    return false;
  }
  // mos_temp
  // rotor_temp
  return true;
}

void
hw_interface__msg__MotorStatus__fini(hw_interface__msg__MotorStatus * msg)
{
  if (!msg) {
    return;
  }
  // arm_name
  rosidl_runtime_c__String__fini(&msg->arm_name);
  // id
  // error
  // error_name
  rosidl_runtime_c__String__fini(&msg->error_name);
  // mos_temp
  // rotor_temp
}

bool
hw_interface__msg__MotorStatus__are_equal(const hw_interface__msg__MotorStatus * lhs, const hw_interface__msg__MotorStatus * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // arm_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->arm_name), &(rhs->arm_name)))
  {
    return false;
  }
  // id
  if (lhs->id != rhs->id) {
    return false;
  }
  // error
  if (lhs->error != rhs->error) {
    return false;
  }
  // error_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->error_name), &(rhs->error_name)))
  {
    return false;
  }
  // mos_temp
  if (lhs->mos_temp != rhs->mos_temp) {
    return false;
  }
  // rotor_temp
  if (lhs->rotor_temp != rhs->rotor_temp) {
    return false;
  }
  return true;
}

bool
hw_interface__msg__MotorStatus__copy(
  const hw_interface__msg__MotorStatus * input,
  hw_interface__msg__MotorStatus * output)
{
  if (!input || !output) {
    return false;
  }
  // arm_name
  if (!rosidl_runtime_c__String__copy(
      &(input->arm_name), &(output->arm_name)))
  {
    return false;
  }
  // id
  output->id = input->id;
  // error
  output->error = input->error;
  // error_name
  if (!rosidl_runtime_c__String__copy(
      &(input->error_name), &(output->error_name)))
  {
    return false;
  }
  // mos_temp
  output->mos_temp = input->mos_temp;
  // rotor_temp
  output->rotor_temp = input->rotor_temp;
  return true;
}

hw_interface__msg__MotorStatus *
hw_interface__msg__MotorStatus__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatus * msg = (hw_interface__msg__MotorStatus *)allocator.allocate(sizeof(hw_interface__msg__MotorStatus), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hw_interface__msg__MotorStatus));
  bool success = hw_interface__msg__MotorStatus__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hw_interface__msg__MotorStatus__destroy(hw_interface__msg__MotorStatus * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hw_interface__msg__MotorStatus__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hw_interface__msg__MotorStatus__Sequence__init(hw_interface__msg__MotorStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatus * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(hw_interface__msg__MotorStatus)) {
      return false;
    }
    data = (hw_interface__msg__MotorStatus *)allocator.zero_allocate(size, sizeof(hw_interface__msg__MotorStatus), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hw_interface__msg__MotorStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hw_interface__msg__MotorStatus__fini(&data[i - 1]);
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
hw_interface__msg__MotorStatus__Sequence__fini(hw_interface__msg__MotorStatus__Sequence * array)
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
      hw_interface__msg__MotorStatus__fini(&array->data[i]);
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

hw_interface__msg__MotorStatus__Sequence *
hw_interface__msg__MotorStatus__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatus__Sequence * array = (hw_interface__msg__MotorStatus__Sequence *)allocator.allocate(sizeof(hw_interface__msg__MotorStatus__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hw_interface__msg__MotorStatus__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hw_interface__msg__MotorStatus__Sequence__destroy(hw_interface__msg__MotorStatus__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hw_interface__msg__MotorStatus__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hw_interface__msg__MotorStatus__Sequence__are_equal(const hw_interface__msg__MotorStatus__Sequence * lhs, const hw_interface__msg__MotorStatus__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hw_interface__msg__MotorStatus__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hw_interface__msg__MotorStatus__Sequence__copy(
  const hw_interface__msg__MotorStatus__Sequence * input,
  hw_interface__msg__MotorStatus__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(hw_interface__msg__MotorStatus)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(hw_interface__msg__MotorStatus);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hw_interface__msg__MotorStatus * data =
      (hw_interface__msg__MotorStatus *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hw_interface__msg__MotorStatus__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hw_interface__msg__MotorStatus__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hw_interface__msg__MotorStatus__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
