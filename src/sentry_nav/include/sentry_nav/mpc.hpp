#pragma once
#include <Eigen/Core>
#include <memory>
#include "sentry_omni_kino_planner/astar.hpp"
#include "sentry_nav/trajectory.hpp"

namespace sentry_nav {
namespace core = sentry_omni_kino_planner;
struct MpcConfig {
  int horizon = 40;
  double dt = 0.025;
  double control_dt = 0.025;
  // Effective response along fixed planning-frame axes (odom in the plugin).
  double response_x = 0.15, response_y = 0.15;
  double max_speed = 0.6, max_acceleration = 0.8;
  double timeout = 0.015;
  // Scales measured angular velocity in the rotating body response model.
  double spin_model_gain = 0.0;
  double position_weight = 8, velocity_weight = 2, delta_weight = 0.3;
  void validate() const;
};
struct MpcResult {
  bool success = false;
  std::string reason;
  std::string diagnostics;  // Populated on failure; independent of planning status.
  Eigen::Vector2d command = Eigen::Vector2d::Zero(); // fixed planning-frame axes (same frame as trajectory)
  double first_command_rate_relaxation = 0; // m/s^2; disturbance recovery, first step only
  std::vector<Eigen::Vector2d> predicted_positions;
};
class Mpc {
public:
  explicit Mpc(MpcConfig config = {});
  ~Mpc();
  void reset();
  // All vectors, including previous_command and predicted positions, share
  // the fixed trajectory frame. Body-frame conversion belongs to the caller.
  MpcResult solve(const Eigen::Vector2d & position,
    const Eigen::Vector2d & measured_velocity, const Eigen::Vector2d & previous_command,
    const TimedTrajectory & trajectory, double time,
    double speed_limit, double angular_velocity = 0.0);
private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
}
