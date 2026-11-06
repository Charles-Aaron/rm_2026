#pragma once
#include <algorithm>
#include <cmath>
#include <chrono>
#include <optional>

namespace sentry_nav {
// Hold zero once low-speed stopping fits inside the goal tolerance.
// Only a sustained position excursion releases the hold; unsafe data must not
// restart motion. The controller's state-validity checks still take precedence.
class GoalSettler {
public:
  using Clock = std::chrono::steady_clock;
  bool update(double distance, double tolerance, double speed, double command_speed,
    double stopping_distance, double entry_speed, bool safe,
    Clock::time_point now = Clock::now())
  {
    if (active_) {
      if (std::isfinite(distance) && std::isfinite(tolerance) && tolerance > 0 && distance > tolerance) {
        if (!outside_since_ || now < *outside_since_) {outside_since_ = now;}
        if (now - *outside_since_ >= std::chrono::milliseconds(200)) {reset();}
      } else {outside_since_.reset();}
      return active_;
    }
    if (!safe || !std::isfinite(distance) || !std::isfinite(tolerance) || tolerance <= 0 ||
      !std::isfinite(speed) || !std::isfinite(command_speed) || !std::isfinite(stopping_distance))
    {return false;}
    if (std::max(speed, command_speed) <= entry_speed &&
      distance + stopping_distance <= tolerance) {
      active_ = true;
    }
    return active_;
  }
  void reset() {active_ = false; outside_since_.reset();}
  bool active() const {return active_;}
private:
  bool active_ = false;
  std::optional<Clock::time_point> outside_since_;
};
}  // namespace sentry_nav
