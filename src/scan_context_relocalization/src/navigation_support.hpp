#pragma once

#include <cmath>
#include <cstring>
#include <set>
#include <stdexcept>
#include <Eigen/Geometry>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/transform.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <backend/types.hpp>

namespace sentry_localization {
inline Eigen::Matrix4d matrix(const geometry_msgs::msg::Pose &pose) {
  const auto &q = pose.orientation;
  Eigen::Quaterniond rotation(q.w, q.x, q.y, q.z);
  const Eigen::Vector3d p(pose.position.x, pose.position.y, pose.position.z);
  if (!p.allFinite() || !std::isfinite(rotation.norm()) || rotation.norm() < 1e-6)
    throw std::invalid_argument("invalid pose");
  Eigen::Matrix4d result = Eigen::Matrix4d::Identity();
  result.block<3, 3>(0, 0) = rotation.normalized().toRotationMatrix();
  result.block<3, 1>(0, 3) = p;
  return result;
}

inline geometry_msgs::msg::Transform transform(const Eigen::Matrix4d &matrix) {
  geometry_msgs::msg::Transform result;
  result.translation.x = matrix(0, 3); result.translation.y = matrix(1, 3);
  result.translation.z = matrix(2, 3);
  const Eigen::Quaterniond q(matrix.block<3, 3>(0, 0));
  result.rotation.x = q.x(); result.rotation.y = q.y();
  result.rotation.z = q.z(); result.rotation.w = q.w();
  return result;
}

// Only the reliable session topic can change the active frontend identity.
// Late data/notifications from a retired instance or epoch cannot revive it.
struct Session {
  uint64_t instance = 0, epoch = 0;
  bool seen = false, failed = false;
  std::set<uint64_t> retired;
  bool accepts(uint64_t id, uint64_t generation) const {
    return seen && !failed && instance == id && epoch == generation;
  }
  bool update(uint64_t id, uint64_t generation, bool failure) {
    if (id == 0 || retired.count(id) || (seen && id == instance && generation < epoch)) return false;
    const bool changed = !seen || instance != id || epoch != generation;
    if (changed) {
      if (seen && instance != id) retired.insert(instance);
      instance = id; epoch = generation; seen = true; failed = failure;
    } else {
      failed = failed || failure;
    }
    return changed;
  }
};

// ZLIO exports little-endian float32 XYZ. Validate layout before reading bytes;
// do not let malformed fields or an organized cloud's padding change indexing.
inline backend::Cloud::Ptr decode(const sensor_msgs::msg::PointCloud2 &message) {
  if (message.is_bigendian || message.point_step == 0 ||
      uint64_t(message.row_step) < uint64_t(message.width) * message.point_step ||
      uint64_t(message.data.size()) < uint64_t(message.row_step) * message.height)
    throw std::invalid_argument("invalid ZLIO cloud layout");
  uint32_t offsets[3]{};
  for (int i = 0; i < 3; ++i) {
    const std::string name(1, "xyz"[i]);
    bool found = false;
    for (const auto &field : message.fields) if (field.name == name) {
      if (field.datatype != sensor_msgs::msg::PointField::FLOAT32 || field.count != 1 ||
          uint64_t(field.offset) + sizeof(float) > message.point_step)
        throw std::invalid_argument("expected float32 XYZ");
      offsets[i] = field.offset; found = true;
    }
    if (!found) throw std::invalid_argument("missing XYZ field");
  }
  auto cloud = backend::Cloud::Ptr(new backend::Cloud);
  cloud->reserve(size_t(message.width) * message.height);
  for (uint32_t row = 0; row < message.height; ++row) {
    for (uint32_t col = 0; col < message.width; ++col) {
      const auto *data = message.data.data() + size_t(row) * message.row_step + size_t(col) * message.point_step;
      float xyz[3];
      for (int i = 0; i < 3; ++i) std::memcpy(&xyz[i], data + offsets[i], sizeof(float));
      if (!std::isfinite(xyz[0]) || !std::isfinite(xyz[1]) || !std::isfinite(xyz[2])) continue;
      backend::PointType point;
      point.x = xyz[0]; point.y = xyz[1]; point.z = xyz[2]; point.intensity = 0;
      cloud->push_back(point);
    }
  }
  return cloud;
}

}  // namespace sentry_localization
