#pragma once
#include <algorithm>

namespace sentry_nav {
// Sample age is measured at command publication; extra_delay models only the
// remaining command/actuator delay. Gain permits under/over prediction.
inline double predict_output_yaw(double sampled_yaw, double angular_velocity,
  double sample_age, double extra_delay, double gain)
{
  return sampled_yaw + angular_velocity * (std::max(0.0, sample_age) + extra_delay) * gain;
}
}
