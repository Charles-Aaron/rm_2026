#pragma once

#include "sentry_omni_kino_planner/grid_map.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <utility>

namespace sentry_nav {
struct CollisionMapSnapshot {
  using Clock = std::chrono::steady_clock;
  std::shared_ptr<const sentry_omni_kino_planner::PreparedMap> local, snap;
  Clock::time_point captured_at;
  int64_t stamp_ns = 0;  // ROS 时钟下的地图复制时间，不是雷达采样时间。
  uint64_t version = 0;

  bool fresh(double timeout, Clock::time_point now = Clock::now()) const {
    const double age = std::chrono::duration<double>(now - captured_at).count();
    return age >= 0 && age <= timeout;
  }
};

// 单生产者、多读取者：读者持有整份不可变快照，地图换代不会改变在途规划。
class CollisionMapCache {
  using CoreMap = sentry_omni_kino_planner::GridMap;
  using PreparedMap = sentry_omni_kino_planner::PreparedMap;
  double local_radius_, snap_radius_;
  bool unknown_;
  uint64_t version_ = 0;
  std::shared_ptr<const CollisionMapSnapshot> latest_;

  static bool same(const CoreMap & a, const CoreMap & b) {
    return a.width == b.width && a.height == b.height && a.resolution == b.resolution &&
      a.origin == b.origin && a.frame == b.frame && a.occupancy == b.occupancy &&
      a.traversal_cost == b.traversal_cost;
  }

public:
  CollisionMapCache(double local_radius, double snap_radius, bool unknown)
  : local_radius_(local_radius), snap_radius_(snap_radius), unknown_(unknown) {}

  void update(CoreMap raw, int64_t stamp_ns,
    CollisionMapSnapshot::Clock::time_point captured_at = CollisionMapSnapshot::Clock::now())
  {
    const auto previous = std::atomic_load(&latest_);
    auto next = std::make_shared<CollisionMapSnapshot>();
    if (previous && same(previous->local->source, raw)) {
      next->local = previous->local; next->snap = previous->snap;
      next->version = previous->version;
    } else {
      next->local = std::make_shared<PreparedMap>(
        sentry_omni_kino_planner::prepare_map(std::move(raw), {65, unknown_, local_radius_}));
      next->snap = snap_radius_ == local_radius_ ? next->local : std::make_shared<PreparedMap>(
        sentry_omni_kino_planner::prepare_map(next->local->source, {65, unknown_, snap_radius_}));
      next->version = ++version_;
    }
    next->captured_at = captured_at; next->stamp_ns = stamp_ns;
    std::atomic_store(&latest_, std::shared_ptr<const CollisionMapSnapshot>(std::move(next)));
  }

  std::shared_ptr<const CollisionMapSnapshot> latest(double timeout,
    CollisionMapSnapshot::Clock::time_point now = CollisionMapSnapshot::Clock::now()) const
  {
    auto value = std::atomic_load(&latest_);
    return value && value->fresh(timeout, now) ? value : nullptr;
  }

  void clear() {std::atomic_store(&latest_, std::shared_ptr<const CollisionMapSnapshot>{});}
};
}  // namespace sentry_nav
