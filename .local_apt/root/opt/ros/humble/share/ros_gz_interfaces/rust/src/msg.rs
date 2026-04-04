#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to ros_gz_interfaces__msg__Altimeter
/// A message for Altimeter readings.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Altimeter {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// Vertical position data, in meters.
    pub vertical_position: f64,

    /// Vertical velocity data, in meters/second.
    pub vertical_velocity: f64,

    /// Vertical reference.
    pub vertical_reference: f64,

}



impl Default for Altimeter {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Altimeter::default())
  }
}

impl rosidl_runtime_rs::Message for Altimeter {
  type RmwMsg = super::msg::rmw::Altimeter;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        vertical_position: msg.vertical_position,
        vertical_velocity: msg.vertical_velocity,
        vertical_reference: msg.vertical_reference,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
      vertical_position: msg.vertical_position,
      vertical_velocity: msg.vertical_velocity,
      vertical_reference: msg.vertical_reference,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      vertical_position: msg.vertical_position,
      vertical_velocity: msg.vertical_velocity,
      vertical_reference: msg.vertical_reference,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Contact

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Contact {
    /// Contact collision1
    pub collision1: super::msg::Entity,

    /// Contact collision2
    pub collision2: super::msg::Entity,

    /// List of contact position
    pub positions: Vec<geometry_msgs::msg::Vector3>,

    /// List of contact normals
    pub normals: Vec<geometry_msgs::msg::Vector3>,

    /// List of penetration depths
    pub depths: Vec<f64>,

    /// List of joint wrenches including forces/torques
    pub wrenches: Vec<super::msg::JointWrench>,

}



impl Default for Contact {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Contact::default())
  }
}

impl rosidl_runtime_rs::Message for Contact {
  type RmwMsg = super::msg::rmw::Contact;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        collision1: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.collision1)).into_owned(),
        collision2: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.collision2)).into_owned(),
        positions: msg.positions
          .into_iter()
          .map(|elem| geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        normals: msg.normals
          .into_iter()
          .map(|elem| geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        depths: msg.depths.into(),
        wrenches: msg.wrenches
          .into_iter()
          .map(|elem| super::msg::JointWrench::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        collision1: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.collision1)).into_owned(),
        collision2: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.collision2)).into_owned(),
        positions: msg.positions
          .iter()
          .map(|elem| geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        normals: msg.normals
          .iter()
          .map(|elem| geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        depths: msg.depths.as_slice().into(),
        wrenches: msg.wrenches
          .iter()
          .map(|elem| super::msg::JointWrench::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      collision1: super::msg::Entity::from_rmw_message(msg.collision1),
      collision2: super::msg::Entity::from_rmw_message(msg.collision2),
      positions: msg.positions
          .into_iter()
          .map(geometry_msgs::msg::Vector3::from_rmw_message)
          .collect(),
      normals: msg.normals
          .into_iter()
          .map(geometry_msgs::msg::Vector3::from_rmw_message)
          .collect(),
      depths: msg.depths
          .into_iter()
          .collect(),
      wrenches: msg.wrenches
          .into_iter()
          .map(super::msg::JointWrench::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Contacts

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Contacts {
    /// Time stamp
    pub header: std_msgs::msg::Header,

    /// List of contacts
    pub contacts: Vec<super::msg::Contact>,

}



impl Default for Contacts {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Contacts::default())
  }
}

impl rosidl_runtime_rs::Message for Contacts {
  type RmwMsg = super::msg::rmw::Contacts;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        contacts: msg.contacts
          .into_iter()
          .map(|elem| super::msg::Contact::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        contacts: msg.contacts
          .iter()
          .map(|elem| super::msg::Contact::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      contacts: msg.contacts
          .into_iter()
          .map(super::msg::Contact::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Dataframe
/// Message containing some payload as well as sender and destination
/// Intended for use with the comms system in Gazebo

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Dataframe {
    /// Time stamp
    pub header: std_msgs::msg::Header,

    /// Address of the sender
    pub src_address: std::string::String,

    /// Address of the destination
    pub dst_address: std::string::String,

    /// Payload
    pub data: Vec<u8>,

    /// Received Signal Strength Indicator
    pub rssi: f64,

}



impl Default for Dataframe {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Dataframe::default())
  }
}

impl rosidl_runtime_rs::Message for Dataframe {
  type RmwMsg = super::msg::rmw::Dataframe;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        src_address: msg.src_address.as_str().into(),
        dst_address: msg.dst_address.as_str().into(),
        data: msg.data.into(),
        rssi: msg.rssi,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        src_address: msg.src_address.as_str().into(),
        dst_address: msg.dst_address.as_str().into(),
        data: msg.data.as_slice().into(),
      rssi: msg.rssi,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      src_address: msg.src_address.to_string(),
      dst_address: msg.dst_address.to_string(),
      data: msg.data
          .into_iter()
          .collect(),
      rssi: msg.rssi,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Entity
/// Entity type: constant definition

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Entity {
    /// Entity unique identifier across all types. Defaults to 0
    pub id: u64,

    /// Entity name, which is not guaranteed to be unique.
    pub name: std::string::String,

    /// Entity type.
    pub type_: u8,

}

impl Entity {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const NONE: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const LIGHT: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const MODEL: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const LINK: u8 = 3;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const VISUAL: u8 = 4;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const COLLISION: u8 = 5;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const SENSOR: u8 = 6;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const JOINT: u8 = 7;

}


impl Default for Entity {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Entity::default())
  }
}

impl rosidl_runtime_rs::Message for Entity {
  type RmwMsg = super::msg::rmw::Entity;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id,
        name: msg.name.as_str().into(),
        type_: msg.type_,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      id: msg.id,
        name: msg.name.as_str().into(),
      type_: msg.type_,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id,
      name: msg.name.to_string(),
      type_: msg.type_,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__EntityFactory

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntityFactory {
    /// New name for the entity, overrides the name on the SDF
    pub name: std::string::String,

    /// Whether the server is allowed to rename the entity in case of
    /// overlap with existing entities.
    pub allow_renaming: bool,

    /// Only one method is supported at a time (sdf,sdf_filename,clone_name)
    /// SDF description in string format
    pub sdf: std::string::String,

    /// Full path to SDF file.
    pub sdf_filename: std::string::String,

    /// Name of entity to clone
    pub clone_name: std::string::String,

    /// Pose where the entity will be spawned in the world.
    pub pose: geometry_msgs::msg::Pose,

    /// Pose is defined relative to the frame of this entity.
    pub relative_to: std::string::String,

}



impl Default for EntityFactory {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::EntityFactory::default())
  }
}

impl rosidl_runtime_rs::Message for EntityFactory {
  type RmwMsg = super::msg::rmw::EntityFactory;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        allow_renaming: msg.allow_renaming,
        sdf: msg.sdf.as_str().into(),
        sdf_filename: msg.sdf_filename.as_str().into(),
        clone_name: msg.clone_name.as_str().into(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
        relative_to: msg.relative_to.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      allow_renaming: msg.allow_renaming,
        sdf: msg.sdf.as_str().into(),
        sdf_filename: msg.sdf_filename.as_str().into(),
        clone_name: msg.clone_name.as_str().into(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
        relative_to: msg.relative_to.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      allow_renaming: msg.allow_renaming,
      sdf: msg.sdf.to_string(),
      sdf_filename: msg.sdf_filename.to_string(),
      clone_name: msg.clone_name.to_string(),
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
      relative_to: msg.relative_to.to_string(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__EntityWrench

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntityWrench {
    /// Time stamp
    pub header: std_msgs::msg::Header,

    /// Entity
    pub entity: super::msg::Entity,

    /// Wrench to be applied to entity
    pub wrench: geometry_msgs::msg::Wrench,

}



impl Default for EntityWrench {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::EntityWrench::default())
  }
}

impl rosidl_runtime_rs::Message for EntityWrench {
  type RmwMsg = super::msg::rmw::EntityWrench;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.entity)).into_owned(),
        wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Owned(msg.wrench)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.entity)).into_owned(),
        wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Borrowed(&msg.wrench)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      entity: super::msg::Entity::from_rmw_message(msg.entity),
      wrench: geometry_msgs::msg::Wrench::from_rmw_message(msg.wrench),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Float32Array

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Float32Array {

    // This member is not documented.
    #[allow(missing_docs)]
    pub data: Vec<f32>,

}



impl Default for Float32Array {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Float32Array::default())
  }
}

impl rosidl_runtime_rs::Message for Float32Array {
  type RmwMsg = super::msg::rmw::Float32Array;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        data: msg.data.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        data: msg.data.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      data: msg.data
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__GuiCamera
/// Message for a GUI Camera.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GuiCamera {
    /// Optional header data.
    pub header: std_msgs::msg::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub view_controller: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub pose: geometry_msgs::msg::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub track: super::msg::TrackVisual,

    /// Type of projection: "perspective" or "orthographic".
    pub projection_type: std::string::String,

}



impl Default for GuiCamera {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::GuiCamera::default())
  }
}

impl rosidl_runtime_rs::Message for GuiCamera {
  type RmwMsg = super::msg::rmw::GuiCamera;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        name: msg.name.as_str().into(),
        view_controller: msg.view_controller.as_str().into(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
        track: super::msg::TrackVisual::into_rmw_message(std::borrow::Cow::Owned(msg.track)).into_owned(),
        projection_type: msg.projection_type.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        name: msg.name.as_str().into(),
        view_controller: msg.view_controller.as_str().into(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
        track: super::msg::TrackVisual::into_rmw_message(std::borrow::Cow::Borrowed(&msg.track)).into_owned(),
        projection_type: msg.projection_type.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      name: msg.name.to_string(),
      view_controller: msg.view_controller.to_string(),
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
      track: super::msg::TrackVisual::from_rmw_message(msg.track),
      projection_type: msg.projection_type.to_string(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__JointWrench

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct JointWrench {
    /// Time stamp
    pub header: std_msgs::msg::Header,

    /// Body 1 name string
    pub body_1_name: std_msgs::msg::String,

    /// Body 1 id
    pub body_1_id: std_msgs::msg::UInt32,

    /// Body 2 name string
    pub body_2_name: std_msgs::msg::String,

    /// Body 2 id
    pub body_2_id: std_msgs::msg::UInt32,

    /// Body 1 wrench
    pub body_1_wrench: geometry_msgs::msg::Wrench,

    /// Body 2 wrench
    pub body_2_wrench: geometry_msgs::msg::Wrench,

}



impl Default for JointWrench {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::JointWrench::default())
  }
}

impl rosidl_runtime_rs::Message for JointWrench {
  type RmwMsg = super::msg::rmw::JointWrench;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        body_1_name: std_msgs::msg::String::into_rmw_message(std::borrow::Cow::Owned(msg.body_1_name)).into_owned(),
        body_1_id: std_msgs::msg::UInt32::into_rmw_message(std::borrow::Cow::Owned(msg.body_1_id)).into_owned(),
        body_2_name: std_msgs::msg::String::into_rmw_message(std::borrow::Cow::Owned(msg.body_2_name)).into_owned(),
        body_2_id: std_msgs::msg::UInt32::into_rmw_message(std::borrow::Cow::Owned(msg.body_2_id)).into_owned(),
        body_1_wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Owned(msg.body_1_wrench)).into_owned(),
        body_2_wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Owned(msg.body_2_wrench)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        body_1_name: std_msgs::msg::String::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_1_name)).into_owned(),
        body_1_id: std_msgs::msg::UInt32::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_1_id)).into_owned(),
        body_2_name: std_msgs::msg::String::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_2_name)).into_owned(),
        body_2_id: std_msgs::msg::UInt32::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_2_id)).into_owned(),
        body_1_wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_1_wrench)).into_owned(),
        body_2_wrench: geometry_msgs::msg::Wrench::into_rmw_message(std::borrow::Cow::Borrowed(&msg.body_2_wrench)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      body_1_name: std_msgs::msg::String::from_rmw_message(msg.body_1_name),
      body_1_id: std_msgs::msg::UInt32::from_rmw_message(msg.body_1_id),
      body_2_name: std_msgs::msg::String::from_rmw_message(msg.body_2_name),
      body_2_id: std_msgs::msg::UInt32::from_rmw_message(msg.body_2_id),
      body_1_wrench: geometry_msgs::msg::Wrench::from_rmw_message(msg.body_1_wrench),
      body_2_wrench: geometry_msgs::msg::Wrench::from_rmw_message(msg.body_2_wrench),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__Light

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Light {
    /// Optional header data
    pub header: std_msgs::msg::Header,

    /// Light name
    pub name: std::string::String,

    /// Light type (from constant definitions)
    pub type_: u8,

    /// Light pose
    pub pose: geometry_msgs::msg::Pose,

    /// Light diffuse emission
    pub diffuse: std_msgs::msg::ColorRGBA,

    /// Light specular emission
    pub specular: std_msgs::msg::ColorRGBA,

    /// Constant variable in attenuation formula
    pub attenuation_constant: f32,

    /// Linear variable in attenuation formula
    pub attenuation_linear: f32,

    /// Quadratic variable in attenuation formula
    pub attenuation_quadratic: f32,

    /// Light direction
    pub direction: geometry_msgs::msg::Vector3,

    /// Light range
    pub range: f32,

    /// Enable/disable shadow casting
    pub cast_shadows: bool,

    /// Spotlight inner cone angle
    pub spot_inner_angle: f32,

    /// Spotlight outer cone angle
    pub spot_outer_angle: f32,

    /// Falloff between inner and outer cone
    pub spot_falloff: f32,

    /// Unique id of the light
    pub id: u32,

    /// Unique id of the light's parent
    pub parent_id: u32,

    /// Light intensity
    pub intensity: f32,

}

impl Light {
    /// Light type: constant definition
    pub const POINT: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const SPOT: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DIRECTIONAL: u8 = 2;

}


impl Default for Light {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Light::default())
  }
}

impl rosidl_runtime_rs::Message for Light {
  type RmwMsg = super::msg::rmw::Light;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        name: msg.name.as_str().into(),
        type_: msg.type_,
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
        diffuse: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.diffuse)).into_owned(),
        specular: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.specular)).into_owned(),
        attenuation_constant: msg.attenuation_constant,
        attenuation_linear: msg.attenuation_linear,
        attenuation_quadratic: msg.attenuation_quadratic,
        direction: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.direction)).into_owned(),
        range: msg.range,
        cast_shadows: msg.cast_shadows,
        spot_inner_angle: msg.spot_inner_angle,
        spot_outer_angle: msg.spot_outer_angle,
        spot_falloff: msg.spot_falloff,
        id: msg.id,
        parent_id: msg.parent_id,
        intensity: msg.intensity,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        name: msg.name.as_str().into(),
      type_: msg.type_,
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
        diffuse: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.diffuse)).into_owned(),
        specular: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.specular)).into_owned(),
      attenuation_constant: msg.attenuation_constant,
      attenuation_linear: msg.attenuation_linear,
      attenuation_quadratic: msg.attenuation_quadratic,
        direction: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.direction)).into_owned(),
      range: msg.range,
      cast_shadows: msg.cast_shadows,
      spot_inner_angle: msg.spot_inner_angle,
      spot_outer_angle: msg.spot_outer_angle,
      spot_falloff: msg.spot_falloff,
      id: msg.id,
      parent_id: msg.parent_id,
      intensity: msg.intensity,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      name: msg.name.to_string(),
      type_: msg.type_,
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
      diffuse: std_msgs::msg::ColorRGBA::from_rmw_message(msg.diffuse),
      specular: std_msgs::msg::ColorRGBA::from_rmw_message(msg.specular),
      attenuation_constant: msg.attenuation_constant,
      attenuation_linear: msg.attenuation_linear,
      attenuation_quadratic: msg.attenuation_quadratic,
      direction: geometry_msgs::msg::Vector3::from_rmw_message(msg.direction),
      range: msg.range,
      cast_shadows: msg.cast_shadows,
      spot_inner_angle: msg.spot_inner_angle,
      spot_outer_angle: msg.spot_outer_angle,
      spot_falloff: msg.spot_falloff,
      id: msg.id,
      parent_id: msg.parent_id,
      intensity: msg.intensity,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__LogPlaybackStatistics

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LogPlaybackStatistics {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// log start time
    pub start_time: builtin_interfaces::msg::Time,

    /// log end time
    pub end_time: builtin_interfaces::msg::Time,

}



impl Default for LogPlaybackStatistics {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::LogPlaybackStatistics::default())
  }
}

impl rosidl_runtime_rs::Message for LogPlaybackStatistics {
  type RmwMsg = super::msg::rmw::LogPlaybackStatistics;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        start_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.start_time)).into_owned(),
        end_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.end_time)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        start_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.start_time)).into_owned(),
        end_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.end_time)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      start_time: builtin_interfaces::msg::Time::from_rmw_message(msg.start_time),
      end_time: builtin_interfaces::msg::Time::from_rmw_message(msg.end_time),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__MaterialColor
/// Entities that match to apply material color: constant definition

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MaterialColor {
    /// Optional header data
    pub header: std_msgs::msg::Header,

    /// Entity to change material color
    pub entity: super::msg::Entity,

    /// Ambient color
    pub ambient: std_msgs::msg::ColorRGBA,

    /// Diffuse color
    pub diffuse: std_msgs::msg::ColorRGBA,

    /// Specular color
    pub specular: std_msgs::msg::ColorRGBA,

    /// Emissive color
    pub emissive: std_msgs::msg::ColorRGBA,

    /// Specular exponent
    pub shininess: f64,

    /// Entities that match to apply material color
    pub entity_match: u8,

}

impl MaterialColor {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const FIRST: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const ALL: u8 = 1;

}


impl Default for MaterialColor {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::MaterialColor::default())
  }
}

impl rosidl_runtime_rs::Message for MaterialColor {
  type RmwMsg = super::msg::rmw::MaterialColor;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.entity)).into_owned(),
        ambient: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.ambient)).into_owned(),
        diffuse: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.diffuse)).into_owned(),
        specular: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.specular)).into_owned(),
        emissive: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Owned(msg.emissive)).into_owned(),
        shininess: msg.shininess,
        entity_match: msg.entity_match,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.entity)).into_owned(),
        ambient: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.ambient)).into_owned(),
        diffuse: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.diffuse)).into_owned(),
        specular: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.specular)).into_owned(),
        emissive: std_msgs::msg::ColorRGBA::into_rmw_message(std::borrow::Cow::Borrowed(&msg.emissive)).into_owned(),
      shininess: msg.shininess,
      entity_match: msg.entity_match,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      entity: super::msg::Entity::from_rmw_message(msg.entity),
      ambient: std_msgs::msg::ColorRGBA::from_rmw_message(msg.ambient),
      diffuse: std_msgs::msg::ColorRGBA::from_rmw_message(msg.diffuse),
      specular: std_msgs::msg::ColorRGBA::from_rmw_message(msg.specular),
      emissive: std_msgs::msg::ColorRGBA::from_rmw_message(msg.emissive),
      shininess: msg.shininess,
      entity_match: msg.entity_match,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__ParamVec
/// A message for a vector of parameters data.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ParamVec {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// A set of name, value pairs
    pub params: Vec<rcl_interfaces::msg::Parameter>,

}



impl Default for ParamVec {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ParamVec::default())
  }
}

impl rosidl_runtime_rs::Message for ParamVec {
  type RmwMsg = super::msg::rmw::ParamVec;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        params: msg.params
          .into_iter()
          .map(|elem| rcl_interfaces::msg::Parameter::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        params: msg.params
          .iter()
          .map(|elem| rcl_interfaces::msg::Parameter::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      params: msg.params
          .into_iter()
          .map(rcl_interfaces::msg::Parameter::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__SensorNoise
/// A message for specifying sensor noise.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SensorNoise {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// The type of noise
    pub type_: u8,

    /// Noise mean
    /// Used by GAUSSIAN and GAUSSIAN_QUANTIZED
    pub mean: f64,

    /// Noise standard deviation
    /// Used by GAUSSIAN and GAUSSIAN_QUANTIZED
    pub stddev: f64,

    /// Noise mean bias
    /// Used by GAUSSIAN and GAUSSIAN_QUANTIZED
    pub bias_mean: f64,

    /// Noise standard deviation bias
    pub bias_stddev: f64,

    /// Noise precision
    /// Used by GAUSSIAN_QUANTIZED
    pub precision: f64,

    /// For type "gaussian*", the standard deviation of the noise used to
    /// drive a process to model slow variations in a sensor bias.
    pub dynamic_bias_stddev: f64,

    /// For type "gaussian*", the correlation time in seconds of the
    /// noise used to drive a process to model slow variations in a sensor bias.
    /// A typical value, when used, would be on the order of
    /// 3600 seconds (1 hour).
    pub dynamic_bias_correlation_time: f64,

}

impl SensorNoise {
    /// Noise type
    pub const NONE: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const GAUSSIAN: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const GAUSSIAN_QUANTIZED: u8 = 3;

}


impl Default for SensorNoise {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::SensorNoise::default())
  }
}

impl rosidl_runtime_rs::Message for SensorNoise {
  type RmwMsg = super::msg::rmw::SensorNoise;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        type_: msg.type_,
        mean: msg.mean,
        stddev: msg.stddev,
        bias_mean: msg.bias_mean,
        bias_stddev: msg.bias_stddev,
        precision: msg.precision,
        dynamic_bias_stddev: msg.dynamic_bias_stddev,
        dynamic_bias_correlation_time: msg.dynamic_bias_correlation_time,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
      type_: msg.type_,
      mean: msg.mean,
      stddev: msg.stddev,
      bias_mean: msg.bias_mean,
      bias_stddev: msg.bias_stddev,
      precision: msg.precision,
      dynamic_bias_stddev: msg.dynamic_bias_stddev,
      dynamic_bias_correlation_time: msg.dynamic_bias_correlation_time,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      type_: msg.type_,
      mean: msg.mean,
      stddev: msg.stddev,
      bias_mean: msg.bias_mean,
      bias_stddev: msg.bias_stddev,
      precision: msg.precision,
      dynamic_bias_stddev: msg.dynamic_bias_stddev,
      dynamic_bias_correlation_time: msg.dynamic_bias_correlation_time,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__StringVec
/// A message for a vector of string data.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StringVec {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// The vector of strings.
    pub data: Vec<std::string::String>,

}



impl Default for StringVec {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::StringVec::default())
  }
}

impl rosidl_runtime_rs::Message for StringVec {
  type RmwMsg = super::msg::rmw::StringVec;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        data: msg.data
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        data: msg.data
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      data: msg.data
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__TrackVisual
/// Message for a tracking a rendering::Visual with a rendering::Camera.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TrackVisual {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// Name of the visual to track.
    pub name: std::string::String,

    /// Id of the visual to track.
    pub id: u32,

    /// True to have the tracking camera inherit the orientation of
    /// the tracked visual.
    pub inherit_orientation: bool,

    /// Minimum follow distance.
    pub min_dist: f64,

    /// Maximum follow distance.
    pub max_dist: f64,

    /// If set to true, the position of the camera is fixed.
    pub is_static: bool,

    /// If set to true, the position of the camera is relative to the.
    /// model reference frame.
    pub use_model_frame: bool,

    /// Position of the camera.
    pub xyz: geometry_msgs::msg::Vector3,

    /// If set to true, the camera inherits the yaw rotation of the model.
    pub inherit_yaw: bool,

}



impl Default for TrackVisual {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::TrackVisual::default())
  }
}

impl rosidl_runtime_rs::Message for TrackVisual {
  type RmwMsg = super::msg::rmw::TrackVisual;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        name: msg.name.as_str().into(),
        id: msg.id,
        inherit_orientation: msg.inherit_orientation,
        min_dist: msg.min_dist,
        max_dist: msg.max_dist,
        is_static: msg.is_static,
        use_model_frame: msg.use_model_frame,
        xyz: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.xyz)).into_owned(),
        inherit_yaw: msg.inherit_yaw,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        name: msg.name.as_str().into(),
      id: msg.id,
      inherit_orientation: msg.inherit_orientation,
      min_dist: msg.min_dist,
      max_dist: msg.max_dist,
      is_static: msg.is_static,
      use_model_frame: msg.use_model_frame,
        xyz: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.xyz)).into_owned(),
      inherit_yaw: msg.inherit_yaw,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      name: msg.name.to_string(),
      id: msg.id,
      inherit_orientation: msg.inherit_orientation,
      min_dist: msg.min_dist,
      max_dist: msg.max_dist,
      is_static: msg.is_static,
      use_model_frame: msg.use_model_frame,
      xyz: geometry_msgs::msg::Vector3::from_rmw_message(msg.xyz),
      inherit_yaw: msg.inherit_yaw,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__VideoRecord
/// A message that allows for control of video recording functions.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VideoRecord {
    /// Optional header data.
    pub header: std_msgs::msg::Header,

    /// True to start video recording.
    pub start: bool,

    /// True to stop video recording.
    pub stop: bool,

    /// Video encoding format, e.g. "mp4", "ogv".
    pub format: std::string::String,

    /// filename of the recorded video.
    pub save_filename: std::string::String,

}



impl Default for VideoRecord {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::VideoRecord::default())
  }
}

impl rosidl_runtime_rs::Message for VideoRecord {
  type RmwMsg = super::msg::rmw::VideoRecord;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        start: msg.start,
        stop: msg.stop,
        format: msg.format.as_str().into(),
        save_filename: msg.save_filename.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
      start: msg.start,
      stop: msg.stop,
        format: msg.format.as_str().into(),
        save_filename: msg.save_filename.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      start: msg.start,
      stop: msg.stop,
      format: msg.format.to_string(),
      save_filename: msg.save_filename.to_string(),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__WorldControl

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct WorldControl {
    /// Paused state.
    pub pause: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub step: bool,

    /// Paused after stepping multi_step.
    pub multi_step: u32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub reset: super::msg::WorldReset,


    // This member is not documented.
    #[allow(missing_docs)]
    pub seed: u32,

    /// A simulation time in the future to run to and
    /// then pause.
    pub run_to_sim_time: builtin_interfaces::msg::Time,

}



impl Default for WorldControl {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::WorldControl::default())
  }
}

impl rosidl_runtime_rs::Message for WorldControl {
  type RmwMsg = super::msg::rmw::WorldControl;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        pause: msg.pause,
        step: msg.step,
        multi_step: msg.multi_step,
        reset: super::msg::WorldReset::into_rmw_message(std::borrow::Cow::Owned(msg.reset)).into_owned(),
        seed: msg.seed,
        run_to_sim_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.run_to_sim_time)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      pause: msg.pause,
      step: msg.step,
      multi_step: msg.multi_step,
        reset: super::msg::WorldReset::into_rmw_message(std::borrow::Cow::Borrowed(&msg.reset)).into_owned(),
      seed: msg.seed,
        run_to_sim_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.run_to_sim_time)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      pause: msg.pause,
      step: msg.step,
      multi_step: msg.multi_step,
      reset: super::msg::WorldReset::from_rmw_message(msg.reset),
      seed: msg.seed,
      run_to_sim_time: builtin_interfaces::msg::Time::from_rmw_message(msg.run_to_sim_time),
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__WorldReset

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct WorldReset {
    /// Reset time and model
    pub all: bool,

    /// Reset time only
    pub time_only: bool,

    /// Reset model only
    pub model_only: bool,

}



impl Default for WorldReset {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::WorldReset::default())
  }
}

impl rosidl_runtime_rs::Message for WorldReset {
  type RmwMsg = super::msg::rmw::WorldReset;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        all: msg.all,
        time_only: msg.time_only,
        model_only: msg.model_only,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      all: msg.all,
      time_only: msg.time_only,
      model_only: msg.model_only,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      all: msg.all,
      time_only: msg.time_only,
      model_only: msg.model_only,
    }
  }
}


// Corresponds to ros_gz_interfaces__msg__WorldStatistics
/// optional header data

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct WorldStatistics {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// Current simulation time
    pub sim_time: builtin_interfaces::msg::Time,

    /// Total time spent paused
    pub pause_time: builtin_interfaces::msg::Time,

    /// Current real time
    pub real_time: builtin_interfaces::msg::Time,

    /// Whether currently paused
    pub paused: bool,

    /// Current iteration count
    pub iterations: u64,

    /// Total number of models in the world
    pub model_count: i32,

    /// Statistics for log playback
    pub log_playback_statistics: super::msg::LogPlaybackStatistics,

    /// This factor expresses how much real time elapses with each step
    /// of simulation time.
    pub real_time_factor: f64,

    /// Iteration step size. It's zero when paused.
    pub step_size: builtin_interfaces::msg::Time,

}



impl Default for WorldStatistics {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::WorldStatistics::default())
  }
}

impl rosidl_runtime_rs::Message for WorldStatistics {
  type RmwMsg = super::msg::rmw::WorldStatistics;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        sim_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.sim_time)).into_owned(),
        pause_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.pause_time)).into_owned(),
        real_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.real_time)).into_owned(),
        paused: msg.paused,
        iterations: msg.iterations,
        model_count: msg.model_count,
        log_playback_statistics: super::msg::LogPlaybackStatistics::into_rmw_message(std::borrow::Cow::Owned(msg.log_playback_statistics)).into_owned(),
        real_time_factor: msg.real_time_factor,
        step_size: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.step_size)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        sim_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.sim_time)).into_owned(),
        pause_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pause_time)).into_owned(),
        real_time: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.real_time)).into_owned(),
      paused: msg.paused,
      iterations: msg.iterations,
      model_count: msg.model_count,
        log_playback_statistics: super::msg::LogPlaybackStatistics::into_rmw_message(std::borrow::Cow::Borrowed(&msg.log_playback_statistics)).into_owned(),
      real_time_factor: msg.real_time_factor,
        step_size: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.step_size)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      sim_time: builtin_interfaces::msg::Time::from_rmw_message(msg.sim_time),
      pause_time: builtin_interfaces::msg::Time::from_rmw_message(msg.pause_time),
      real_time: builtin_interfaces::msg::Time::from_rmw_message(msg.real_time),
      paused: msg.paused,
      iterations: msg.iterations,
      model_count: msg.model_count,
      log_playback_statistics: super::msg::LogPlaybackStatistics::from_rmw_message(msg.log_playback_statistics),
      real_time_factor: msg.real_time_factor,
      step_size: builtin_interfaces::msg::Time::from_rmw_message(msg.step_size),
    }
  }
}


