#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__Actuators() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__Actuators__init(msg: *mut Actuators) -> bool;
    fn actuator_msgs__msg__Actuators__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Actuators>, size: usize) -> bool;
    fn actuator_msgs__msg__Actuators__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Actuators>);
    fn actuator_msgs__msg__Actuators__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Actuators>, out_seq: *mut rosidl_runtime_rs::Sequence<Actuators>) -> bool;
}

// Corresponds to actuator_msgs__msg__Actuators
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Actuators {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// This message defines lowest level commands to be sent to the actuator(s).
    /// Position of the actuators in [rad] for angular actuators
    /// and [m] for linear actuators.
    pub position: rosidl_runtime_rs::Sequence<f64>,

    /// Velocities of the actuators in [rad/s] for angular actuators
    /// and [m/s] for linear actuators.
    pub velocity: rosidl_runtime_rs::Sequence<f64>,

    /// Everything that does not fit the above,
    /// normalized between.
    pub normalized: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for Actuators {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__Actuators__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__Actuators__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Actuators {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__Actuators__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__Actuators__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__Actuators__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Actuators {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Actuators where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/Actuators";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__Actuators() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsAngularPosition() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsAngularPosition__init(msg: *mut ActuatorsAngularPosition) -> bool;
    fn actuator_msgs__msg__ActuatorsAngularPosition__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularPosition>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsAngularPosition__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularPosition>);
    fn actuator_msgs__msg__ActuatorsAngularPosition__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsAngularPosition>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularPosition>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsAngularPosition
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// This message defines lowest level commands to be sent
/// to the actuator(s) for angular position.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsAngularPosition {
    /// Position of the actuators in
    pub position: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for ActuatorsAngularPosition {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsAngularPosition__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsAngularPosition__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsAngularPosition {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularPosition__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularPosition__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularPosition__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsAngularPosition {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsAngularPosition where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsAngularPosition";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsAngularPosition() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsAngularVelocity() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsAngularVelocity__init(msg: *mut ActuatorsAngularVelocity) -> bool;
    fn actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularVelocity>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularVelocity>);
    fn actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsAngularVelocity>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsAngularVelocity>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsAngularVelocity
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// This message defines lowest level commands to be sent
/// to the actuator(s) for angular velocity.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsAngularVelocity {
    /// Velocity of the actuators in
    pub velocity: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for ActuatorsAngularVelocity {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsAngularVelocity__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsAngularVelocity__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsAngularVelocity {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsAngularVelocity__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsAngularVelocity {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsAngularVelocity where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsAngularVelocity";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsAngularVelocity() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsLinearPosition() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsLinearPosition__init(msg: *mut ActuatorsLinearPosition) -> bool;
    fn actuator_msgs__msg__ActuatorsLinearPosition__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearPosition>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsLinearPosition__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearPosition>);
    fn actuator_msgs__msg__ActuatorsLinearPosition__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsLinearPosition>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearPosition>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsLinearPosition
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// This message defines lowest level commands to be sent
/// to the actuator(s) for linear position.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsLinearPosition {
    /// Position of the actuators in
    pub position: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for ActuatorsLinearPosition {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsLinearPosition__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsLinearPosition__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsLinearPosition {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearPosition__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearPosition__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearPosition__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsLinearPosition {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsLinearPosition where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsLinearPosition";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsLinearPosition() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsLinearVelocity() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsLinearVelocity__init(msg: *mut ActuatorsLinearVelocity) -> bool;
    fn actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearVelocity>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearVelocity>);
    fn actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsLinearVelocity>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsLinearVelocity>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsLinearVelocity
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// This message defines lowest level commands to be sent
/// to the actuator(s) for linear velocity.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsLinearVelocity {
    /// Velocity of the actuators in
    pub velocity: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for ActuatorsLinearVelocity {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsLinearVelocity__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsLinearVelocity__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsLinearVelocity {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsLinearVelocity__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsLinearVelocity {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsLinearVelocity where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsLinearVelocity";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsLinearVelocity() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsNormalized() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsNormalized__init(msg: *mut ActuatorsNormalized) -> bool;
    fn actuator_msgs__msg__ActuatorsNormalized__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsNormalized>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsNormalized__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsNormalized>);
    fn actuator_msgs__msg__ActuatorsNormalized__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsNormalized>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsNormalized>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsNormalized
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsNormalized {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for normalized outputs.
    /// Normalized between.
    pub normalized: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for ActuatorsNormalized {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsNormalized__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsNormalized__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsNormalized {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsNormalized__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsNormalized__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsNormalized__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsNormalized {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsNormalized where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsNormalized";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsNormalized() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsPosition() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsPosition__init(msg: *mut ActuatorsPosition) -> bool;
    fn actuator_msgs__msg__ActuatorsPosition__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsPosition>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsPosition__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsPosition>);
    fn actuator_msgs__msg__ActuatorsPosition__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsPosition>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsPosition>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsPosition
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsPosition {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for position.
    /// Position of the actuators in
    pub angular: super::super::msg::rmw::ActuatorsAngularPosition,

    /// Position of the actuators in
    pub linear: super::super::msg::rmw::ActuatorsLinearPosition,

}



impl Default for ActuatorsPosition {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsPosition__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsPosition__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsPosition {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsPosition__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsPosition__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsPosition__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsPosition {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsPosition where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsPosition";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsPosition() }
  }
}


#[link(name = "actuator_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsVelocity() -> *const std::ffi::c_void;
}

#[link(name = "actuator_msgs__rosidl_generator_c")]
extern "C" {
    fn actuator_msgs__msg__ActuatorsVelocity__init(msg: *mut ActuatorsVelocity) -> bool;
    fn actuator_msgs__msg__ActuatorsVelocity__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsVelocity>, size: usize) -> bool;
    fn actuator_msgs__msg__ActuatorsVelocity__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ActuatorsVelocity>);
    fn actuator_msgs__msg__ActuatorsVelocity__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ActuatorsVelocity>, out_seq: *mut rosidl_runtime_rs::Sequence<ActuatorsVelocity>) -> bool;
}

// Corresponds to actuator_msgs__msg__ActuatorsVelocity
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ActuatorsVelocity {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// This message defines lowest level commands to be sent
    /// to the actuator(s) for velocity.
    /// Velocity of the actuators in
    pub angular: super::super::msg::rmw::ActuatorsAngularVelocity,

    /// Velocity of the actuators in
    pub linear: super::super::msg::rmw::ActuatorsLinearVelocity,

}



impl Default for ActuatorsVelocity {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !actuator_msgs__msg__ActuatorsVelocity__init(&mut msg as *mut _) {
        panic!("Call to actuator_msgs__msg__ActuatorsVelocity__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ActuatorsVelocity {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsVelocity__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsVelocity__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { actuator_msgs__msg__ActuatorsVelocity__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ActuatorsVelocity {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ActuatorsVelocity where Self: Sized {
  const TYPE_NAME: &'static str = "actuator_msgs/msg/ActuatorsVelocity";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__actuator_msgs__msg__ActuatorsVelocity() }
  }
}


