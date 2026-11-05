// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status_array.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'motors'
#include "hw_interface/msg/detail/motor_status__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__hw_interface__msg__MotorStatusArray __attribute__((deprecated))
#else
# define DEPRECATED__hw_interface__msg__MotorStatusArray __declspec(deprecated)
#endif

namespace hw_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct MotorStatusArray_
{
  using Type = MotorStatusArray_<ContainerAllocator>;

  explicit MotorStatusArray_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit MotorStatusArray_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _motors_type =
    std::vector<hw_interface::msg::MotorStatus_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<hw_interface::msg::MotorStatus_<ContainerAllocator>>>;
  _motors_type motors;

  // setters for named parameter idiom
  Type & set__motors(
    const std::vector<hw_interface::msg::MotorStatus_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<hw_interface::msg::MotorStatus_<ContainerAllocator>>> & _arg)
  {
    this->motors = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    hw_interface::msg::MotorStatusArray_<ContainerAllocator> *;
  using ConstRawPtr =
    const hw_interface::msg::MotorStatusArray_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      hw_interface::msg::MotorStatusArray_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      hw_interface::msg::MotorStatusArray_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__hw_interface__msg__MotorStatusArray
    std::shared_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__hw_interface__msg__MotorStatusArray
    std::shared_ptr<hw_interface::msg::MotorStatusArray_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MotorStatusArray_ & other) const
  {
    if (this->motors != other.motors) {
      return false;
    }
    return true;
  }
  bool operator!=(const MotorStatusArray_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MotorStatusArray_

// alias to use template instance with default allocator
using MotorStatusArray =
  hw_interface::msg::MotorStatusArray_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace hw_interface

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS_ARRAY__STRUCT_HPP_
