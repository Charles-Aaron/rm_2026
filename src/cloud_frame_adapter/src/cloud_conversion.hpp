#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

#include "geometry_msgs/msg/transform.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "tf2/LinearMath/Transform.h"

namespace cloud_frame_adapter
{
// Build the rotation matrix once; subsequent clouds only use R * p + t.
inline tf2::Transform fixed_matrix(const geometry_msgs::msg::Transform & message)
{
  tf2::Quaternion rotation(
    message.rotation.x, message.rotation.y, message.rotation.z, message.rotation.w);
  const auto & t = message.translation;
  if (!std::isfinite(rotation.length2()) || rotation.length2() <= 0.0 ||
    !std::isfinite(t.x) || !std::isfinite(t.y) || !std::isfinite(t.z))
  {
    throw std::invalid_argument("Fixed transform contains an invalid rotation or translation");
  }
  rotation.normalize();
  return tf2::Transform(rotation, tf2::Vector3(t.x, t.y, t.z));
}

inline void convert_cloud(
  sensor_msgs::msg::PointCloud2 & cloud, const tf2::Transform & matrix,
  const std::string & target_frame)
{
  const uint16_t endian_probe = 1;
  const bool native_bigendian = *reinterpret_cast<const uint8_t *>(&endian_probe) == 0;
  if (cloud.is_bigendian != native_bigendian) {
    throw std::invalid_argument("Point cloud byte order must match the host");
  }

  const std::array<std::string, 3> names{"x", "y", "z"};
  std::array<size_t, 3> offsets{};
  for (size_t axis = 0; axis < names.size(); ++axis) {
    bool found = false;
    for (const auto & field : cloud.fields) {
      if (field.name != names[axis]) {continue;}
      if (field.datatype != sensor_msgs::msg::PointField::FLOAT32 || field.count != 1 ||
        static_cast<uint64_t>(field.offset) + sizeof(float) > cloud.point_step)
      {
        throw std::invalid_argument("Point cloud XYZ must be scalar FLOAT32 fields within point_step");
      }
      offsets[axis] = field.offset;
      found = true;
      break;
    }
    if (!found) {throw std::invalid_argument("Point cloud is missing an XYZ field");}
  }
  if (static_cast<uint64_t>(cloud.width) * cloud.point_step > cloud.row_step ||
    static_cast<uint64_t>(cloud.height) * cloud.row_step > cloud.data.size())
  {
    throw std::invalid_argument("Point cloud data is shorter than its declared layout");
  }

  // Modify only XYZ in the owned message, preserving timestamps, intensity and padding.
  for (uint32_t row = 0; row < cloud.height; ++row) {
    for (uint32_t col = 0; col < cloud.width; ++col) {
      auto * point = cloud.data.data() + static_cast<size_t>(row) * cloud.row_step +
        static_cast<size_t>(col) * cloud.point_step;
      std::array<float, 3> xyz;
      for (size_t axis = 0; axis < xyz.size(); ++axis) {
        std::memcpy(&xyz[axis], point + offsets[axis], sizeof(float));
      }
      if (!std::isfinite(xyz[0]) || !std::isfinite(xyz[1]) || !std::isfinite(xyz[2])) {
        cloud.is_dense = false;
        continue;
      }
      const auto converted = matrix * tf2::Vector3(xyz[0], xyz[1], xyz[2]);
      for (size_t axis = 0; axis < xyz.size(); ++axis) {
        const auto value = static_cast<float>(converted[axis]);
        std::memcpy(point + offsets[axis], &value, sizeof(float));
      }
    }
  }
  cloud.header.frame_id = target_frame;
}
}  // namespace cloud_frame_adapter
