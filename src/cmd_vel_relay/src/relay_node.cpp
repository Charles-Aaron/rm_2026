#include <memory>
#include <exception>
#include <chrono>
#include <cmath>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

class CmdVelRelay : public rclcpp::Node
{
public:
  CmdVelRelay() : Node("cmd_vel_relay")
  {
    const auto input = declare_parameter<std::string>("input_topic", "/x_sentry/cmd_vel");
    const auto output = declare_parameter<std::string>("output_topic", "/red_standard_robot1/cmd_vel");
    const double timeout = declare_parameter<double>("command_timeout", 0.2);
    if (!std::isfinite(timeout) || timeout <= 0) {throw std::invalid_argument("command_timeout must be positive");}
    const auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).best_effort().durability_volatile();
    publisher_ = create_publisher<geometry_msgs::msg::Twist>(output, qos);
    subscription_ = create_subscription<geometry_msgs::msg::Twist>(
      input, qos, [this](geometry_msgs::msg::Twist::ConstSharedPtr message) {
        if (!std::isfinite(message->linear.x) || !std::isfinite(message->linear.y) ||
          !std::isfinite(message->angular.z)) {publisher_->publish(geometry_msgs::msg::Twist{}); return;}
        last_command_ = std::chrono::steady_clock::now();
        publisher_->publish(*message);
      });
    timer_ = create_wall_timer(std::chrono::milliseconds(25), [this, timeout]() {
      if (std::chrono::duration<double>(std::chrono::steady_clock::now() - last_command_).count() > timeout) {
        publisher_->publish(geometry_msgs::msg::Twist{});
      }
    });
    RCLCPP_INFO(get_logger(), "Relaying %s -> %s", input.c_str(), output.c_str());
  }

private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::chrono::steady_clock::time_point last_command_{};
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int result = 0;
  try {
    rclcpp::spin(std::make_shared<CmdVelRelay>());
  } catch (const std::exception & error) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("cmd_vel_relay"), "%s", error.what());
      result = 1;
    }
  }
  rclcpp::shutdown();
  return result;
}
