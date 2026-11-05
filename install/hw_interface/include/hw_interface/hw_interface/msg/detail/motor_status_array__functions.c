// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice
#include "hw_interface/msg/detail/motor_status_array__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `motors`
#include "hw_interface/msg/detail/motor_status__functions.h"

bool
hw_interface__msg__MotorStatusArray__init(hw_interface__msg__MotorStatusArray * msg)
{
  if (!msg) {
    return false;
  }
  // motors
  if (!hw_interface__msg__MotorStatus__Sequence__init(&msg->motors, 0)) {
    hw_interface__msg__MotorStatusArray__fini(msg);
    return false;
  }
  return true;
}

void
hw_interface__msg__MotorStatusArray__fini(hw_interface__msg__MotorStatusArray * msg)
{
  if (!msg) {
    return;
  }
  // motors
  hw_interface__msg__MotorStatus__Sequence__fini(&msg->motors);
}

bool
hw_interface__msg__MotorStatusArray__are_equal(const hw_interface__msg__MotorStatusArray * lhs, const hw_interface__msg__MotorStatusArray * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // motors
  if (!hw_interface__msg__MotorStatus__Sequence__are_equal(
      &(lhs->motors), &(rhs->motors)))
  {
    return false;
  }
  return true;
}

bool
hw_interface__msg__MotorStatusArray__copy(
  const hw_interface__msg__MotorStatusArray * input,
  hw_interface__msg__MotorStatusArray * output)
{
  if (!input || !output) {
    return false;
  }
  // motors
  if (!hw_interface__msg__MotorStatus__Sequence__copy(
      &(input->motors), &(output->motors)))
  {
    return false;
  }
  return true;
}

hw_interface__msg__MotorStatusArray *
hw_interface__msg__MotorStatusArray__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatusArray * msg = (hw_interface__msg__MotorStatusArray *)allocator.allocate(sizeof(hw_interface__msg__MotorStatusArray), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hw_interface__msg__MotorStatusArray));
  bool success = hw_interface__msg__MotorStatusArray__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hw_interface__msg__MotorStatusArray__destroy(hw_interface__msg__MotorStatusArray * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hw_interface__msg__MotorStatusArray__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hw_interface__msg__MotorStatusArray__Sequence__init(hw_interface__msg__MotorStatusArray__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatusArray * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(hw_interface__msg__MotorStatusArray)) {
      return false;
    }
    data = (hw_interface__msg__MotorStatusArray *)allocator.zero_allocate(size, sizeof(hw_interface__msg__MotorStatusArray), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hw_interface__msg__MotorStatusArray__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hw_interface__msg__MotorStatusArray__fini(&data[i - 1]);
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
hw_interface__msg__MotorStatusArray__Sequence__fini(hw_interface__msg__MotorStatusArray__Sequence * array)
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
      hw_interface__msg__MotorStatusArray__fini(&array->data[i]);
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

hw_interface__msg__MotorStatusArray__Sequence *
hw_interface__msg__MotorStatusArray__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hw_interface__msg__MotorStatusArray__Sequence * array = (hw_interface__msg__MotorStatusArray__Sequence *)allocator.allocate(sizeof(hw_interface__msg__MotorStatusArray__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hw_interface__msg__MotorStatusArray__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hw_interface__msg__MotorStatusArray__Sequence__destroy(hw_interface__msg__MotorStatusArray__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hw_interface__msg__MotorStatusArray__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hw_interface__msg__MotorStatusArray__Sequence__are_equal(const hw_interface__msg__MotorStatusArray__Sequence * lhs, const hw_interface__msg__MotorStatusArray__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hw_interface__msg__MotorStatusArray__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hw_interface__msg__MotorStatusArray__Sequence__copy(
  const hw_interface__msg__MotorStatusArray__Sequence * input,
  hw_interface__msg__MotorStatusArray__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(hw_interface__msg__MotorStatusArray)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(hw_interface__msg__MotorStatusArray);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hw_interface__msg__MotorStatusArray * data =
      (hw_interface__msg__MotorStatusArray *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hw_interface__msg__MotorStatusArray__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hw_interface__msg__MotorStatusArray__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hw_interface__msg__MotorStatusArray__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
