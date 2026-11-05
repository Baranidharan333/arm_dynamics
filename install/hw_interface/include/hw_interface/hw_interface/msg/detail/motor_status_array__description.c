// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from hw_interface:msg/MotorStatusArray.idl
// generated code does not contain a copyright notice

#include "hw_interface/msg/detail/motor_status_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_hw_interface
const rosidl_type_hash_t *
hw_interface__msg__MotorStatusArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x81, 0x9b, 0x71, 0x1a, 0x97, 0x1e, 0xc8, 0x80,
      0x0e, 0xd1, 0x77, 0xf6, 0x34, 0x60, 0xd6, 0xae,
      0x73, 0xdb, 0xfe, 0xbe, 0x2e, 0xbe, 0x84, 0xe4,
      0x93, 0x93, 0x15, 0x40, 0xfe, 0x64, 0x5c, 0xae,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "hw_interface/msg/detail/motor_status__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t hw_interface__msg__MotorStatus__EXPECTED_HASH = {1, {
    0x17, 0x3e, 0xcc, 0x66, 0x2b, 0x50, 0x41, 0x8b,
    0x72, 0x02, 0x77, 0x1d, 0x73, 0x64, 0xc4, 0x6c,
    0xc7, 0x11, 0x0b, 0x0c, 0x85, 0xb0, 0x31, 0x3d,
    0x04, 0xfa, 0x44, 0xf7, 0x35, 0xaa, 0x35, 0xeb,
  }};
#endif

static char hw_interface__msg__MotorStatusArray__TYPE_NAME[] = "hw_interface/msg/MotorStatusArray";
static char hw_interface__msg__MotorStatus__TYPE_NAME[] = "hw_interface/msg/MotorStatus";

// Define type names, field names, and default values
static char hw_interface__msg__MotorStatusArray__FIELD_NAME__motors[] = "motors";

static rosidl_runtime_c__type_description__Field hw_interface__msg__MotorStatusArray__FIELDS[] = {
  {
    {hw_interface__msg__MotorStatusArray__FIELD_NAME__motors, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {hw_interface__msg__MotorStatus__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription hw_interface__msg__MotorStatusArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {hw_interface__msg__MotorStatus__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
hw_interface__msg__MotorStatusArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {hw_interface__msg__MotorStatusArray__TYPE_NAME, 33, 33},
      {hw_interface__msg__MotorStatusArray__FIELDS, 1, 1},
    },
    {hw_interface__msg__MotorStatusArray__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&hw_interface__msg__MotorStatus__EXPECTED_HASH, hw_interface__msg__MotorStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = hw_interface__msg__MotorStatus__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "MotorStatus[] motors";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
hw_interface__msg__MotorStatusArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {hw_interface__msg__MotorStatusArray__TYPE_NAME, 33, 33},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 21, 21},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
hw_interface__msg__MotorStatusArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *hw_interface__msg__MotorStatusArray__get_individual_type_description_source(NULL),
    sources[1] = *hw_interface__msg__MotorStatus__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
