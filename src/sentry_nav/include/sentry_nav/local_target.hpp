#pragma once
#include "sentry_omni_kino_planner/astar.hpp"
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

namespace sentry_nav {
// Select a free endpoint along the route; connecting it is the local A* task.
inline std::pair<Eigen::Vector2d, Eigen::Vector2d> local_target(
  const std::vector<Eigen::Vector2d> & route, const Eigen::Vector2d & p,
  const sentry_omni_kino_planner::PreparedMap & map, double lookahead)
{
  if (route.empty()) {return {p, Eigen::Vector2d::Zero()};}
  // Project onto segments, since global paths may contain only a few waypoints.
  size_t closest = 0; double fraction = 0, distance = std::numeric_limits<double>::infinity();
  for (size_t i = 0; i + 1 < route.size(); ++i) {
    const Eigen::Vector2d delta = route[i + 1] - route[i];
    const double f = std::clamp((p - route[i]).dot(delta) / std::max(delta.squaredNorm(), 1e-12), 0.0, 1.0);
    const double norm = (p - (route[i] + f * delta)).squaredNorm();
    if (norm < distance) {distance = norm; closest = i; fraction = f;}
  }
  Eigen::Vector2d cursor = route.size() == 1 ? route.front() : route[closest] + fraction * (route[closest + 1] - route[closest]);
  Eigen::Vector2d target = cursor, direction = Eigen::Vector2d::Zero();
  double remaining = lookahead;
  for (size_t i = closest + 1; i < route.size() && remaining > 0; ++i) {
    const Eigen::Vector2d delta = route[i] - cursor; const double length = delta.norm();
    if (length < 1e-8) {cursor = route[i]; continue;}
    const Eigen::Vector2d tangent = delta / length;
    const double travel = std::min(length, remaining);
    for (double step = std::min(0.1, travel); step <= travel + 1e-8; step = std::min(step + 0.1, travel)) {
      const Eigen::Vector2d test = cursor + step * tangent;
      // Obstacles on the global route must not truncate the local goal.
      // A* will decide whether a free point beyond them is reachable.
      if (!map.is_blocked(test)) {target = test; direction = tangent;}
      if (step >= travel - 1e-8) {break;}
    }
    remaining -= travel; cursor = route[i];
  }
  return {target, direction};
}
}  // namespace sentry_nav
