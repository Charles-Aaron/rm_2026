#pragma once

#include <cstdint>
#include <set>

namespace sentry_terrain {
// 调用方持锁；旧实例、旧代次、重复帧均不能让地图重新变为有效。
struct InputGate {
  uint64_t instance{0}, epoch{0}, sequence{0};
  bool seen{false}, failed{true};
  double stamp{-1};
  std::set<uint64_t> retired;

  bool session(uint64_t id, uint64_t generation, bool failure) {
    if (!id || retired.count(id) || (seen && id == instance && generation < epoch)) return false;
    const bool changed = !seen || id != instance || generation != epoch;
    const bool new_failure = failure && !failed;
    if (changed) {
      if (seen && id != instance) retired.insert(instance);
      instance = id; epoch = generation; seen = true; failed = failure;
      sequence = 0; stamp = -1;
    } else {
      failed = failed || failure;
    }
    return changed || new_failure;
  }
  bool accept(uint64_t id, uint64_t generation, uint64_t seq, double time) {
    if (!seen || failed || instance != id || epoch != generation || seq <= sequence || time <= stamp)
      return false;
    sequence = seq; stamp = time;
    return true;
  }
  void rewind() {sequence = 0; stamp = -1;}
};
}  // namespace sentry_terrain
