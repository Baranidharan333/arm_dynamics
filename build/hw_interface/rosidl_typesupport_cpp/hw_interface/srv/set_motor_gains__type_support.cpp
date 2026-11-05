// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from hw_interface:srv/SetMotorGains.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "hw_interface/srv/detail/set_motor_gains__functions.h"
#include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetMotorGains_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetMotorGains_Request_type_support_ids_t;

static const _SetMotorGains_Request_type_support_ids_t _SetMotorGains_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetMotorGains_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetMotorGains_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetMotorGains_Request_type_support_symbol_names_t _SetMotorGains_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, hw_interface, srv, SetMotorGains_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Request)),
  }
};

typedef struct _SetMotorGains_Request_type_support_data_t
{
  void * data[2];
} _SetMotorGains_Request_type_support_data_t;

static _SetMotorGains_Request_type_support_data_t _SetMotorGains_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetMotorGains_Request_message_typesupport_map = {
  2,
  "hw_interface",
  &_SetMotorGains_Request_message_typesupport_ids.typesupport_identifier[0],
  &_SetMotorGains_Request_message_typesupport_symbol_names.symbol_name[0],
  &_SetMotorGains_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetMotorGains_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetMotorGains_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Request__get_type_hash,
  &hw_interface__srv__SetMotorGains_Request__get_type_description,
  &hw_interface__srv__SetMotorGains_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>()
{
  return &::hw_interface::srv::rosidl_typesupport_cpp::SetMotorGains_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, hw_interface, srv, SetMotorGains_Request)() {
  return get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetMotorGains_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetMotorGains_Response_type_support_ids_t;

static const _SetMotorGains_Response_type_support_ids_t _SetMotorGains_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetMotorGains_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetMotorGains_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetMotorGains_Response_type_support_symbol_names_t _SetMotorGains_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, hw_interface, srv, SetMotorGains_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Response)),
  }
};

typedef struct _SetMotorGains_Response_type_support_data_t
{
  void * data[2];
} _SetMotorGains_Response_type_support_data_t;

static _SetMotorGains_Response_type_support_data_t _SetMotorGains_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetMotorGains_Response_message_typesupport_map = {
  2,
  "hw_interface",
  &_SetMotorGains_Response_message_typesupport_ids.typesupport_identifier[0],
  &_SetMotorGains_Response_message_typesupport_symbol_names.symbol_name[0],
  &_SetMotorGains_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetMotorGains_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetMotorGains_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Response__get_type_hash,
  &hw_interface__srv__SetMotorGains_Response__get_type_description,
  &hw_interface__srv__SetMotorGains_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>()
{
  return &::hw_interface::srv::rosidl_typesupport_cpp::SetMotorGains_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, hw_interface, srv, SetMotorGains_Response)() {
  return get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__functions.h"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetMotorGains_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetMotorGains_Event_type_support_ids_t;

static const _SetMotorGains_Event_type_support_ids_t _SetMotorGains_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetMotorGains_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetMotorGains_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetMotorGains_Event_type_support_symbol_names_t _SetMotorGains_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, hw_interface, srv, SetMotorGains_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains_Event)),
  }
};

typedef struct _SetMotorGains_Event_type_support_data_t
{
  void * data[2];
} _SetMotorGains_Event_type_support_data_t;

static _SetMotorGains_Event_type_support_data_t _SetMotorGains_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetMotorGains_Event_message_typesupport_map = {
  2,
  "hw_interface",
  &_SetMotorGains_Event_message_typesupport_ids.typesupport_identifier[0],
  &_SetMotorGains_Event_message_typesupport_symbol_names.symbol_name[0],
  &_SetMotorGains_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetMotorGains_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetMotorGains_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &hw_interface__srv__SetMotorGains_Event__get_type_hash,
  &hw_interface__srv__SetMotorGains_Event__get_type_description,
  &hw_interface__srv__SetMotorGains_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<hw_interface::srv::SetMotorGains_Event>()
{
  return &::hw_interface::srv::rosidl_typesupport_cpp::SetMotorGains_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, hw_interface, srv, SetMotorGains_Event)() {
  return get_message_type_support_handle<hw_interface::srv::SetMotorGains_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "hw_interface/srv/detail/set_motor_gains__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace hw_interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetMotorGains_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetMotorGains_type_support_ids_t;

static const _SetMotorGains_type_support_ids_t _SetMotorGains_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetMotorGains_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetMotorGains_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetMotorGains_type_support_symbol_names_t _SetMotorGains_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, hw_interface, srv, SetMotorGains)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, hw_interface, srv, SetMotorGains)),
  }
};

typedef struct _SetMotorGains_type_support_data_t
{
  void * data[2];
} _SetMotorGains_type_support_data_t;

static _SetMotorGains_type_support_data_t _SetMotorGains_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetMotorGains_service_typesupport_map = {
  2,
  "hw_interface",
  &_SetMotorGains_service_typesupport_ids.typesupport_identifier[0],
  &_SetMotorGains_service_typesupport_symbol_names.symbol_name[0],
  &_SetMotorGains_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t SetMotorGains_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetMotorGains_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<hw_interface::srv::SetMotorGains_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<hw_interface::srv::SetMotorGains>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<hw_interface::srv::SetMotorGains>,
  &hw_interface__srv__SetMotorGains__get_type_hash,
  &hw_interface__srv__SetMotorGains__get_type_description,
  &hw_interface__srv__SetMotorGains__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace hw_interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<hw_interface::srv::SetMotorGains>()
{
  return &::hw_interface::srv::rosidl_typesupport_cpp::SetMotorGains_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, hw_interface, srv, SetMotorGains)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<hw_interface::srv::SetMotorGains>();
}

#ifdef __cplusplus
}
#endif
