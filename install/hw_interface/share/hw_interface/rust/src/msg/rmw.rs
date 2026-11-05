#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "hw_interface__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__hw_interface__msg__MotorStatus() -> *const std::ffi::c_void;
}

#[link(name = "hw_interface__rosidl_generator_c")]
extern "C" {
    fn hw_interface__msg__MotorStatus__init(msg: *mut MotorStatus) -> bool;
    fn hw_interface__msg__MotorStatus__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MotorStatus>, size: usize) -> bool;
    fn hw_interface__msg__MotorStatus__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MotorStatus>);
    fn hw_interface__msg__MotorStatus__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MotorStatus>, out_seq: *mut rosidl_runtime_rs::Sequence<MotorStatus>) -> bool;
}

// Corresponds to hw_interface__msg__MotorStatus
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MotorStatus {

    // This member is not documented.
    #[allow(missing_docs)]
    pub arm_name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub error: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub error_name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub mos_temp: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub rotor_temp: f32,

}



impl Default for MotorStatus {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !hw_interface__msg__MotorStatus__init(&mut msg as *mut _) {
        panic!("Call to hw_interface__msg__MotorStatus__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MotorStatus {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatus__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatus__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatus__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MotorStatus {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MotorStatus where Self: Sized {
  const TYPE_NAME: &'static str = "hw_interface/msg/MotorStatus";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__hw_interface__msg__MotorStatus() }
  }
}


#[link(name = "hw_interface__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__hw_interface__msg__MotorStatusArray() -> *const std::ffi::c_void;
}

#[link(name = "hw_interface__rosidl_generator_c")]
extern "C" {
    fn hw_interface__msg__MotorStatusArray__init(msg: *mut MotorStatusArray) -> bool;
    fn hw_interface__msg__MotorStatusArray__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MotorStatusArray>, size: usize) -> bool;
    fn hw_interface__msg__MotorStatusArray__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MotorStatusArray>);
    fn hw_interface__msg__MotorStatusArray__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MotorStatusArray>, out_seq: *mut rosidl_runtime_rs::Sequence<MotorStatusArray>) -> bool;
}

// Corresponds to hw_interface__msg__MotorStatusArray
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MotorStatusArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub motors: rosidl_runtime_rs::Sequence<super::super::msg::rmw::MotorStatus>,

}



impl Default for MotorStatusArray {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !hw_interface__msg__MotorStatusArray__init(&mut msg as *mut _) {
        panic!("Call to hw_interface__msg__MotorStatusArray__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MotorStatusArray {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatusArray__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatusArray__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__msg__MotorStatusArray__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MotorStatusArray {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MotorStatusArray where Self: Sized {
  const TYPE_NAME: &'static str = "hw_interface/msg/MotorStatusArray";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__hw_interface__msg__MotorStatusArray() }
  }
}


