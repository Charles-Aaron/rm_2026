#pragma once

#include <Eigen/Geometry>
#include <stdexcept>

namespace sentry_frames {

/// T_imu_chassis 将底盘系向量变到 IMU 系；与 frames.yaml 的既有定义一致。
inline Eigen::Isometry3d imuFromChassis(
    const Eigen::Vector3d& translation, const Eigen::Vector3d& rpy) {
    if (!translation.allFinite() || !rpy.allFinite())
        throw std::invalid_argument("IMU/chassis extrinsics must be finite");
    Eigen::Isometry3d result = Eigen::Isometry3d::Identity();
    result.linear() = (Eigen::AngleAxisd(rpy.z(), Eigen::Vector3d::UnitZ()) *
                       Eigen::AngleAxisd(rpy.y(), Eigen::Vector3d::UnitY()) *
                       Eigen::AngleAxisd(rpy.x(), Eigen::Vector3d::UnitX())).toRotationMatrix();
    result.translation() = translation;
    return result;
}

struct LevelAlignment {
    Eigen::Quaterniond initial_imu_rotation = Eigen::Quaterniond::Identity();
    Eigen::Isometry3d odom_from_lio = Eigen::Isometry3d::Identity();
};

/// 已知底盘水平：用最短旋转补偿安装倾角，IMU 启动位置仍为前端原点。
inline LevelAlignment levelAlignment(const Eigen::Isometry3d& imu_from_chassis) {
    LevelAlignment result;
    const Eigen::Vector3d up_in_imu = imu_from_chassis.linear().col(2);
    // 纯 yaw 安装保持严格单位初始旋转，沿用现有地图与 TF 数值。
    if (!up_in_imu.isApprox(Eigen::Vector3d::UnitZ(), 1e-12))
        result.initial_imu_rotation = Eigen::Quaterniond::FromTwoVectors(
            up_in_imu, Eigen::Vector3d::UnitZ()).normalized();
    Eigen::Isometry3d initial_imu = Eigen::Isometry3d::Identity();
    initial_imu.linear() = result.initial_imu_rotation.toRotationMatrix();
    result.odom_from_lio = (initial_imu * imu_from_chassis).inverse();
    return result;
}

}  // namespace sentry_frames
