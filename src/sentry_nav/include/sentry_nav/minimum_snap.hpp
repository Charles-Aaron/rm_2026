#pragma once
#include "sentry_nav/trajectory.hpp"
#include "sentry_omni_kino_planner/astar.hpp"
#include <array>
namespace sentry_nav {
struct SnapConfig {
  double timeout_seconds = .2;
  double corridor_range = .4;
  int collision_iterations = 6;
  int max_segments = 80;
  bool constrain_goal_velocity = true;
  void validate() const;
};
struct SnapResult {
  bool success = false;
  std::string reason = "not_run";
  std::vector<QuinticSegment> segments;
  // Last attempted solver corridors (xmin, ymin, xmax, ymax), including failures.
  std::vector<std::array<float, 4>> corridors;
  double elapsed_ms = 0;
};
// TDT defaults: degree five, integrated squared jerk, C2 joins. Endpoint
// positions and initial velocity are fixed. Goal velocity is optional;
// endpoint accelerations are unconstrained.
SnapResult optimize_snap(const core::PreparedMap &, const core::PlanRequest &,
  const core::PlannerConfig &, const core::PlanResult &, const SnapConfig &,
  const std::function<bool()> & canceled = {});
}
