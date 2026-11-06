#pragma once

#include "sentry_nav/trajectory.hpp"
#include "sentry_omni_kino_planner/astar.hpp"
#include <algorithm>
#include <cmath>

namespace sentry_nav {
// Sample in time and check each swept chord on the footprint-inflated map.
// This is a local collision check, not a continuous-curve certificate.
inline bool trajectory_window_safe(const TimedTrajectory & trajectory,
  const core::PreparedMap & map, const Eigen::Vector2d & position,
  double start, double horizon, double step = 0.025)
{
  if (trajectory.segments.empty() || !position.allFinite() || !std::isfinite(start) ||
    !std::isfinite(horizon) || !std::isfinite(step) || horizon <= 0 || step <= 0) {return false;}
  start = std::clamp(start, 0.0, trajectory.duration());
  const double end = std::min(trajectory.duration(), start + horizon);
  auto previous = trajectory.sample(start).position;
  if (!core::line_free(map, position, previous)) {return false;}
  for (double t = start; t < end;) {
    t = std::min(end, t + step);
    const auto point = trajectory.sample(t).position;
    if (!core::line_free(map, previous, point)) {return false;}
    previous = point;
  }
  return true;
}

inline bool candidate_state_valid(const TimedTrajectory & trajectory, double time,
  const Eigen::Vector2d & position, const Eigen::Vector2d & velocity,
  double max_position_error, double max_velocity_error, double max_speed,
  double max_acceleration, double horizon, double step = 0.025)
{
  if (trajectory.segments.empty() || !position.allFinite() || !velocity.allFinite() ||
    !std::isfinite(time) || !std::isfinite(max_position_error) || !std::isfinite(max_velocity_error) ||
    !std::isfinite(max_speed) || !std::isfinite(max_acceleration) || !std::isfinite(horizon) ||
    !std::isfinite(step) || max_position_error <= 0 || max_velocity_error <= 0 ||
    max_speed <= 0 || max_acceleration <= 0 || horizon <= 0 || step <= 0) {return false;}
  time = std::clamp(time, 0.0, trajectory.duration());
  const auto first = trajectory.sample(time);
  if ((first.position - position).norm() > max_position_error ||
    (first.velocity - velocity).norm() > max_velocity_error) {return false;}
  const double end = std::min(trajectory.duration(), time + horizon);
  for (double t = time;;) {
    const auto state = trajectory.sample(t);
    if (!state.position.allFinite() || !state.velocity.allFinite() || !state.acceleration.allFinite() ||
      state.velocity.norm() > max_speed + 1e-3 || state.acceleration.norm() > max_acceleration + 1e-3)
    {return false;}
    if (t >= end) {break;}
    t = std::min(end, t + step);
  }
  return true;
}

inline bool similar_trajectories(const TimedTrajectory & old_trajectory, double old_time,
  const TimedTrajectory & candidate, double candidate_time,
  double horizon, double position_tolerance, double velocity_tolerance)
{
  // Do not suppress a candidate if the old trajectory is about to run out.
  if (old_trajectory.duration() - old_time < horizon ||
    candidate.duration() - candidate_time < horizon) {return false;}
  for (double t = 0; t <= horizon + 1e-9; t += 0.1) {
    const auto a = old_trajectory.sample(old_time + t), b = candidate.sample(candidate_time + t);
    if ((a.position-b.position).norm() > position_tolerance ||
      (a.velocity-b.velocity).norm() > velocity_tolerance) {return false;}
  }
  return true;
}
}  // namespace sentry_nav
