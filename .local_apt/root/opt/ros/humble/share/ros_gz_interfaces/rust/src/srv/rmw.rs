#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__ControlWorld_Request() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__ControlWorld_Request__init(msg: *mut ControlWorld_Request) -> bool;
    fn ros_gz_interfaces__srv__ControlWorld_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Request>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__ControlWorld_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Request>);
    fn ros_gz_interfaces__srv__ControlWorld_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ControlWorld_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Request>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__ControlWorld_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ControlWorld_Request {
    /// Message to Control world in Gazebo Sim
    pub world_control: super::super::msg::rmw::WorldControl,

}



impl Default for ControlWorld_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__ControlWorld_Request__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__ControlWorld_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ControlWorld_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ControlWorld_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ControlWorld_Request where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/ControlWorld_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__ControlWorld_Request() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__ControlWorld_Response() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__ControlWorld_Response__init(msg: *mut ControlWorld_Response) -> bool;
    fn ros_gz_interfaces__srv__ControlWorld_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Response>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__ControlWorld_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Response>);
    fn ros_gz_interfaces__srv__ControlWorld_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ControlWorld_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ControlWorld_Response>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__ControlWorld_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ControlWorld_Response {
    /// Return true if control is successful.
    pub success: bool,

}



impl Default for ControlWorld_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__ControlWorld_Response__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__ControlWorld_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ControlWorld_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__ControlWorld_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ControlWorld_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ControlWorld_Response where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/ControlWorld_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__ControlWorld_Response() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__DeleteEntity_Request() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__DeleteEntity_Request__init(msg: *mut DeleteEntity_Request) -> bool;
    fn ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Request>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Request>);
    fn ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DeleteEntity_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Request>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__DeleteEntity_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteEntity_Request {
    /// Gazebo Sim entity to be deleted.
    pub entity: super::super::msg::rmw::Entity,

}



impl Default for DeleteEntity_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__DeleteEntity_Request__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__DeleteEntity_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DeleteEntity_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DeleteEntity_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DeleteEntity_Request where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/DeleteEntity_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__DeleteEntity_Request() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__DeleteEntity_Response() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__DeleteEntity_Response__init(msg: *mut DeleteEntity_Response) -> bool;
    fn ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Response>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Response>);
    fn ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DeleteEntity_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<DeleteEntity_Response>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__DeleteEntity_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteEntity_Response {
    /// Return true if deletion is successful.
    pub success: bool,

}



impl Default for DeleteEntity_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__DeleteEntity_Response__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__DeleteEntity_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DeleteEntity_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__DeleteEntity_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DeleteEntity_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DeleteEntity_Response where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/DeleteEntity_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__DeleteEntity_Response() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SetEntityPose_Request() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__SetEntityPose_Request__init(msg: *mut SetEntityPose_Request) -> bool;
    fn ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Request>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Request>);
    fn ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEntityPose_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Request>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__SetEntityPose_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEntityPose_Request {
    /// Gazebo Sim entity.
    pub entity: super::super::msg::rmw::Entity,

    /// Pose of entity.
    pub pose: geometry_msgs::msg::rmw::Pose,

}



impl Default for SetEntityPose_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__SetEntityPose_Request__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__SetEntityPose_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEntityPose_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEntityPose_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEntityPose_Request where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/SetEntityPose_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SetEntityPose_Request() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SetEntityPose_Response() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__SetEntityPose_Response__init(msg: *mut SetEntityPose_Response) -> bool;
    fn ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Response>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Response>);
    fn ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SetEntityPose_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SetEntityPose_Response>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__SetEntityPose_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEntityPose_Response {
    /// Return true if set successfully.
    pub success: bool,

}



impl Default for SetEntityPose_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__SetEntityPose_Response__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__SetEntityPose_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SetEntityPose_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SetEntityPose_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SetEntityPose_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SetEntityPose_Response where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/SetEntityPose_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SetEntityPose_Response() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SpawnEntity_Request() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__SpawnEntity_Request__init(msg: *mut SpawnEntity_Request) -> bool;
    fn ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Request>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Request>);
    fn ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SpawnEntity_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Request>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__SpawnEntity_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnEntity_Request {
    /// Message to create a new entity
    pub entity_factory: super::super::msg::rmw::EntityFactory,

}



impl Default for SpawnEntity_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__SpawnEntity_Request__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__SpawnEntity_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SpawnEntity_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SpawnEntity_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SpawnEntity_Request where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/SpawnEntity_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SpawnEntity_Request() }
  }
}


#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SpawnEntity_Response() -> *const std::ffi::c_void;
}

#[link(name = "ros_gz_interfaces__rosidl_generator_c")]
extern "C" {
    fn ros_gz_interfaces__srv__SpawnEntity_Response__init(msg: *mut SpawnEntity_Response) -> bool;
    fn ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Response>, size: usize) -> bool;
    fn ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Response>);
    fn ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SpawnEntity_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SpawnEntity_Response>) -> bool;
}

// Corresponds to ros_gz_interfaces__srv__SpawnEntity_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnEntity_Response {
    /// Return true if spawned successfully.
    pub success: bool,

}



impl Default for SpawnEntity_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !ros_gz_interfaces__srv__SpawnEntity_Response__init(&mut msg as *mut _) {
        panic!("Call to ros_gz_interfaces__srv__SpawnEntity_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SpawnEntity_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { ros_gz_interfaces__srv__SpawnEntity_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SpawnEntity_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SpawnEntity_Response where Self: Sized {
  const TYPE_NAME: &'static str = "ros_gz_interfaces/srv/SpawnEntity_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__ros_gz_interfaces__srv__SpawnEntity_Response() }
  }
}






#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__ControlWorld() -> *const std::ffi::c_void;
}

// Corresponds to ros_gz_interfaces__srv__ControlWorld
#[allow(missing_docs, non_camel_case_types)]
pub struct ControlWorld;

impl rosidl_runtime_rs::Service for ControlWorld {
    type Request = ControlWorld_Request;
    type Response = ControlWorld_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__ControlWorld() }
    }
}




#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__DeleteEntity() -> *const std::ffi::c_void;
}

// Corresponds to ros_gz_interfaces__srv__DeleteEntity
#[allow(missing_docs, non_camel_case_types)]
pub struct DeleteEntity;

impl rosidl_runtime_rs::Service for DeleteEntity {
    type Request = DeleteEntity_Request;
    type Response = DeleteEntity_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__DeleteEntity() }
    }
}




#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__SetEntityPose() -> *const std::ffi::c_void;
}

// Corresponds to ros_gz_interfaces__srv__SetEntityPose
#[allow(missing_docs, non_camel_case_types)]
pub struct SetEntityPose;

impl rosidl_runtime_rs::Service for SetEntityPose {
    type Request = SetEntityPose_Request;
    type Response = SetEntityPose_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__SetEntityPose() }
    }
}




#[link(name = "ros_gz_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__SpawnEntity() -> *const std::ffi::c_void;
}

// Corresponds to ros_gz_interfaces__srv__SpawnEntity
#[allow(missing_docs, non_camel_case_types)]
pub struct SpawnEntity;

impl rosidl_runtime_rs::Service for SpawnEntity {
    type Request = SpawnEntity_Request;
    type Response = SpawnEntity_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__ros_gz_interfaces__srv__SpawnEntity() }
    }
}


