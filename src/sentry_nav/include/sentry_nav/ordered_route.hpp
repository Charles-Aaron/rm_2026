#pragma once
#include "sentry_nav/local_target.hpp"
#include <chrono>
#include <stdexcept>

namespace sentry_nav {
namespace ordered {
using Point = Eigen::Vector2d;
namespace core = sentry_omni_kino_planner;

// Match exact global planner segment ends in order, including repeated positions.
inline std::vector<size_t> waypoint_indices(
  const std::vector<Point> & route, const std::vector<Point> & goals)
{
  std::vector<size_t> indices;
  size_t at = 0;
  for (const auto & goal : goals) {
    while (at < route.size() && (route[at] - goal).norm() > 1e-4) {++at;}
    if (at == route.size()) {throw std::invalid_argument("global path omits an ordered waypoint");}
    indices.push_back(at);
    // Keep identical consecutive task goals at the same vertex.
  }
  return indices;
}

struct LocalPlan {
  Point target = Point::Zero(), direction = Point::Zero();
  std::vector<Point> via;
  bool final = false;
};

// Projection is confined to the current leg. Lookahead may cross its end only
// by retaining every intervening mandatory vertex in the optimization request.
inline LocalPlan local_plan(const std::vector<Point> & route,
  const std::vector<size_t> & indices, size_t active, const Point & p,
  const core::PreparedMap & map, double lookahead)
{
  LocalPlan out; out.target = p;
  if (route.empty() || active >= indices.size()) {return out;}
  const size_t begin = active == 0 ? 0 : indices[active - 1];
  const size_t end = indices[active];
  size_t closest = begin;
  double fraction = 0, distance = std::numeric_limits<double>::infinity();
  for (size_t i = begin; i < end; ++i) {
    const Point delta = route[i + 1] - route[i];
    const double f = std::clamp((p - route[i]).dot(delta) / std::max(1e-12, delta.squaredNorm()), 0., 1.);
    const double d = (p - route[i] - f * delta).squaredNorm();
    if (d < distance) {distance = d; closest = i; fraction = f;}
  }
  Point cursor = closest < end ? route[closest] + fraction * (route[closest + 1] - route[closest]) : route[end];
  std::vector<Point> passed;
  size_t next = active;
  // A projection onto the current mandatory vertex must not discard it.
  while (next + 1 < indices.size() && indices[next] <= closest) {
    if (map.is_blocked(route[indices[next]])) {return out;}
    passed.push_back(route[indices[next++]]);
  }
  double remaining = lookahead;
  for (size_t i = closest + 1; i < route.size() && remaining > 1e-8; ++i) {
    const Point delta = route[i] - cursor;
    const double length = delta.norm();
    const bool mandatory = next + 1 < indices.size() && indices[next] == i;
    if (length > 1e-8) {
      const Point direction = delta / length;
      const double travel = std::min(length, remaining);
      for (double step = std::min(0.1, travel);; step = std::min(step + 0.1, travel)) {
        const Point test = cursor + step * direction;
        if (!map.is_blocked(test)) {
          out.target = test; out.direction = direction; out.via = passed;
          out.final = i + 1 == route.size() && step >= length - 1e-8;
        }
        if (step >= travel - 1e-8) {break;}
      }
      remaining -= travel;
      if (travel < length - 1e-8) {break;}
    }
    // A distant waypoint may be outside the rolling map. Approach the last
    // free point before it, but never continue on the following task leg.
    if (mandatory && map.is_blocked(route[i])) {break;}
    while (next + 1 < indices.size() && indices[next] == i) {
      passed.push_back(route[i]); ++next;
    }
    cursor = route[i];
  }
  return out;
}

// All legs share a single A* time budget. TDT MinimumSnap subsequently optimizes their
// union, with position-only interpolation constraints at mandatory joins.
inline core::PlanResult seed(const core::PreparedMap & map, const core::PlanRequest & request,
  const core::PlannerConfig & limits, const std::vector<Point> & via,
  const std::function<bool()> & canceled = {})
{
  if (via.empty()) {return core::astar_seed(map, request, limits, canceled);}
  const auto begin = std::chrono::steady_clock::now();
  core::PlanResult out;
  core::PlanRequest leg = request;
  std::vector<Point> stops = via; stops.push_back(request.goal);
  for (size_t i = 0; i < stops.size(); ++i) {
    const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count();
    if (elapsed >= limits.timeout_seconds || (canceled && canceled())) {
      out.reason = "canceled_or_timeout"; return out;
    }
    auto budget = limits; budget.timeout_seconds -= elapsed;
    leg.goal = stops[i];
    leg.goal_velocity = i + 1 == stops.size() ? request.goal_velocity : Point::Zero();
    leg.goal_acceleration = i + 1 == stops.size() ? request.goal_acceleration : Point::Zero();
    const auto part = core::astar_seed(map, leg, budget, canceled);
    if (!part.success) {out.reason = part.reason; return out;}
    const double offset = out.sample_times.empty() ? 0 : out.sample_times.back();
    for (size_t j = out.points.empty() ? 0 : 1; j < part.points.size(); ++j) {
      out.points.push_back(part.points[j]); out.sample_times.push_back(offset + part.sample_times[j]);
    }
    if (i + 1 < stops.size() && !out.points.empty()) {out.mandatory_indices.push_back(out.points.size() - 1);}
    out.length += part.length; out.nodes += part.nodes; out.expansions += part.expansions;
    leg.start = leg.goal; leg.velocity.setZero(); leg.acceleration.setZero();
  }
  out.success = true; out.reason = "ordered_geometric_seed";
  out.elapsed_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
  return out;
}
}  // namespace ordered
}  // namespace sentry_nav
