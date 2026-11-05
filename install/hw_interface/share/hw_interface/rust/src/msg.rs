#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to hw_interface__msg__MotorStatus

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MotorStatus {

    // This member is not documented.
    #[allow(missing_docs)]
    pub arm_name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub error: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub error_name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub mos_temp: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub rotor_temp: f32,

}



impl Default for MotorStatus {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::MotorStatus::default())
  }
}

impl rosidl_runtime_rs::Message for MotorStatus {
  type RmwMsg = super::msg::rmw::MotorStatus;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        arm_name: msg.arm_name.as_str().into(),
        id: msg.id,
        error: msg.error,
        error_name: msg.error_name.as_str().into(),
        mos_temp: msg.mos_temp,
        rotor_temp: msg.rotor_temp,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        arm_name: msg.arm_name.as_str().into(),
      id: msg.id,
      error: msg.error,
        error_name: msg.error_name.as_str().into(),
      mos_temp: msg.mos_temp,
      rotor_temp: msg.rotor_temp,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      arm_name: msg.arm_name.to_string(),
      id: msg.id,
      error: msg.error,
      error_name: msg.error_name.to_string(),
      mos_temp: msg.mos_temp,
      rotor_temp: msg.rotor_temp,
    }
  }
}


// Corresponds to hw_interface__msg__MotorStatusArray

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MotorStatusArray {

    // This member is not documented.
    #[allow(missing_docs)]
    pub motors: Vec<super::msg::MotorStatus>,

}



impl Default for MotorStatusArray {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::MotorStatusArray::default())
  }
}

impl rosidl_runtime_rs::Message for MotorStatusArray {
  type RmwMsg = super::msg::rmw::MotorStatusArray;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        motors: msg.motors
          .into_iter()
          .map(|elem| super::msg::MotorStatus::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        motors: msg.motors
          .iter()
          .map(|elem| super::msg::MotorStatus::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      motors: msg.motors
          .into_iter()
          .map(super::msg::MotorStatus::from_rmw_message)
          .collect(),
    }
  }
}


