#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "gps_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__gps_msgs__msg__GPSFix() -> *const std::ffi::c_void;
}

#[link(name = "gps_msgs__rosidl_generator_c")]
extern "C" {
    fn gps_msgs__msg__GPSFix__init(msg: *mut GPSFix) -> bool;
    fn gps_msgs__msg__GPSFix__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GPSFix>, size: usize) -> bool;
    fn gps_msgs__msg__GPSFix__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GPSFix>);
    fn gps_msgs__msg__GPSFix__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GPSFix>, out_seq: *mut rosidl_runtime_rs::Sequence<GPSFix>) -> bool;
}

// Corresponds to gps_msgs__msg__GPSFix
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// A more complete GPS fix to supplement sensor_msgs/NavSatFix.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GPSFix {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub status: super::super::msg::rmw::GPSStatus,

    /// Latitude (degrees). Positive is north of equator; negative is south.
    pub latitude: f64,

    /// Longitude (degrees). Positive is east of prime meridian, negative west.
    pub longitude: f64,

    /// Altitude (meters). Positive is above reference (e.g., sea level).
    pub altitude: f64,

    /// Direction (degrees from north)
    pub track: f64,

    /// Ground speed (meters/second)
    pub speed: f64,

    /// Vertical speed (meters/second)
    pub climb: f64,

    /// Device orientation (units in degrees)
    pub pitch: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub roll: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dip: f64,

    /// GPS time
    pub time: f64,

    /// Dilution of precision; Xdop<=0 means the value is unknown
    /// Total (positional-temporal) dilution of precision
    pub gdop: f64,

    /// Positional (3D) dilution of precision
    pub pdop: f64,

    /// Horizontal dilution of precision
    pub hdop: f64,

    /// Vertical dilution of precision
    pub vdop: f64,

    /// Temporal dilution of precision
    pub tdop: f64,

    /// Uncertainty of measurement, 95% confidence
    /// Spherical position uncertainty (meters)
    pub err: f64,

    /// Horizontal position uncertainty (meters)
    pub err_horz: f64,

    /// Vertical position uncertainty (meters)
    pub err_vert: f64,

    /// Track uncertainty (degrees)
    pub err_track: f64,

    /// Ground speed uncertainty (meters/second)
    pub err_speed: f64,

    /// Vertical speed uncertainty (meters/second)
    pub err_climb: f64,

    /// Temporal uncertainty
    pub err_time: f64,

    /// Orientation uncertainty (degrees)
    pub err_pitch: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub err_roll: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub err_dip: f64,

    /// Position covariance defined relative to a tangential plane
    /// through the reported position. The components are East, North, and
    /// Up (ENU), in row-major order.
    pub position_covariance: [f64; 9],


    // This member is not documented.
    #[allow(missing_docs)]
    pub position_covariance_type: u8,

}

impl GPSFix {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const COVARIANCE_TYPE_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const COVARIANCE_TYPE_APPROXIMATED: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const COVARIANCE_TYPE_DIAGONAL_KNOWN: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const COVARIANCE_TYPE_KNOWN: u8 = 3;

}


impl Default for GPSFix {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !gps_msgs__msg__GPSFix__init(&mut msg as *mut _) {
        panic!("Call to gps_msgs__msg__GPSFix__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GPSFix {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSFix__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSFix__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSFix__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GPSFix {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GPSFix where Self: Sized {
  const TYPE_NAME: &'static str = "gps_msgs/msg/GPSFix";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__gps_msgs__msg__GPSFix() }
  }
}


#[link(name = "gps_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__gps_msgs__msg__GPSStatus() -> *const std::ffi::c_void;
}

#[link(name = "gps_msgs__rosidl_generator_c")]
extern "C" {
    fn gps_msgs__msg__GPSStatus__init(msg: *mut GPSStatus) -> bool;
    fn gps_msgs__msg__GPSStatus__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GPSStatus>, size: usize) -> bool;
    fn gps_msgs__msg__GPSStatus__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GPSStatus>);
    fn gps_msgs__msg__GPSStatus__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GPSStatus>, out_seq: *mut rosidl_runtime_rs::Sequence<GPSStatus>) -> bool;
}

// Corresponds to gps_msgs__msg__GPSStatus
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GPSStatus {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,

    /// Satellites used in solution
    /// Number of satellites
    pub satellites_used: u16,

    /// PRN identifiers
    pub satellite_used_prn: rosidl_runtime_rs::Sequence<i32>,

    /// Satellites visible
    pub satellites_visible: u16,

    /// PRN identifiers
    pub satellite_visible_prn: rosidl_runtime_rs::Sequence<i32>,

    /// Elevation of satellites
    pub satellite_visible_z: rosidl_runtime_rs::Sequence<i32>,

    /// Azimuth of satellites
    pub satellite_visible_azimuth: rosidl_runtime_rs::Sequence<i32>,

    /// Signal-to-noise ratios (dB)
    pub satellite_visible_snr: rosidl_runtime_rs::Sequence<i32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i16,

    /// Source for speed, climb and track
    pub motion_source: u16,

    /// Source for device orientation
    pub orientation_source: u16,

    /// Source for position
    pub position_source: u16,

}

impl GPSStatus {
    /// Measurement status
    /// Unable to fix position
    pub const STATUS_NO_FIX: i16 = -1;

    /// Normal fix
    pub const STATUS_FIX: i16 = 0;

    /// Fixed using a satellite-based augmentation system
    pub const STATUS_SBAS_FIX: i16 = 1;

    /// or a ground-based augmentation system
    pub const STATUS_GBAS_FIX: i16 = 2;

    /// Fixed with DGPS
    pub const STATUS_DGPS_FIX: i16 = 18;

    /// Real-Time Kinematic, fixed integers
    pub const STATUS_RTK_FIX: i16 = 19;

    /// Real-Time Kinematic, float integers
    pub const STATUS_RTK_FLOAT: i16 = 20;

    /// Fixed with WAAS
    pub const STATUS_WAAS_FIX: i16 = 33;

    /// No information is available
    pub const SOURCE_NONE: u16 = 0;

    /// Using standard GPS location
    pub const SOURCE_GPS: u16 = 1;

    /// Motion/orientation fix is derived from successive points
    pub const SOURCE_POINTS: u16 = 2;

    /// Motion is derived using the Doppler effect
    pub const SOURCE_DOPPLER: u16 = 4;

    /// Using an altimeter
    pub const SOURCE_ALTIMETER: u16 = 8;

    /// Using magnetic sensors
    pub const SOURCE_MAGNETIC: u16 = 16;

    /// Using gyroscopes
    pub const SOURCE_GYRO: u16 = 32;

    /// Using accelerometers
    pub const SOURCE_ACCEL: u16 = 64;

}


impl Default for GPSStatus {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !gps_msgs__msg__GPSStatus__init(&mut msg as *mut _) {
        panic!("Call to gps_msgs__msg__GPSStatus__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GPSStatus {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSStatus__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSStatus__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { gps_msgs__msg__GPSStatus__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GPSStatus {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GPSStatus where Self: Sized {
  const TYPE_NAME: &'static str = "gps_msgs/msg/GPSStatus";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__gps_msgs__msg__GPSStatus() }
  }
}


