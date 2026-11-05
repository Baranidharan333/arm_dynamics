// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "hw_interface/srv/set_motor_gains.hpp"


#ifndef HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__TRAITS_HPP_
#define HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace hw_interface
{

namespace srv
{

inline void to_flow_style_yaml(
  const SetMotorGains_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: motor_ids
  {
    if (msg.motor_ids.size() == 0) {
      out << "motor_ids: []";
    } else {
      out << "motor_ids: [";
      size_t pending_items = msg.motor_ids.size();
      for (auto item : msg.motor_ids) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: kp
  {
    if (msg.kp.size() == 0) {
      out << "kp: []";
    } else {
      out << "kp: [";
      size_t pending_items = msg.kp.size();
      for (auto item : msg.kp) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: kd
  {
    if (msg.kd.size() == 0) {
      out << "kd: []";
    } else {
      out << "kd: [";
      size_t pending_items = msg.kd.size();
      for (auto item : msg.kd) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SetMotorGains_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: motor_ids
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motor_ids.size() == 0) {
      out << "motor_ids: []\n";
    } else {
      out << "motor_ids:\n";
      for (auto item : msg.motor_ids) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: kp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.kp.size() == 0) {
      out << "kp: []\n";
    } else {
      out << "kp:\n";
      for (auto item : msg.kp) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: kd
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.kd.size() == 0) {
      out << "kd: []\n";
    } else {
      out << "kd:\n";
      for (auto item : msg.kd) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SetMotorGains_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_generator_traits
{

[[deprecated("use hw_interface::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const hw_interface::srv::SetMotorGains_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  hw_interface::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hw_interface::srv::to_yaml() instead")]]
inline std::string to_yaml(const hw_interface::srv::SetMotorGains_Request & msg)
{
  return hw_interface::srv::to_yaml(msg);
}

template<>
inline const char * data_type<hw_interface::srv::SetMotorGains_Request>()
{
  return "hw_interface::srv::SetMotorGains_Request";
}

template<>
inline const char * name<hw_interface::srv::SetMotorGains_Request>()
{
  return "hw_interface/srv/SetMotorGains_Request";
}

template<>
struct has_fixed_size<hw_interface::srv::SetMotorGains_Request>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<hw_interface::srv::SetMotorGains_Request>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<hw_interface::srv::SetMotorGains_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace hw_interface
{

namespace srv
{

inline void to_flow_style_yaml(
  const SetMotorGains_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SetMotorGains_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SetMotorGains_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_generator_traits
{

[[deprecated("use hw_interface::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const hw_interface::srv::SetMotorGains_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  hw_interface::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hw_interface::srv::to_yaml() instead")]]
inline std::string to_yaml(const hw_interface::srv::SetMotorGains_Response & msg)
{
  return hw_interface::srv::to_yaml(msg);
}

template<>
inline const char * data_type<hw_interface::srv::SetMotorGains_Response>()
{
  return "hw_interface::srv::SetMotorGains_Response";
}

template<>
inline const char * name<hw_interface::srv::SetMotorGains_Response>()
{
  return "hw_interface/srv/SetMotorGains_Response";
}

template<>
struct has_fixed_size<hw_interface::srv::SetMotorGains_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<hw_interface::srv::SetMotorGains_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<hw_interface::srv::SetMotorGains_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace hw_interface
{

namespace srv
{

inline void to_flow_style_yaml(
  const SetMotorGains_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SetMotorGains_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SetMotorGains_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_generator_traits
{

[[deprecated("use hw_interface::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const hw_interface::srv::SetMotorGains_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  hw_interface::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hw_interface::srv::to_yaml() instead")]]
inline std::string to_yaml(const hw_interface::srv::SetMotorGains_Event & msg)
{
  return hw_interface::srv::to_yaml(msg);
}

template<>
inline const char * data_type<hw_interface::srv::SetMotorGains_Event>()
{
  return "hw_interface::srv::SetMotorGains_Event";
}

template<>
inline const char * name<hw_interface::srv::SetMotorGains_Event>()
{
  return "hw_interface/srv/SetMotorGains_Event";
}

template<>
struct has_fixed_size<hw_interface::srv::SetMotorGains_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<hw_interface::srv::SetMotorGains_Event>
  : std::integral_constant<bool, has_bounded_size<hw_interface::srv::SetMotorGains_Request>::value && has_bounded_size<hw_interface::srv::SetMotorGains_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<hw_interface::srv::SetMotorGains_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<hw_interface::srv::SetMotorGains>()
{
  return "hw_interface::srv::SetMotorGains";
}

template<>
inline const char * name<hw_interface::srv::SetMotorGains>()
{
  return "hw_interface/srv/SetMotorGains";
}

template<>
struct has_fixed_size<hw_interface::srv::SetMotorGains>
  : std::integral_constant<
    bool,
    has_fixed_size<hw_interface::srv::SetMotorGains_Request>::value &&
    has_fixed_size<hw_interface::srv::SetMotorGains_Response>::value
  >
{
};

template<>
struct has_bounded_size<hw_interface::srv::SetMotorGains>
  : std::integral_constant<
    bool,
    has_bounded_size<hw_interface::srv::SetMotorGains_Request>::value &&
    has_bounded_size<hw_interface::srv::SetMotorGains_Response>::value
  >
{
};

template<>
struct is_service<hw_interface::srv::SetMotorGains>
  : std::true_type
{
};

template<>
struct is_service_request<hw_interface::srv::SetMotorGains_Request>
  : std::true_type
{
};

template<>
struct is_service_response<hw_interface::srv::SetMotorGains_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // HW_INTERFACE__SRV__DETAIL__SET_MOTOR_GAINS__TRAITS_HPP_
