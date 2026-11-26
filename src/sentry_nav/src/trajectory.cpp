#include "sentry_nav/trajectory.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace sentry_nav {
double TimedTrajectory::duration() const {
  double out = 0; for (const auto & s : segments) {out += s.duration;} return out;
}
TrajectoryStats TimedTrajectory::stats(double step) const {
  if (!std::isfinite(step) || step <= 0) {throw std::invalid_argument("invalid statistics step");}
  TrajectoryStats out; out.duration = duration();
  if (segments.empty() || !std::isfinite(out.duration) || out.duration <= 0) {
    throw std::invalid_argument("invalid trajectory statistics");
  }
  double offset = 0;
  for (const auto & segment : segments) {
    if (!std::isfinite(segment.duration) || segment.duration <= 0 || !segment.coefficients.allFinite()) {
      throw std::invalid_argument("invalid trajectory segment");
    }
    const int count = std::max(1, static_cast<int>(std::ceil(segment.duration / step)));
    const double dt = segment.duration / count;
    double previous_speed = sample(offset).velocity.norm();
    out.max_speed = std::max(out.max_speed, previous_speed);
    for (int i = 1; i <= count; ++i) {
      const double speed = sample(offset + i * dt).velocity.norm();
      out.length += .5 * (previous_speed + speed) * dt;
      out.max_speed = std::max(out.max_speed, speed); previous_speed = speed;
    }
    offset += segment.duration;
  }
  out.average_speed = out.length / out.duration;
  return out;
}
core::PathPoint TimedTrajectory::sample(double time) const {
  if (segments.empty() || !std::isfinite(time)) {throw std::invalid_argument("invalid trajectory sample");}
  const double total = duration();
  time = std::max(time, 0.0);
  const QuinticSegment * selected = &segments.back();
  if (time >= total) {time = selected->duration;}
  else {
    for (size_t i = 0; i + 1 < segments.size(); ++i) {
      const auto & s = segments[i];
      if (time <= s.duration) {selected = &s; break;}
      time -= s.duration;
    }
  }
  time = std::clamp(time, 0.0, selected->duration);
  core::PathPoint out{Eigen::Vector2d::Zero(), Eigen::Vector2d::Zero(), Eigen::Vector2d::Zero()};
  for (int k = 0; k < 6; ++k) {
    out.position += std::pow(time, k) * selected->coefficients.row(k).transpose();
    if (k >= 1) {out.velocity += k * std::pow(time, k - 1) * selected->coefficients.row(k).transpose();}
    if (k >= 2) {out.acceleration += k * (k - 1) * std::pow(time, k - 2) * selected->coefficients.row(k).transpose();}
  }
  return out;
}
core::PathPoint TimedTrajectory::reference(double time) const {
  auto out = sample(time);
  if (time >= duration()) {out.velocity.setZero(); out.acceleration.setZero();}
  return out;
}
double TimedTrajectory::nearest_time(const Eigen::Vector2d & position, double begin, double end) const {
  if (!position.allFinite() || !std::isfinite(begin) || !std::isfinite(end))
    {throw std::invalid_argument("invalid nearest-point search");}
  begin = std::clamp(begin, 0.0, duration());
  end = std::clamp(end, begin, duration());
  const auto distance = [&](double t) {return (sample(t).position - position).squaredNorm();};
  double best = begin, error = distance(begin);
  const auto consider = [&](double t) {
    const double value = distance(t);
    // Keep the earlier branch when positions are indistinguishable.
    if (value < error - 1e-12) {best = t; error = value;}
  };
  constexpr double step = 0.025;
  for (double t = begin + step; t < end; t += step) {consider(t);}
  consider(end);
  double lo = std::max(begin, best - step), hi = std::min(end, best + step);
  // Refine only around the best coarse sample, always inside the progress window.
  for (int i = 0; i < 24; ++i) {
    const double a = lo + (hi - lo) / 3, b = hi - (hi - lo) / 3;
    if (distance(a) <= distance(b)) {hi = b;} else {lo = a;}
  }
  consider((lo + hi) / 2);
  return best;
}

}
