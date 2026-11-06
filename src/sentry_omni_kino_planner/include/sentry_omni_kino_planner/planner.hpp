#pragma once

#include <functional>
#include <string>
#include <vector>
#include <Eigen/Core>
#include "sentry_omni_kino_planner/grid_map.hpp"

namespace sentry_omni_kino_planner {

struct PlannerConfig {
  double max_axis_velocity = 2.3;
  double max_speed = 2.3;
  double max_axis_acceleration = 1.8;
  double max_acceleration = 1.8;
  double max_primitive_duration = 0.6;
  double velocity_resolution = 0.3;
  double time_weight = 5.0;
  double heuristic_weight = 3.0;
  double max_sample_distance = 0.04;
  double goal_connection_distance = 1.2;
  double timeout_seconds = 3.0;
  size_t max_nodes = 60000;
  void validate() const;
};

struct PlanRequest {
  Eigen::Vector2d start = Eigen::Vector2d::Zero();
  Eigen::Vector2d velocity = Eigen::Vector2d::Zero();
  Eigen::Vector2d acceleration = Eigen::Vector2d::Zero();
  Eigen::Vector2d goal = Eigen::Vector2d::Zero();
  double fixed_yaw = 0.0;  // Display only; translation is independent of body yaw.
  Eigen::Vector2d goal_velocity = Eigen::Vector2d::Zero();
  Eigen::Vector2d goal_acceleration = Eigen::Vector2d::Zero();
};

struct PathPoint {
  Eigen::Vector2d position;
  Eigen::Vector2d velocity;
  Eigen::Vector2d acceleration;
};

struct PlanResult {
  bool success = false;
  std::string reason;
  std::vector<PathPoint> points;  // Spatial samples, no trajectory time contract.
  // Internal seed metadata for the optimization backend; never published as a schedule.
  std::vector<double> sample_times;
  // Ordered seed vertices that MINCO must interpolate exactly (no velocity constraint).
  std::vector<size_t> mandatory_indices;
  size_t nodes = 0;
  size_t expansions = 0;
  double elapsed_ms = 0.0;
  double length = 0.0;
};

PlanResult plan(
  const PreparedMap & map, const PlanRequest & request, const PlannerConfig & config,
  const std::function<bool()> & canceled = {});

}  // namespace sentry_omni_kino_planner
