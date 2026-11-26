#pragma once
#include <chrono>
#include <mutex>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>

// Optional for legacy launches/tests. Integrated ZLIO launches require a fresh
// positive heartbeat; a cached TF alone never authorizes movement.
class LocalizationGuard {
public:
  template<class Node> void configure(Node &node) {
    required_ = node.template declare_parameter<bool>("require_localization", false);
    if (!required_) return;
    subscription_ = node.template create_subscription<std_msgs::msg::Bool>(
      "localization_valid", rclcpp::QoS(1),
      [this](std_msgs::msg::Bool::ConstSharedPtr value) {
        std::lock_guard<std::mutex> lock(mutex_);
        valid_ = value->data; received_ = std::chrono::steady_clock::now();
      });
  }
  bool ready() const {
    if (!required_) return true;
    std::lock_guard<std::mutex> lock(mutex_);
    return valid_ && std::chrono::duration<double>(std::chrono::steady_clock::now() - received_).count() <= 0.3;
  }
private:
  bool required_ = false, valid_ = false;
  mutable std::mutex mutex_;
  std::chrono::steady_clock::time_point received_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr subscription_;
};
