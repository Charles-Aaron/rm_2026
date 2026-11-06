#pragma once
#include "sentry_omni_kino_planner/planner.hpp"
namespace sentry_nav {
namespace core = sentry_omni_kino_planner;
struct QuinticSegment {
  double duration = 0;
  // Physical seconds, ascending powers: p(t) = sum coefficients[k] * t^k.
  Eigen::Matrix<double, 6, 2> coefficients = Eigen::Matrix<double, 6, 2>::Zero();
};
struct TrajectoryStats {
  double duration = 0, length = 0, average_speed = 0, max_speed = 0;
};
struct TimedTrajectory {
  std::vector<QuinticSegment> segments;
  double duration() const;
  TrajectoryStats stats(double step = .025) const;
  core::PathPoint sample(double time) const;
  core::PathPoint reference(double time) const;
  double nearest_time(const Eigen::Vector2d & position, double begin, double end) const;
};
}
