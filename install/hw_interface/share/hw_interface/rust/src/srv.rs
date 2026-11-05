#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to hw_interface__srv__SetMotorGains_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetMotorGains_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub motor_ids: Vec<i32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub kp: Vec<f32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub kd: Vec<f32>,

}



impl Default for SetMotorGains_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetMotorGains_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetMotorGains_Request {
  type RmwMsg = super::srv::rmw::SetMotorGains_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        motor_ids: msg.motor_ids.as_slice().into(),
        kp: msg.kp.as_slice().into(),
        kd: msg.kd.as_slice().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        motor_ids: msg.motor_ids.as_slice().into(),
        kp: msg.kp.as_slice().into(),
        kd: msg.kd.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      motor_ids: msg.motor_ids.into(),
      kp: msg.kp.into(),
      kd: msg.kd.into(),
    }
  }
}


// Corresponds to hw_interface__srv__SetMotorGains_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetMotorGains_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: std::string::String,

}



impl Default for SetMotorGains_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetMotorGains_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetMotorGains_Response {
  type RmwMsg = super::srv::rmw::SetMotorGains_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
        message: msg.message.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
        message: msg.message.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
      message: msg.message.to_string(),
    }
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


