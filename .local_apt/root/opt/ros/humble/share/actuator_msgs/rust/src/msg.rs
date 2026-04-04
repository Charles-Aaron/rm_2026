#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to actuator_msgs__msg__Actuators

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Actuators {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// This message defines lowest level commands to be sent to the actuator(s).
    /// Position of the actuators in [rad] for angular actuators
    /// and [m] for linear actuators.
    pub position: Vec<f64>,

    /// Velocities of the actuators in [rad/s] for angular actuators
    /// and [m/s] for linear actuators.
    pub velocity: Vec<f64>,

    /// Everything that does not fit the above,
    /// normalized between.
    pub normalized: Vec<f64>,

}



impl Default for Actuators {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Actuators::default())
  }
}

impl rosidl_runtime_rs::Message for Actuators {
  type RmwMsg = super::msg::rmw::Actuators;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        position: msg.position.into(),
        velocity: msg.velocity.into(),
        normalized: msg.normalized.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        position: msg.position.as_slice().into(),
        velocity: msg.velocity.as_slice().into(),
        normalized: msg.normalized.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      position: msg.position
          .into_iter()
          .collect(),
      velocity: msg.velocity
          .into_iter()
          .collect(),
      normalized: msg.normalized
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsAngularPosition
/// This message defines lowest level commands to be sent
/// to the actuator(s) for angular position.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsAngularPosition {
    /// Position of the actuators in
    pub position: Vec<f64>,

}



impl Default for ActuatorsAngularPosition {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsAngularPosition::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsAngularPosition {
  type RmwMsg = super::msg::rmw::ActuatorsAngularPosition;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: msg.position.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: msg.position.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: msg.position
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsAngularVelocity
/// This message defines lowest level commands to be sent
/// to the actuator(s) for angular velocity.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsAngularVelocity {
    /// Velocity of the actuators in
    pub velocity: Vec<f64>,

}



impl Default for ActuatorsAngularVelocity {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsAngularVelocity::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsAngularVelocity {
  type RmwMsg = super::msg::rmw::ActuatorsAngularVelocity;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        velocity: msg.velocity.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        velocity: msg.velocity.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      velocity: msg.velocity
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsLinearPosition
/// This message defines lowest level commands to be sent
/// to the actuator(s) for linear position.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsLinearPosition {
    /// Position of the actuators in
    pub position: Vec<f64>,

}



impl Default for ActuatorsLinearPosition {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsLinearPosition::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsLinearPosition {
  type RmwMsg = super::msg::rmw::ActuatorsLinearPosition;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: msg.position.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        position: msg.position.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      position: msg.position
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsLinearVelocity
/// This message defines lowest level commands to be sent
/// to the actuator(s) for linear velocity.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsLinearVelocity {
    /// Velocity of the actuators in
    pub velocity: Vec<f64>,

}



impl Default for ActuatorsLinearVelocity {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsLinearVelocity::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsLinearVelocity {
  type RmwMsg = super::msg::rmw::ActuatorsLinearVelocity;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        velocity: msg.velocity.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        velocity: msg.velocity.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      velocity: msg.velocity
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsNormalized

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsNormalized {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for normalized outputs.
    /// Normalized between.
    pub normalized: Vec<f64>,

}



impl Default for ActuatorsNormalized {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsNormalized::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsNormalized {
  type RmwMsg = super::msg::rmw::ActuatorsNormalized;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        normalized: msg.normalized.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        normalized: msg.normalized.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      normalized: msg.normalized
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsPosition

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsPosition {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for position.
    /// Position of the actuators in
    pub angular: super::msg::ActuatorsAngularPosition,

    /// Position of the actuators in
    pub linear: super::msg::ActuatorsLinearPosition,

}



impl Default for ActuatorsPosition {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsPosition::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsPosition {
  type RmwMsg = super::msg::rmw::ActuatorsPosition;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        angular: super::msg::ActuatorsAngularPosition::into_rmw_message(std::borrow::Cow::Owned(msg.angular)).into_owned(),
        linear: super::msg::ActuatorsLinearPosition::into_rmw_message(std::borrow::Cow::Owned(msg.linear)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        angular: super::msg::ActuatorsAngularPosition::into_rmw_message(std::borrow::Cow::Borrowed(&msg.angular)).into_owned(),
        linear: super::msg::ActuatorsLinearPosition::into_rmw_message(std::borrow::Cow::Borrowed(&msg.linear)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      angular: super::msg::ActuatorsAngularPosition::from_rmw_message(msg.angular),
      linear: super::msg::ActuatorsLinearPosition::from_rmw_message(msg.linear),
    }
  }
}


// Corresponds to actuator_msgs__msg__ActuatorsVelocity

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsVelocity {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for velocity.
    /// Velocity of the actuators in
    pub angular: super::msg::ActuatorsAngularVelocity,

    /// Velocity of the actuators in
    pub linear: super::msg::ActuatorsLinearVelocity,

}



impl Default for ActuatorsVelocity {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ActuatorsVelocity::default())
  }
}

impl rosidl_runtime_rs::Message for ActuatorsVelocity {
  type RmwMsg = super::msg::rmw::ActuatorsVelocity;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        angular: super::msg::ActuatorsAngularVelocity::into_rmw_message(std::borrow::Cow::Owned(msg.angular)).into_owned(),
        linear: super::msg::ActuatorsLinearVelocity::into_rmw_message(std::borrow::Cow::Owned(msg.linear)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        angular: super::msg::ActuatorsAngularVelocity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.angular)).into_owned(),
        linear: super::msg::ActuatorsLinearVelocity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.linear)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      angular: super::msg::ActuatorsAngularVelocity::from_rmw_message(msg.angular),
      linear: super::msg::ActuatorsLinearVelocity::from_rmw_message(msg.linear),
    }
  }
}


