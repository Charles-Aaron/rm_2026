#pragma once
#include "sentry_nav/trajectory.hpp"
#include <cmath>

namespace sentry_nav {
// Only freshness and representation validity gate adoption; no state-error,
// collision or motion-limit acceptance test is applied here.
inline bool handover_ready(const TimedTrajectory & trajectory,
  double elapsed, double age, double max_age)
{
  if (trajectory.segments.empty() || !std::isfinite(elapsed) || !std::isfinite(age) ||
    elapsed < 0 || age < 0 || age > max_age || elapsed > max_age) {return false;}
  for (const auto & segment : trajectory.segments) {
    if (!std::isfinite(segment.duration) || segment.duration <= 0 ||
      !segment.coefficients.allFinite()) {return false;}
  }
  return std::isfinite(trajectory.duration());
}
}
