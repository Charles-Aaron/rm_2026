/**
 * @file static_initialization.hpp
 * @brief 已知水平静止条件下，统计零偏并构造同一时刻的状态与地图。
 */
#pragma once

#include <deque>

#include "zlio/initialization.hpp"

namespace zlio {

class StaticLevelInitializer {
public:
    explicit StaticLevelInitializer(const Config& config)
        : config_(config),
          expected_acceleration_(config.initialization.static_level.initial_rotation.conjugate() *
                                 Vec3(0, 0, config.gravity)) {}
    std::optional<InitializationResult> add(Scan scan, Time end, const std::vector<Imu>& imus);
    const InitializationStatus& status() const { return status_; }

private:
    struct Statistics {
        Vec3 acceleration = Vec3::Zero(), angular_velocity = Vec3::Zero();
        Mat3 accel_covariance = Mat3::Zero(), gyro_covariance = Mat3::Zero();
    };

    Config config_;
    Vec3 expected_acceleration_;
    std::deque<Imu> imus_;
    std::deque<Scan> scans_;
    std::optional<Time> last_imu_;
    InitializationStatus status_;

    void collectImus(const std::vector<Imu>& imus);
    Statistics statistics() const;
    bool stationary(const Statistics& stats);
    Mat15 covariance(const Statistics& stats) const;
    std::optional<InitializationResult> makeResult(Time end, const Statistics& stats);
};

}  // namespace zlio
