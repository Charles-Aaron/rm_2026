#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to ros_gz_interfaces__srv__ControlWorld_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ControlWorld_Request {
    /// Message to Control world in Gazebo Sim
    pub world_control: super::msg::WorldControl,

}



impl Default for ControlWorld_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ControlWorld_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ControlWorld_Request {
  type RmwMsg = super::srv::rmw::ControlWorld_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        world_control: super::msg::WorldControl::into_rmw_message(std::borrow::Cow::Owned(msg.world_control)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        world_control: super::msg::WorldControl::into_rmw_message(std::borrow::Cow::Borrowed(&msg.world_control)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      world_control: super::msg::WorldControl::from_rmw_message(msg.world_control),
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__ControlWorld_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ControlWorld_Response {
    /// Return true if control is successful.
    pub success: bool,

}



impl Default for ControlWorld_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ControlWorld_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ControlWorld_Response {
  type RmwMsg = super::srv::rmw::ControlWorld_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__DeleteEntity_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteEntity_Request {
    /// Gazebo Sim entity to be deleted.
    pub entity: super::msg::Entity,

}



impl Default for DeleteEntity_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::DeleteEntity_Request::default())
  }
}

impl rosidl_runtime_rs::Message for DeleteEntity_Request {
  type RmwMsg = super::srv::rmw::DeleteEntity_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.entity)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.entity)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      entity: super::msg::Entity::from_rmw_message(msg.entity),
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__DeleteEntity_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DeleteEntity_Response {
    /// Return true if deletion is successful.
    pub success: bool,

}



impl Default for DeleteEntity_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::DeleteEntity_Response::default())
  }
}

impl rosidl_runtime_rs::Message for DeleteEntity_Response {
  type RmwMsg = super::srv::rmw::DeleteEntity_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__SetEntityPose_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEntityPose_Request {
    /// Gazebo Sim entity.
    pub entity: super::msg::Entity,

    /// Pose of entity.
    pub pose: geometry_msgs::msg::Pose,

}



impl Default for SetEntityPose_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEntityPose_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SetEntityPose_Request {
  type RmwMsg = super::srv::rmw::SetEntityPose_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Owned(msg.entity)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.pose)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity: super::msg::Entity::into_rmw_message(std::borrow::Cow::Borrowed(&msg.entity)).into_owned(),
        pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.pose)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      entity: super::msg::Entity::from_rmw_message(msg.entity),
      pose: geometry_msgs::msg::Pose::from_rmw_message(msg.pose),
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__SetEntityPose_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SetEntityPose_Response {
    /// Return true if set successfully.
    pub success: bool,

}



impl Default for SetEntityPose_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SetEntityPose_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SetEntityPose_Response {
  type RmwMsg = super::srv::rmw::SetEntityPose_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__SpawnEntity_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnEntity_Request {
    /// Message to create a new entity
    pub entity_factory: super::msg::EntityFactory,

}



impl Default for SpawnEntity_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SpawnEntity_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SpawnEntity_Request {
  type RmwMsg = super::srv::rmw::SpawnEntity_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity_factory: super::msg::EntityFactory::into_rmw_message(std::borrow::Cow::Owned(msg.entity_factory)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        entity_factory: super::msg::EntityFactory::into_rmw_message(std::borrow::Cow::Borrowed(&msg.entity_factory)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      entity_factory: super::msg::EntityFactory::from_rmw_message(msg.entity_factory),
    }
  }
}


// Corresponds to ros_gz_interfaces__srv__SpawnEntity_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnEntity_Response {
    /// Return true if spawned successfully.
    pub success: bool,

}



impl Default for SpawnEntity_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SpawnEntity_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SpawnEntity_Response {
  type RmwMsg = super::srv::rmw::SpawnEntity_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
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


