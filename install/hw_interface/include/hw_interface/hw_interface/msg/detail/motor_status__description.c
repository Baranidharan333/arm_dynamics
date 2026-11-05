// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from hw_interface:msg/MotorStatus.idl
// generated code does not contain a copyright notice

#include "hw_interface/msg/detail/motor_status__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_hw_interface
const rosidl_type_hash_t *
hw_interface__msg__MotorStatus__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x17, 0x3e, 0xcc, 0x66, 0x2b, 0x50, 0x41, 0x8b,
      0x72, 0x02, 0x77, 0x1d, 0x73, 0x64, 0xc4, 0x6c,
      0xc7, 0x11, 0x0b, 0x0c, 0x85, 0xb0, 0x31, 0x3d,
      0x04, 0xfa, 0x44, 0xf7, 0x35, 0xaa, 0x35, 0xeb,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char hw_interface__msg__MotorStatus__TYPE_NAME[] = "hw_interface/msg/MotorStatus";

// Define type names, field names, and default values
static char hw_interface__msg__MotorStatus__FIELD_NAME__arm_name[] = "arm_name";
static char hw_interface__msg__MotorStatus__FIELD_NAME__id[] = "id";
static char hw_interface__msg__MotorStatus__FIELD_NAME__error[] = "error";
static char hw_interface__msg__MotorStatus__FIELD_NAME__error_name[] = "error_name";
static char hw_interface__msg__MotorStatus__FIELD_NAME__mos_temp[] = "mos_temp";
static char hw_interface__msg__MotorStatus__FIELD_NAME__rotor_temp[] = "rotor_temp";

static rosidl_runtime_c__type_description__Field hw_interface__msg__MotorStatus__FIELDS[] = {
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__arm_name, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__error, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__error_name, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__mos_temp, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {hw_interface__msg__MotorStatus__FIELD_NAME__rotor_temp, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
hw_interface__msg__MotorStatus__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {hw_interface__msg__MotorStatus__TYPE_NAME, 28, 28},
      {hw_interface__msg__MotorStatus__FIELDS, 6, 6},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string arm_name\n"
  "int32 id\n"
  "int32 error\n"
  "string error_name\n"
  "float32 mos_temp\n"
  "float32 rotor_temp";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
hw_interface__msg__MotorStatus__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {hw_interface__msg__MotorStatus__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 90, 90},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
hw_interface__msg__MotorStatus__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *hw_interface__msg__MotorStatus__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
