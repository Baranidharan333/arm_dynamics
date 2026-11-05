#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "hw_interface__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__hw_interface__srv__SetMotorGains_Request() -> *const std::ffi::c_void;
}

#[link(name = "hw_interface__rosidl_generator_c")]
extern "C" {
    fn hw_interface__srv__SetMotorGains_Request__init(msg: *mut SetMotorGains_Request) -> bool;
    fn hw_interface__srv__SetMotorGains_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Request>, size: usize) -> bool;
    fn hw_interface__srv__SetMotorGains_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Request>);
    fn hw_interface__srv__SetMotorGains_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetMotorGains_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Request>) -> bool;
}

// Corresponds to hw_interface__srv__SetMotorGains_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetMotorGains_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub motor_ids: rosidl_runtime_rs::Sequence<i32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub kp: rosidl_runtime_rs::Sequence<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub kd: rosidl_runtime_rs::Sequence<f32>,

}



impl Default for SetMotorGains_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !hw_interface__srv__SetMotorGains_Request__init(&mut msg as *mut _) {
        panic!("Call to hw_interface__srv__SetMotorGains_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetMotorGains_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetMotorGains_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetMotorGains_Request where Self: Sized {
  const TYPE_NAME: &'static str = "hw_interface/srv/SetMotorGains_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__hw_interface__srv__SetMotorGains_Request() }
  }
}


#[link(name = "hw_interface__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__hw_interface__srv__SetMotorGains_Response() -> *const std::ffi::c_void;
}

#[link(name = "hw_interface__rosidl_generator_c")]
extern "C" {
    fn hw_interface__srv__SetMotorGains_Response__init(msg: *mut SetMotorGains_Response) -> bool;
    fn hw_interface__srv__SetMotorGains_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Response>, size: usize) -> bool;
    fn hw_interface__srv__SetMotorGains_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Response>);
    fn hw_interface__srv__SetMotorGains_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetMotorGains_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetMotorGains_Response>) -> bool;
}

// Corresponds to hw_interface__srv__SetMotorGains_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetMotorGains_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,

}



impl Default for SetMotorGains_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !hw_interface__srv__SetMotorGains_Response__init(&mut msg as *mut _) {
        panic!("Call to hw_interface__srv__SetMotorGains_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetMotorGains_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hw_interface__srv__SetMotorGains_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetMotorGains_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetMotorGains_Response where Self: Sized {
  const TYPE_NAME: &'static str = "hw_interface/srv/SetMotorGains_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__hw_interface__srv__SetMotorGains_Response() }
  }
}






#[link(name = "hw_interface__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__hw_interface__srv__SetMotorGains() -> *const std::ffi::c_void;
}

// Corresponds to hw_interface__srv__SetMotorGains
#[allow(missing_docs, non_camel_case_types)]
pub struct SetMotorGains;

impl rosidl_runtime_rs::Service for SetMotorGains {
    type Request = SetMotorGains_Request;
    type Response = SetMotorGains_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__hw_interface__srv__SetMotorGains() }
    }
}


