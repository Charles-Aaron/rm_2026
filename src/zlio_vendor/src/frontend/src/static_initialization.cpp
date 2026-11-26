/**
 * @file static_initialization.cpp
 * @brief 由已知水平底盘及安装姿态确定初值；静止 IMU 估计零偏，不求启动轨迹。
 */
#include "static_initialization.hpp"

#include <Eigen/Eigenvalues>
#include <cmath>
#include <utility>

#include "bootstrap.hpp"
#include "covariance.hpp"
#include "zlio/state.hpp"

namespace zlio {

/// 同步包的边界可能重复或经过插值；只统计一次真实采样，保留最近静止窗口。
void StaticLevelInitializer::collectImus(const std::vector<Imu>& imus) {
    for (const auto& imu : imus) {
        if (imu.source_begin != imu.source_end || (last_imu_ && imu.time <= *last_imu_))
            continue;
        imus_.push_back(imu);
        last_imu_ = imu.time;
    }
    if (imus_.empty())
        return;
    const Time cutoff = imus_.back().time -
                        nanoseconds(config_.initialization.static_level.minimum_duration);
    // 留一个跨越时间窗起点的真实样本，并满足低采样率时的样本数要求。
    while (imus_.size() > config_.initialization.seed_imu_samples && imus_[1].time <= cutoff)
        imus_.pop_front();
}

/// Welford 统计保留三轴相关项，避免大时间戳或长序列的相消误差。
StaticLevelInitializer::Statistics StaticLevelInitializer::statistics() const {
    Statistics stats;
    std::size_t count = 0;
    for (const auto& imu : imus_) {
        ++count;
        const Vec3 accel_delta = imu.acceleration - stats.acceleration;
        const Vec3 gyro_delta = imu.angular_velocity - stats.angular_velocity;
        stats.acceleration += accel_delta / static_cast<double>(count);
        stats.angular_velocity += gyro_delta / static_cast<double>(count);
        stats.accel_covariance += accel_delta * (imu.acceleration - stats.acceleration).transpose();
        stats.gyro_covariance += gyro_delta * (imu.angular_velocity - stats.angular_velocity).transpose();
    }
    const double divisor = static_cast<double>(count - 1);
    stats.accel_covariance =
        (.5 / divisor) * (stats.accel_covariance + stats.accel_covariance.transpose()).eval();
    stats.gyro_covariance =
        (.5 / divisor) * (stats.gyro_covariance + stats.gyro_covariance.transpose()).eval();
    return stats;
}

/// 水平姿态是本模式的已知条件；明显运动或无法用合理零偏解释的数据拒绝启动。
bool StaticLevelInitializer::stationary(const Statistics& stats) {
    const auto& limits = config_.initialization.static_level;
    const Vec3 accel_bias = stats.acceleration - expected_acceleration_;
    const char* reason = nullptr;
    if (stats.angular_velocity.norm() > limits.maximum_gyro_bias)
        reason = "static_level: angular velocity mean exceeds stationary bias limit";
    else if (stats.gyro_covariance.trace() > std::pow(limits.maximum_gyro_std, 2))
        reason = "static_level: angular velocity is not stable";
    else if (stats.accel_covariance.trace() > std::pow(limits.maximum_accel_std, 2))
        reason = "static_level: acceleration is not stable";
    else if ((accel_bias - config_.initial_accel_bias).norm() > limits.maximum_accel_bias)
        reason = "static_level: level assumption or acceleration bias limit is violated";
    if (!reason)
        return true;
    scans_.clear();
    status_ = {reason, InitializationState::NotStationary};
    return false;
}

/// 保留安装姿态的不确定性及其与加速度零偏的相关性，不把静止状态设为零协方差。
Mat15 StaticLevelInitializer::covariance(const Statistics& stats) const {
    const auto& prior = config_.initialization.registration;
    Mat15 result = Mat15::Identity() * prior.initial_state_variance;
    const Mat3 floor = Mat3::Identity() * prior.initial_bias_variance;
    const double count = static_cast<double>(imus_.size());
    result.block<3, 3>(9, 9) = stats.gyro_covariance / count + floor;
    const Mat3 gravity_jacobian =
        config_.initialization.static_level.initial_rotation.toRotationMatrix().transpose() *
        skew(Vec3(0, 0, -config_.gravity));
    const Mat3 attitude = result.block<3, 3>(3, 3);
    result.block<3, 3>(12, 12) = stats.accel_covariance / count + floor +
                                 gravity_jacobian * attitude * gravity_jacobian.transpose();
    result.block<3, 3>(3, 12) = attitude * gravity_jacobian.transpose();
    result.block<3, 3>(12, 3) = gravity_jacobian * attitude;
    return result;
}

/// 只检查固定姿态下的平面支撑，不优化姿态、位移、速度或重力方向。
std::optional<InitializationResult> StaticLevelInitializer::makeResult(
    Time end, const Statistics& stats) {
    std::vector<std::vector<Vec3>> clouds;
    clouds.reserve(scans_.size());
    std::vector<Observation> observations;
    const auto& extrinsic = config_.body_from_lidar;
    const Mat3 rotation = extrinsic.rotation.toRotationMatrix();
    const RayMeasurementCovariance noise(config_.map.range_sigma, config_.map.angle_sigma_degrees);
    for (const auto& scan : scans_) {
        auto& cloud = clouds.emplace_back();
        cloud.reserve(scan.size());
        for (const auto& point : scan) {
            const Vec3 body_point = extrinsic.apply(point.position);
            cloud.push_back(body_point);
            observations.push_back({body_point, rotation * noise(point.position) * rotation.transpose(),
                                    point.time, point.id});
        }
    }
    std::vector<WindowState> states(scans_.size());
    StartupPlaneModel planes(config_.initialization.optimization);
    planes.rebuild(clouds, states, true);
    if (planes.size() < config_.initialization.optimization.minimum_planes || !planes.connected()) {
        status_ = {"static_level: insufficient connected map planes",
                   InitializationState::InsufficientPlanes};
        return std::nullopt;
    }
    const Eigen::SelfAdjointEigenSolver<Mat3> support(planes.normalSupport());
    if (support.info() != Eigen::Success ||
        support.eigenvalues()[0] < config_.initialization.minimum_directional_support) {
        status_ = {"static_level: insufficient directional map support", InitializationState::WeakGeometry};
        return std::nullopt;
    }

    InitializationResult result;
    result.gravity = Vec3(0, 0, -config_.gravity);
    auto& state = result.estimate.state;
    state.time = end;
    // IMU 原点仍为前端原点；姿态补偿安装倾角，辅助 TF 使底盘与 odom 重合。
    state.pose.rotation = config_.initialization.static_level.initial_rotation;
    state.gyro_bias = stats.angular_velocity;
    state.accel_bias = stats.acceleration - expected_acceleration_;
    result.estimate.covariance = covariance(stats);
    if (!numericallyPositiveCovariance(result.estimate.covariance))
        throw NumericalError("static_level initialization covariance is not positive definite");
    result.map = transformForMap(observations, result.estimate);
    status_ = {"static_level initialization complete", InitializationState::Ready};
    scans_.clear();
    imus_.clear();
    return result;
}

/// 在同步扫描末端交接；新旧滤波器共用此时刻，启动窗口不被再次积分。
std::optional<InitializationResult> StaticLevelInitializer::add(
    Scan scan, Time end, const std::vector<Imu>& imus) {
    collectImus(imus);
    scans_.push_back(std::move(scan));
    while (scans_.size() > config_.initialization.window_frames)
        scans_.pop_front();
    if (imus_.size() < config_.initialization.seed_imu_samples ||
        seconds(imus_.back().time - imus_.front().time) <
            config_.initialization.static_level.minimum_duration) {
        status_ = {"static_level: collecting stationary IMU samples", InitializationState::Collecting};
        return std::nullopt;
    }
    const auto stats = statistics();
    if (!stationary(stats))
        return std::nullopt;
    if (scans_.size() < config_.initialization.window_frames) {
        status_ = {"static_level: collecting stationary scans", InitializationState::Collecting};
        return std::nullopt;
    }
    try {
        return makeResult(end, stats);
    } catch (const NumericalError& error) {
        scans_.clear();
        status_ = {error.what(), InitializationState::Failed};
        return std::nullopt;
    }
}

}  // namespace zlio
