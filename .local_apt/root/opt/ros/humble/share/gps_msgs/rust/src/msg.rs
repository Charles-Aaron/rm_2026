#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to gps_msgs__msg__GPSFix
/// A more complete GPS fix to supplement sensor_msgs/NavSatFix.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GPSFix {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub status: super::msg::GPSStatus,

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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::GPSFix::default())
  }
}

impl rosidl_runtime_rs::Message for GPSFix {
  type RmwMsg = super::msg::rmw::GPSFix;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        status: super::msg::GPSStatus::into_rmw_message(std::borrow::Cow::Owned(msg.status)).into_owned(),
        latitude: msg.latitude,
        longitude: msg.longitude,
        altitude: msg.altitude,
        track: msg.track,
        speed: msg.speed,
        climb: msg.climb,
        pitch: msg.pitch,
        roll: msg.roll,
        dip: msg.dip,
        time: msg.time,
        gdop: msg.gdop,
        pdop: msg.pdop,
        hdop: msg.hdop,
        vdop: msg.vdop,
        tdop: msg.tdop,
        err: msg.err,
        err_horz: msg.err_horz,
        err_vert: msg.err_vert,
        err_track: msg.err_track,
        err_speed: msg.err_speed,
        err_climb: msg.err_climb,
        err_time: msg.err_time,
        err_pitch: msg.err_pitch,
        err_roll: msg.err_roll,
        err_dip: msg.err_dip,
        position_covariance: msg.position_covariance,
        position_covariance_type: msg.position_covariance_type,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
        status: super::msg::GPSStatus::into_rmw_message(std::borrow::Cow::Borrowed(&msg.status)).into_owned(),
      latitude: msg.latitude,
      longitude: msg.longitude,
      altitude: msg.altitude,
      track: msg.track,
      speed: msg.speed,
      climb: msg.climb,
      pitch: msg.pitch,
      roll: msg.roll,
      dip: msg.dip,
      time: msg.time,
      gdop: msg.gdop,
      pdop: msg.pdop,
      hdop: msg.hdop,
      vdop: msg.vdop,
      tdop: msg.tdop,
      err: msg.err,
      err_horz: msg.err_horz,
      err_vert: msg.err_vert,
      err_track: msg.err_track,
      err_speed: msg.err_speed,
      err_climb: msg.err_climb,
      err_time: msg.err_time,
      err_pitch: msg.err_pitch,
      err_roll: msg.err_roll,
      err_dip: msg.err_dip,
        position_covariance: msg.position_covariance,
      position_covariance_type: msg.position_covariance_type,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      status: super::msg::GPSStatus::from_rmw_message(msg.status),
      latitude: msg.latitude,
      longitude: msg.longitude,
      altitude: msg.altitude,
      track: msg.track,
      speed: msg.speed,
      climb: msg.climb,
      pitch: msg.pitch,
      roll: msg.roll,
      dip: msg.dip,
      time: msg.time,
      gdop: msg.gdop,
      pdop: msg.pdop,
      hdop: msg.hdop,
      vdop: msg.vdop,
      tdop: msg.tdop,
      err: msg.err,
      err_horz: msg.err_horz,
      err_vert: msg.err_vert,
      err_track: msg.err_track,
      err_speed: msg.err_speed,
      err_climb: msg.err_climb,
      err_time: msg.err_time,
      err_pitch: msg.err_pitch,
      err_roll: msg.err_roll,
      err_dip: msg.err_dip,
      position_covariance: msg.position_covariance,
      position_covariance_type: msg.position_covariance_type,
    }
  }
}


// Corresponds to gps_msgs__msg__GPSStatus

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GPSStatus {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,

    /// Satellites used in solution
    /// Number of satellites
    pub satellites_used: u16,

    /// PRN identifiers
    pub satellite_used_prn: Vec<i32>,

    /// Satellites visible
    pub satellites_visible: u16,

    /// PRN identifiers
    pub satellite_visible_prn: Vec<i32>,

    /// Elevation of satellites
    pub satellite_visible_z: Vec<i32>,

    /// Azimuth of satellites
    pub satellite_visible_azimuth: Vec<i32>,

    /// Signal-to-noise ratios (dB)
    pub satellite_visible_snr: Vec<i32>,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::GPSStatus::default())
  }
}

impl rosidl_runtime_rs::Message for GPSStatus {
  type RmwMsg = super::msg::rmw::GPSStatus;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        satellites_used: msg.satellites_used,
        satellite_used_prn: msg.satellite_used_prn.into(),
        satellites_visible: msg.satellites_visible,
        satellite_visible_prn: msg.satellite_visible_prn.into(),
        satellite_visible_z: msg.satellite_visible_z.into(),
        satellite_visible_azimuth: msg.satellite_visible_azimuth.into(),
        satellite_visible_snr: msg.satellite_visible_snr.into(),
        status: msg.status,
        motion_source: msg.motion_source,
        orientation_source: msg.orientation_source,
        position_source: msg.position_source,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
      satellites_used: msg.satellites_used,
        satellite_used_prn: msg.satellite_used_prn.as_slice().into(),
      satellites_visible: msg.satellites_visible,
        satellite_visible_prn: msg.satellite_visible_prn.as_slice().into(),
        satellite_visible_z: msg.satellite_visible_z.as_slice().into(),
        satellite_visible_azimuth: msg.satellite_visible_azimuth.as_slice().into(),
        satellite_visible_snr: msg.satellite_visible_snr.as_slice().into(),
      status: msg.status,
      motion_source: msg.motion_source,
      orientation_source: msg.orientation_source,
      position_source: msg.position_source,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      satellites_used: msg.satellites_used,
      satellite_used_prn: msg.satellite_used_prn
          .into_iter()
          .collect(),
      satellites_visible: msg.satellites_visible,
      satellite_visible_prn: msg.satellite_visible_prn
          .into_iter()
          .collect(),
      satellite_visible_z: msg.satellite_visible_z
          .into_iter()
          .collect(),
      satellite_visible_azimuth: msg.satellite_visible_azimuth
          .into_iter()
          .collect(),
      satellite_visible_snr: msg.satellite_visible_snr
          .into_iter()
          .collect(),
      status: msg.status,
      motion_source: msg.motion_source,
      orientation_source: msg.orientation_source,
      position_source: msg.position_source,
    }
  }
}


