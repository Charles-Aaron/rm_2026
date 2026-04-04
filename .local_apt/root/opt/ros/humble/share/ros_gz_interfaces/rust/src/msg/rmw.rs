#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Altimeter() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Altimeter__init(msg: *mut Altimeter) -> bool;
    fn ros_gz_interfaces__msg__Altimeter__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Altimeter>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Altimeter__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Altimeter>);
    fn ros_gz_interfaces__msg__Altimeter__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Altimeter>, out_seq: *mut rosidl_runtime_rs::Sequence<Altimeter>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Altimeter
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A message for Altimeter readings.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Altimeter {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

    /// Vertical position data, in meters.
    pub vertical_position: f64,

    /// Vertical velocity data, in meters/second.
    pub vertical_velocity: f64,

    /// Vertical reference.
    pub vertical_reference: f64,

}



impl Default for Altimeter {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Altimeter__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Altimeter__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Altimeter {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Altimeter__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Altimeter__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Altimeter__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Altimeter {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Altimeter where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Altimeter";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Altimeter() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Contact() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Contact__init(msg: *mut Contact) -> bool;
    fn ros_gz_interfaces__msg__Contact__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Contact>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Contact__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Contact>);
    fn ros_gz_interfaces__msg__Contact__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Contact>, out_seq: *mut rosidl_runtime_rs::Sequence<Contact>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Contact
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Contact {
    /// Contact collision1
    pub collision1: super::super::msg::rmw::Entity,

    /// Contact collision2
    pub collision2: super::super::msg::rmw::Entity,

    /// List of contact position
    pub positions: rosidl_runtime_rs::Sequence<geometry_msgs::msg::rmw::Vector3>,

    /// List of contact normals
    pub normals: rosidl_runtime_rs::Sequence<geometry_msgs::msg::rmw::Vector3>,

    /// List of penetration depths
    pub depths: rosidl_runtime_rs::Sequence<f64>,

    /// List of joint wrenches including forces/torques
    pub wrenches: rosidl_runtime_rs::Sequence<super::super::msg::rmw::JointWrench>,

}



impl Default for Contact {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Contact__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Contact__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Contact {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contact__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contact__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contact__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Contact {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Contact where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Contact";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Contact() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Contacts() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Contacts__init(msg: *mut Contacts) -> bool;
    fn ros_gz_interfaces__msg__Contacts__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Contacts>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Contacts__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Contacts>);
    fn ros_gz_interfaces__msg__Contacts__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Contacts>, out_seq: *mut rosidl_runtime_rs::Sequence<Contacts>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Contacts
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Contacts {
    /// Time stamp
    pub header: std_msgs::msg::rmw::Header,

    /// List of contacts
    pub contacts: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Contact>,

}



impl Default for Contacts {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Contacts__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Contacts__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Contacts {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contacts__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contacts__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Contacts__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Contacts {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Contacts where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Contacts";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Contacts() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Dataframe() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Dataframe__init(msg: *mut Dataframe) -> bool;
    fn ros_gz_interfaces__msg__Dataframe__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Dataframe>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Dataframe__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Dataframe>);
    fn ros_gz_interfaces__msg__Dataframe__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Dataframe>, out_seq: *mut rosidl_runtime_rs::Sequence<Dataframe>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Dataframe
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Message containing some payload as well as sender and destination
/// Intended for use with the comms system in Gazebo

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Dataframe {
    /// Time stamp
    pub header: std_msgs::msg::rmw::Header,

    /// Address of the sender
    pub src_address: rosidl_runtime_rs::String,

    /// Address of the destination
    pub dst_address: rosidl_runtime_rs::String,

    /// Payload
    pub data: rosidl_runtime_rs::Sequence<u8>,

    /// Received Signal Strength Indicator
    pub rssi: f64,

}



impl Default for Dataframe {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Dataframe__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Dataframe__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Dataframe {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Dataframe__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Dataframe__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Dataframe__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Dataframe {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Dataframe where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Dataframe";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Dataframe() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Entity() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Entity__init(msg: *mut Entity) -> bool;
    fn ros_gz_interfaces__msg__Entity__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Entity>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Entity__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Entity>);
    fn ros_gz_interfaces__msg__Entity__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Entity>, out_seq: *mut rosidl_runtime_rs::Sequence<Entity>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Entity
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Entity type: constant definition

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Entity {
    /// Entity unique identifier across all types. Defaults to 0
    pub id: u64,

    /// Entity name, which is not guaranteed to be unique.
    pub name: rosidl_runtime_rs::String,

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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Entity__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Entity__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Entity {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Entity__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Entity__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Entity__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Entity {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Entity where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Entity";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Entity() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__EntityFactory() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__EntityFactory__init(msg: *mut EntityFactory) -> bool;
    fn ros_gz_interfaces__msg__EntityFactory__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<EntityFactory>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__EntityFactory__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<EntityFactory>);
    fn ros_gz_interfaces__msg__EntityFactory__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<EntityFactory>, out_seq: *mut rosidl_runtime_rs::Sequence<EntityFactory>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__EntityFactory
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntityFactory {
    /// New name for the entity, overrides the name on the SDF
    pub name: rosidl_runtime_rs::String,

    /// Whether the server is allowed to rename the entity in case of
    /// overlap with existing entities.
    pub allow_renaming: bool,

    /// Only one method is supported at a time (sdf,sdf_filename,clone_name)
    /// SDF description in string format
    pub sdf: rosidl_runtime_rs::String,

    /// Full path to SDF file.
    pub sdf_filename: rosidl_runtime_rs::String,

    /// Name of entity to clone
    pub clone_name: rosidl_runtime_rs::String,

    /// Pose where the entity will be spawned in the world.
    pub pose: geometry_msgs::msg::rmw::Pose,

    /// Pose is defined relative to the frame of this entity.
    pub relative_to: rosidl_runtime_rs::String,

}



impl Default for EntityFactory {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__EntityFactory__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__EntityFactory__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for EntityFactory {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityFactory__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityFactory__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityFactory__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for EntityFactory {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for EntityFactory where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/EntityFactory";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__EntityFactory() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__EntityWrench() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__EntityWrench__init(msg: *mut EntityWrench) -> bool;
    fn ros_gz_interfaces__msg__EntityWrench__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<EntityWrench>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__EntityWrench__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<EntityWrench>);
    fn ros_gz_interfaces__msg__EntityWrench__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<EntityWrench>, out_seq: *mut rosidl_runtime_rs::Sequence<EntityWrench>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__EntityWrench
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntityWrench {
    /// Time stamp
    pub header: std_msgs::msg::rmw::Header,

    /// Entity
    pub entity: super::super::msg::rmw::Entity,

    /// Wrench to be applied to entity
    pub wrench: geometry_msgs::msg::rmw::Wrench,

}



impl Default for EntityWrench {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__EntityWrench__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__EntityWrench__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for EntityWrench {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityWrench__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityWrench__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__EntityWrench__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for EntityWrench {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for EntityWrench where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/EntityWrench";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__EntityWrench() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Float32Array() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Float32Array__init(msg: *mut Float32Array) -> bool;
    fn ros_gz_interfaces__msg__Float32Array__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Float32Array>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Float32Array__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Float32Array>);
    fn ros_gz_interfaces__msg__Float32Array__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Float32Array>, out_seq: *mut rosidl_runtime_rs::Sequence<Float32Array>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Float32Array
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Float32Array {

    // This member is not documented.
    #[allow(missing_docs)]
    pub data: rosidl_runtime_rs::Sequence<f32>,

}



impl Default for Float32Array {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Float32Array__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Float32Array__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Float32Array {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Float32Array__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Float32Array__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Float32Array__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Float32Array {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Float32Array where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Float32Array";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Float32Array() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__GuiCamera() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__GuiCamera__init(msg: *mut GuiCamera) -> bool;
    fn ros_gz_interfaces__msg__GuiCamera__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GuiCamera>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__GuiCamera__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GuiCamera>);
    fn ros_gz_interfaces__msg__GuiCamera__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GuiCamera>, out_seq: *mut rosidl_runtime_rs::Sequence<GuiCamera>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__GuiCamera
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Message for a GUI Camera.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GuiCamera {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub view_controller: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub pose: geometry_msgs::msg::rmw::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub track: super::super::msg::rmw::TrackVisual,

    /// Type of projection: "perspective" or "orthographic".
    pub projection_type: rosidl_runtime_rs::String,

}



impl Default for GuiCamera {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__GuiCamera__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__GuiCamera__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GuiCamera {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__GuiCamera__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__GuiCamera__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__GuiCamera__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GuiCamera {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GuiCamera where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/GuiCamera";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__GuiCamera() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__JointWrench() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__JointWrench__init(msg: *mut JointWrench) -> bool;
    fn ros_gz_interfaces__msg__JointWrench__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<JointWrench>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__JointWrench__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<JointWrench>);
    fn ros_gz_interfaces__msg__JointWrench__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<JointWrench>, out_seq: *mut rosidl_runtime_rs::Sequence<JointWrench>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__JointWrench
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct JointWrench {
    /// Time stamp
    pub header: std_msgs::msg::rmw::Header,

    /// Body 1 name string
    pub body_1_name: std_msgs::msg::rmw::String,

    /// Body 1 id
    pub body_1_id: std_msgs::msg::rmw::UInt32,

    /// Body 2 name string
    pub body_2_name: std_msgs::msg::rmw::String,

    /// Body 2 id
    pub body_2_id: std_msgs::msg::rmw::UInt32,

    /// Body 1 wrench
    pub body_1_wrench: geometry_msgs::msg::rmw::Wrench,

    /// Body 2 wrench
    pub body_2_wrench: geometry_msgs::msg::rmw::Wrench,

}



impl Default for JointWrench {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__JointWrench__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__JointWrench__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for JointWrench {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__JointWrench__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__JointWrench__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__JointWrench__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for JointWrench {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for JointWrench where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/JointWrench";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__JointWrench() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Light() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__Light__init(msg: *mut Light) -> bool;
    fn ros_gz_interfaces__msg__Light__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Light>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__Light__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Light>);
    fn ros_gz_interfaces__msg__Light__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Light>, out_seq: *mut rosidl_runtime_rs::Sequence<Light>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__Light
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Light {
    /// Optional header data
    pub header: std_msgs::msg::rmw::Header,

    /// Light name
    pub name: rosidl_runtime_rs::String,

    /// Light type (from constant definitions)
    pub type_: u8,

    /// Light pose
    pub pose: geometry_msgs::msg::rmw::Pose,

    /// Light diffuse emission
    pub diffuse: std_msgs::msg::rmw::ColorRGBA,

    /// Light specular emission
    pub specular: std_msgs::msg::rmw::ColorRGBA,

    /// Constant variable in attenuation formula
    pub attenuation_constant: f32,

    /// Linear variable in attenuation formula
    pub attenuation_linear: f32,

    /// Quadratic variable in attenuation formula
    pub attenuation_quadratic: f32,

    /// Light direction
    pub direction: geometry_msgs::msg::rmw::Vector3,

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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__Light__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__Light__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Light {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Light__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Light__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__Light__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Light {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Light where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/Light";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__Light() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__LogPlaybackStatistics() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__LogPlaybackStatistics__init(msg: *mut LogPlaybackStatistics) -> bool;
    fn ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<LogPlaybackStatistics>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<LogPlaybackStatistics>);
    fn ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<LogPlaybackStatistics>, out_seq: *mut rosidl_runtime_rs::Sequence<LogPlaybackStatistics>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__LogPlaybackStatistics
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct LogPlaybackStatistics {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// log start time
    pub start_time: builtin_interfaces::msg::rmw::Time,

    /// log end time
    pub end_time: builtin_interfaces::msg::rmw::Time,

}



impl Default for LogPlaybackStatistics {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__LogPlaybackStatistics__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__LogPlaybackStatistics__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for LogPlaybackStatistics {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__LogPlaybackStatistics__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for LogPlaybackStatistics {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for LogPlaybackStatistics where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/LogPlaybackStatistics";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__LogPlaybackStatistics() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__MaterialColor() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__MaterialColor__init(msg: *mut MaterialColor) -> bool;
    fn ros_gz_interfaces__msg__MaterialColor__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MaterialColor>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__MaterialColor__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MaterialColor>);
    fn ros_gz_interfaces__msg__MaterialColor__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MaterialColor>, out_seq: *mut rosidl_runtime_rs::Sequence<MaterialColor>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__MaterialColor
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Entities that match to apply material color: constant definition

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MaterialColor {
    /// Optional header data
    pub header: std_msgs::msg::rmw::Header,

    /// Entity to change material color
    pub entity: super::super::msg::rmw::Entity,

    /// Ambient color
    pub ambient: std_msgs::msg::rmw::ColorRGBA,

    /// Diffuse color
    pub diffuse: std_msgs::msg::rmw::ColorRGBA,

    /// Specular color
    pub specular: std_msgs::msg::rmw::ColorRGBA,

    /// Emissive color
    pub emissive: std_msgs::msg::rmw::ColorRGBA,

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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__MaterialColor__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__MaterialColor__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MaterialColor {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__MaterialColor__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__MaterialColor__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__MaterialColor__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MaterialColor {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MaterialColor where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/MaterialColor";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__MaterialColor() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__ParamVec() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__ParamVec__init(msg: *mut ParamVec) -> bool;
    fn ros_gz_interfaces__msg__ParamVec__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ParamVec>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__ParamVec__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ParamVec>);
    fn ros_gz_interfaces__msg__ParamVec__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ParamVec>, out_seq: *mut rosidl_runtime_rs::Sequence<ParamVec>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__ParamVec
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A message for a vector of parameters data.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ParamVec {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

    /// A set of name, value pairs
    pub params: rosidl_runtime_rs::Sequence<rcl_interfaces::msg::rmw::Parameter>,

}



impl Default for ParamVec {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__ParamVec__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__ParamVec__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ParamVec {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__ParamVec__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__ParamVec__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__ParamVec__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ParamVec {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ParamVec where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/ParamVec";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__ParamVec() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__SensorNoise() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__SensorNoise__init(msg: *mut SensorNoise) -> bool;
    fn ros_gz_interfaces__msg__SensorNoise__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SensorNoise>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__SensorNoise__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SensorNoise>);
    fn ros_gz_interfaces__msg__SensorNoise__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SensorNoise>, out_seq: *mut rosidl_runtime_rs::Sequence<SensorNoise>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__SensorNoise
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A message for specifying sensor noise.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SensorNoise {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__SensorNoise__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__SensorNoise__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SensorNoise {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__SensorNoise__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__SensorNoise__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__SensorNoise__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SensorNoise {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SensorNoise where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/SensorNoise";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__SensorNoise() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__StringVec() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__StringVec__init(msg: *mut StringVec) -> bool;
    fn ros_gz_interfaces__msg__StringVec__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<StringVec>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__StringVec__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<StringVec>);
    fn ros_gz_interfaces__msg__StringVec__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<StringVec>, out_seq: *mut rosidl_runtime_rs::Sequence<StringVec>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__StringVec
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A message for a vector of string data.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StringVec {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

    /// The vector of strings.
    pub data: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,

}



impl Default for StringVec {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__StringVec__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__StringVec__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for StringVec {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__StringVec__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__StringVec__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__StringVec__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for StringVec {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for StringVec where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/StringVec";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__StringVec() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__TrackVisual() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__TrackVisual__init(msg: *mut TrackVisual) -> bool;
    fn ros_gz_interfaces__msg__TrackVisual__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<TrackVisual>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__TrackVisual__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<TrackVisual>);
    fn ros_gz_interfaces__msg__TrackVisual__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<TrackVisual>, out_seq: *mut rosidl_runtime_rs::Sequence<TrackVisual>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__TrackVisual
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Message for a tracking a rendering::Visual with a rendering::Camera.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TrackVisual {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

    /// Name of the visual to track.
    pub name: rosidl_runtime_rs::String,

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
    pub xyz: geometry_msgs::msg::rmw::Vector3,

    /// If set to true, the camera inherits the yaw rotation of the model.
    pub inherit_yaw: bool,

}



impl Default for TrackVisual {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__TrackVisual__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__TrackVisual__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for TrackVisual {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__TrackVisual__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__TrackVisual__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__TrackVisual__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for TrackVisual {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for TrackVisual where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/TrackVisual";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__TrackVisual() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__VideoRecord() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__VideoRecord__init(msg: *mut VideoRecord) -> bool;
    fn ros_gz_interfaces__msg__VideoRecord__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<VideoRecord>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__VideoRecord__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<VideoRecord>);
    fn ros_gz_interfaces__msg__VideoRecord__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<VideoRecord>, out_seq: *mut rosidl_runtime_rs::Sequence<VideoRecord>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__VideoRecord
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A message that allows for control of video recording functions.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VideoRecord {
    /// Optional header data.
    pub header: std_msgs::msg::rmw::Header,

    /// True to start video recording.
    pub start: bool,

    /// True to stop video recording.
    pub stop: bool,

    /// Video encoding format, e.g. "mp4", "ogv".
    pub format: rosidl_runtime_rs::String,

    /// filename of the recorded video.
    pub save_filename: rosidl_runtime_rs::String,

}



impl Default for VideoRecord {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__VideoRecord__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__VideoRecord__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for VideoRecord {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__VideoRecord__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__VideoRecord__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__VideoRecord__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for VideoRecord {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for VideoRecord where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/VideoRecord";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__VideoRecord() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldControl() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__WorldControl__init(msg: *mut WorldControl) -> bool;
    fn ros_gz_interfaces__msg__WorldControl__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<WorldControl>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__WorldControl__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<WorldControl>);
    fn ros_gz_interfaces__msg__WorldControl__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<WorldControl>, out_seq: *mut rosidl_runtime_rs::Sequence<WorldControl>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__WorldControl
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
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
    pub reset: super::super::msg::rmw::WorldReset,


    // This member is not documented.
    #[allow(missing_docs)]
    pub seed: u32,

    /// A simulation time in the future to run to and
    /// then pause.
    pub run_to_sim_time: builtin_interfaces::msg::rmw::Time,

}



impl Default for WorldControl {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__WorldControl__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__WorldControl__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for WorldControl {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldControl__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldControl__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldControl__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for WorldControl {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for WorldControl where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/WorldControl";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldControl() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldReset() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__WorldReset__init(msg: *mut WorldReset) -> bool;
    fn ros_gz_interfaces__msg__WorldReset__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<WorldReset>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__WorldReset__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<WorldReset>);
    fn ros_gz_interfaces__msg__WorldReset__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<WorldReset>, out_seq: *mut rosidl_runtime_rs::Sequence<WorldReset>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__WorldReset
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__WorldReset__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__WorldReset__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for WorldReset {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldReset__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldReset__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldReset__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for WorldReset {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for WorldReset where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/WorldReset";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldReset() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldStatistics() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__msg__WorldStatistics__init(msg: *mut WorldStatistics) -> bool;
    fn ros_gz_interfaces__msg__WorldStatistics__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<WorldStatistics>, size: usize) -> bool;
    fn ros_gz_interfaces__msg__WorldStatistics__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<WorldStatistics>);
    fn ros_gz_interfaces__msg__WorldStatistics__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<WorldStatistics>, out_seq: *mut rosidl_runtime_rs::Sequence<WorldStatistics>) -> bool;
}

// Corresponds to ros_gz_interfaces__msg__WorldStatistics
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// optional header data

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct WorldStatistics {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// Current simulation time
    pub sim_time: builtin_interfaces::msg::rmw::Time,

    /// Total time spent paused
    pub pause_time: builtin_interfaces::msg::rmw::Time,

    /// Current real time
    pub real_time: builtin_interfaces::msg::rmw::Time,

    /// Whether currently paused
    pub paused: bool,

    /// Current iteration count
    pub iterations: u64,

    /// Total number of models in the world
    pub model_count: i32,

    /// Statistics for log playback
    pub log_playback_statistics: super::super::msg::rmw::LogPlaybackStatistics,

    /// This factor expresses how much real time elapses with each step
    /// of simulation time.
    pub real_time_factor: f64,

    /// Iteration step size. It's zero when paused.
    pub step_size: builtin_interfaces::msg::rmw::Time,

}



impl Default for WorldStatistics {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__msg__WorldStatistics__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__msg__WorldStatistics__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for WorldStatistics {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldStatistics__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldStatistics__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__msg__WorldStatistics__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for WorldStatistics {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for WorldStatistics where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/msg/WorldStatistics";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__msg__WorldStatistics() }
  }
}


