#include "sentry_omni_kino_planner/planner.hpp"
#include "kinodynamicAstar.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace sentry_omni_kino_planner {

void PlannerConfig::validate() const {
  for (double v : {max_axis_velocity, max_speed, max_axis_acceleration, max_acceleration,
      max_primitive_duration, velocity_resolution, time_weight, heuristic_weight,
      max_sample_distance, goal_connection_distance, timeout_seconds})
  {
    if (!std::isfinite(v) || v <= 0) {
      throw std::invalid_argument("planner limits and resolutions must be finite and positive");
    }
  }
  if (max_nodes == 0 || max_nodes > 1000000 || timeout_seconds > 300 ||
    max_axis_velocity / velocity_resolution >= std::numeric_limits<int>::max())
  {
    throw std::invalid_argument("planner budget or velocity discretization is out of range");
  }
}

PlanResult plan(
  const PreparedMap & map, const PlanRequest & request, const PlannerConfig & config,
  const std::function<bool()> & canceled)
{
  const auto begin = std::chrono::steady_clock::now();
  PlanResult result;
  auto finish = [&]() {
      result.elapsed_ms = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - begin).count();
      return result;
    };
  try {
    config.validate();
    if (!request.goal_velocity.isZero() || !request.goal_acceleration.isZero()) {
      result.reason = "kino_nonzero_terminal_state_unsupported"; return finish();
    }
    map.source.validate();
    if (map.blocked.size() != map.source.occupancy.size()) {
      throw std::invalid_argument("prepared map size mismatch");
    }
    if (!request.start.allFinite() || !request.goal.allFinite() ||
      !request.velocity.allFinite() || !request.acceleration.allFinite() ||
      !std::isfinite(request.fixed_yaw))
    {
      result.reason = "nonfinite_request";
      return finish();
    }
    if (!map.source.cell(request.start)) {result.reason = "start_outside_map"; return finish();}
    if (!map.source.cell(request.goal)) {result.reason = "goal_outside_map"; return finish();}
    if (map.is_blocked(request.start)) {result.reason = "start_in_obstacle"; return finish();}
    if (map.is_blocked(request.goal)) {result.reason = "goal_in_obstacle"; return finish();}
    if (request.velocity.cwiseAbs().maxCoeff() > config.max_axis_velocity ||
      request.velocity.norm() > config.max_speed ||
      request.acceleration.cwiseAbs().maxCoeff() > config.max_axis_acceleration ||
      request.acceleration.norm() > config.max_acceleration)
    {
      result.reason = "initial_state_exceeds_limits";
      return finish();
    }
    if (canceled && canceled()) {result.reason = "canceled"; return finish();}
    const auto & m = map.source;
    // The TDT occupancy overload treats every nonzero byte as blocked. Using
    // it avoids building an unused YAstar potential-field stencil, which can
    // be enormous at small resolutions. Rebase to zero so float coordinates in
    // upstream collision code do not lose precision at large/negative origins.
    std::vector<int8_t> binary(map.blocked.begin(), map.blocked.end());
    KinodynamicAstar search;
    search.setMap(m.width, m.height, static_cast<float>(m.resolution), 0.0f, 0.0f, binary);
    auto & c = search.config;
    c.maxVel = config.max_axis_velocity;
    c.maxSpeed = config.max_speed;
    c.maxAcc = config.max_axis_acceleration;
    c.maxAcceleration = config.max_acceleration;
    c.maxTau = config.max_primitive_duration;
    c.velResolution = config.velocity_resolution;
    c.timeWeight = config.time_weight;
    c.heuristicWeight = config.heuristic_weight;
    c.sampleTime = std::min(config.max_sample_distance, m.resolution * 0.5) / config.max_speed;
    c.goalConnectionDistance = config.goal_connection_distance;
    c.timeoutSeconds = config.timeout_seconds;
    c.maxNodes = config.max_nodes;
    c.canceled = canceled;
    const auto raw = search.search(request.start - m.origin,
      request.velocity, request.acceleration, request.goal - m.origin);
    result.success = raw.success;
    result.reason = raw.reason;
    result.nodes = raw.nodes;
    result.expansions = raw.iterations;
    if (raw.success) {
      result.points.reserve(raw.trajectory.size());
      result.sample_times.reserve(raw.trajectory.size());
      for (const auto & sample : raw.trajectory) {
        result.sample_times.push_back(sample.time);
        result.points.push_back({sample.state.head<2>() + m.origin,
          sample.state.tail<2>(), sample.acceleration});
        if (result.points.size() > 1) {
          result.length += (result.points.back().position -
            result.points[result.points.size() - 2].position).norm();
        }
      }
    }
  } catch (const std::exception & error) {
    result.success = false;
    result.points.clear();
    result.reason = std::string("invalid_input: ") + error.what();
  }
  return finish();
}

}  // namespace sentry_omni_kino_planner
