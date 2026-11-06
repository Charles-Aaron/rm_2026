#pragma once
#include "sentry_omni_kino_planner/planner.hpp"

namespace sentry_omni_kino_planner {
// Geometric seed only; timestamps are not a certified execution schedule.
PlanResult astar_seed(const PreparedMap & map, const PlanRequest & request,
  const PlannerConfig & limits, const std::function<bool()> & canceled = {});
bool line_free(const PreparedMap & map, const Eigen::Vector2d & a, const Eigen::Vector2d & b);
}
