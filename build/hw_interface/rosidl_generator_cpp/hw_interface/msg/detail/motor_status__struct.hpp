// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/msg/motor_status.hpp"


#ifndef HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_HPP_
#define HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__hw_interface__msg__MotorStatus __attribute__((deprecated))
#else
# define DEPRECATED__hw_interface__msg__MotorStatus __declspec(deprecated)
#endif

namespace hw_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct MotorStatus_
{
  using Type = MotorStatus_<ContainerAllocator>;

  explicit MotorStatus_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->arm_name = "";
      this->id = 0l;
      this->error = 0l;
      this->error_name = "";
      this->mos_temp = 0.0f;
      this->rotor_temp = 0.0f;
    }
  }

  explicit MotorStatus_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : arm_name(_alloc),
    error_name(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->arm_name = "";
      this->id = 0l;
      this->error = 0l;
      this->error_name = "";
      this->mos_temp = 0.0f;
      this->rotor_temp = 0.0f;
    }
  }

  // field types and members
  using _arm_name_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _arm_name_type arm_name;
  using _id_type =
    int32_t;
  _id_type id;
  using _error_type =
    int32_t;
  _error_type error;
  using _error_name_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _error_name_type error_name;
  using _mos_temp_type =
    float;
  _mos_temp_type mos_temp;
  using _rotor_temp_type =
    float;
  _rotor_temp_type rotor_temp;

  // setters for named parameter idiom
  Type & set__arm_name(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->arm_name = _arg;
    return *this;
  }
  Type & set__id(
    const int32_t & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__error(
    const int32_t & _arg)
  {
    this->error = _arg;
    return *this;
  }
  Type & set__error_name(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->error_name = _arg;
    return *this;
  }
  Type & set__mos_temp(
    const float & _arg)
  {
    this->mos_temp = _arg;
    return *this;
  }
  Type & set__rotor_temp(
    const float & _arg)
  {
    this->rotor_temp = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    hw_interface::msg::MotorStatus_<ContainerAllocator> *;
  using ConstRawPtr =
    const hw_interface::msg::MotorStatus_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      hw_interface::msg::MotorStatus_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      hw_interface::msg::MotorStatus_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__hw_interface__msg__MotorStatus
    std::shared_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__hw_interface__msg__MotorStatus
    std::shared_ptr<hw_interface::msg::MotorStatus_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MotorStatus_ & other) const
  {
    if (this->arm_name != other.arm_name) {
      return false;
    }
    if (this->id != other.id) {
      return false;
    }
    if (this->error != other.error) {
      return false;
    }
    if (this->error_name != other.error_name) {
      return false;
    }
    if (this->mos_temp != other.mos_temp) {
      return false;
    }
    if (this->rotor_temp != other.rotor_temp) {
      return false;
    }
    return true;
  }
  bool operator!=(const MotorStatus_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MotorStatus_

// alias to use template instance with default allocator
using MotorStatus =
  hw_interface::msg::MotorStatus_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace hw_interface

#endif  // HW_INTERFACE__MSG__DETAIL__MOTOR_STATUS__STRUCT_HPP_
