#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to bond__msg__Constants

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Constants {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}

impl Constants {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEAD_PUBLISH_PERIOD: f32 = 0.05;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEFAULT_CONNECT_TIMEOUT: f32 = 10.0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEFAULT_HEARTBEAT_TIMEOUT: f32 = 4.0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEFAULT_DISCONNECT_TIMEOUT: f32 = 2.0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEFAULT_HEARTBEAT_PERIOD: f32 = 1.0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DISABLE_HEARTBEAT_TIMEOUT_PARAM: &'static str = "/bond_disable_heartbeat_timeout";

}


impl Default for Constants {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Constants::default())
  }
}

impl rosidl_runtime_rs::Message for Constants {
  type RmwMsg = super::msg::rmw::Constants;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to bond__msg__Status

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Status {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// ID of the bond
    pub id: std::string::String,

    /// Unique ID for an individual in a bond
    pub instance_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub active: bool,

    /// Including the timeouts for the bond makes it easier to debug mis-matches
    /// between the two sides.
    pub heartbeat_timeout: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub heartbeat_period: f32,

}



impl Default for Status {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Status::default())
  }
}

impl rosidl_runtime_rs::Message for Status {
  type RmwMsg = super::msg::rmw::Status;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        id: msg.id.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        active: msg.active,
        heartbeat_timeout: msg.heartbeat_timeout,
        heartbeat_period: msg.heartbeat_period,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        id: msg.id.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      active: msg.active,
      heartbeat_timeout: msg.heartbeat_timeout,
      heartbeat_period: msg.heartbeat_period,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      id: msg.id.to_string(),
      instance_id: msg.instance_id.to_string(),
      active: msg.active,
      heartbeat_timeout: msg.heartbeat_timeout,
      heartbeat_period: msg.heartbeat_period,
    }
  }
}


