#include "sentry_omni_kino_planner/astar.hpp"
#include "yastar.hpp"
#include <chrono>
#include <cmath>

namespace sentry_omni_kino_planner {
bool line_free(const PreparedMap & map, const Eigen::Vector2d & a, const Eigen::Vector2d & b) {
  if (!a.allFinite() || !b.allFinite() || map.is_blocked(a) || map.is_blocked(b)) {return false;}
  // Supercover: test both adjacent cells at exact corner crossings.
  auto ca = *map.source.cell(a), cb = *map.source.cell(b);
  const Eigen::Vector2d q = (a - map.source.origin) / map.source.resolution;
  const Eigen::Vector2d d = (b - a) / map.source.resolution;
  const int sx = d.x() > 0 ? 1 : -1, sy = d.y() > 0 ? 1 : -1;
  const double inf = std::numeric_limits<double>::infinity();
  double tx = d.x() == 0 ? inf : ((ca.x() + (sx > 0 ? 1 : 0)) - q.x()) / d.x();
  double ty = d.y() == 0 ? inf : ((ca.y() + (sy > 0 ? 1 : 0)) - q.y()) / d.y();
  const double dx = d.x() == 0 ? inf : std::abs(1 / d.x());
  const double dy = d.y() == 0 ? inf : std::abs(1 / d.y());
  auto blocked = [&](int x, int y) {
    return x < 0 || y < 0 || x >= map.source.width || y >= map.source.height ||
      map.blocked[map.source.index(x, y)] != 0;
  };
  while (ca != cb) {
    // Tiny numerical displacements at a grid boundary must not step past the
    // endpoint cell on an axis already complete.
    if (ca.x() == cb.x()) {tx = inf;}
    if (ca.y() == cb.y()) {ty = inf;}
    if (std::abs(tx - ty) < 1e-12) {
      if (blocked(ca.x() + sx, ca.y()) || blocked(ca.x(), ca.y() + sy)) {return false;}
      ca.x() += sx; ca.y() += sy; tx += dx; ty += dy;
    } else if (tx < ty) {ca.x() += sx; tx += dx;}
    else {ca.y() += sy; ty += dy;}
    if (blocked(ca.x(), ca.y())) {return false;}
  }
  return true;
}

PlanResult astar_seed(const PreparedMap & map, const PlanRequest & request,
  const PlannerConfig & limits, const std::function<bool()> & canceled)
{
  const auto begin = std::chrono::steady_clock::now();
  PlanResult out;
  auto finish = [&](const std::string & reason) {
    out.reason = reason;
    out.elapsed_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
    return out;
  };
  limits.validate(); map.source.validate();
  if (map.blocked.size() != map.source.occupancy.size()) {return finish("invalid_map");}
  if (!request.start.allFinite() || !request.goal.allFinite() ||
    !request.velocity.allFinite() || !request.acceleration.allFinite() ||
    !request.goal_velocity.allFinite() || !request.goal_acceleration.allFinite())
  {return finish("nonfinite_request");}
  if (map.is_blocked(request.start)) {return finish("start_blocked");}
  if (map.is_blocked(request.goal)) {return finish("goal_blocked");}
  auto exceeds = [&](const Eigen::Vector2d & v, double axis, double norm) {
    return v.cwiseAbs().maxCoeff() > axis || v.norm() > norm;
  };
  if (exceeds(request.velocity, limits.max_axis_velocity, limits.max_speed) ||
    exceeds(request.goal_velocity, limits.max_axis_velocity, limits.max_speed) ||
    exceeds(request.acceleration, limits.max_axis_acceleration, limits.max_acceleration) ||
    exceeds(request.goal_acceleration, limits.max_axis_acceleration, limits.max_acceleration))
  {return finish("boundary_exceeds_limits");}
  auto stopped = [&]() {
    return (canceled && canceled()) ||
      std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count() >= limits.timeout_seconds;
  };
  if (stopped()) {return finish("canceled_or_timeout");}
  std::vector<Eigen::Vector2d> raw;
  if (line_free(map, request.start, request.goal)) {raw = {request.start, request.goal};}
  else {
    YAstar search;
    auto binary = std::vector<int8_t>(map.blocked.begin(), map.blocked.end());
    search.setMap(map.source.width, map.source.height, static_cast<float>(map.source.resolution), 0.f, 0.f, binary);
    search.setCostField(0.5f, [](float distance) {return 0.3f / (0.1f + distance);});
    search.setCostWeight(1.f);
    search.searchCanceled = stopped;
    auto path = search.search((request.start - map.source.origin).cast<float>(),
      (request.goal - map.source.origin).cast<float>());
    if (path.size() < 2) {return finish(stopped() ? "canceled_or_timeout" : "no_path");}
    raw.push_back(request.start);
    for (const auto & p : path) {raw.push_back(p.cast<double>() + map.source.origin + Eigen::Vector2d::Constant(map.source.resolution * 0.5));}
    raw.push_back(request.goal);
  }
  for (size_t i = 1; i < raw.size(); ++i) {
    if (!line_free(map, raw[i - 1], raw[i])) {return finish("seed_collision");}
  }
  std::vector<Eigen::Vector2d> reduced{raw.front()};
  for (size_t at = 0; at + 1 < raw.size();) {
    if (stopped()) {return finish("canceled_or_timeout");}
    size_t next = at + 1;
    // Bounded lookahead, preserving collision-free edges.
    for (size_t i = next + 1; i < raw.size() && i <= at + 40; ++i) {
      if (line_free(map, raw[at], raw[i])) {next = i;}
    }
    if ((raw[next] - reduced.back()).norm() > 1e-8) {reduced.push_back(raw[next]);}
    at = next;
  }
  if (reduced.size() == 1) {reduced.push_back(request.goal);}
  double time = 0;
  for (size_t i = 0; i < reduced.size(); ++i) {
    if (i > 0) {
      const double distance = (reduced[i] - reduced[i - 1]).norm();
      out.length += distance;
      time += std::max({0.2, distance / (0.6 * limits.max_speed),
        2 * std::sqrt(distance / limits.max_acceleration)});
    }
    out.points.push_back({reduced[i], Eigen::Vector2d::Zero(), Eigen::Vector2d::Zero()});
    out.sample_times.push_back(time);
  }
  out.points.front().velocity = request.velocity;
  out.points.front().acceleration = request.acceleration;
  out.points.back().velocity = request.goal_velocity;
  out.points.back().acceleration = request.goal_acceleration;
  out.success = true;
  return finish("geometric_seed");
}
}
