#include <cmath>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "odometry_conversion.hpp"

class OdomFrameAdapter : public rclcpp::Node
{
public:
  OdomFrameAdapter() : Node("odom_frame_adapter")
  {
    const auto input = declare_parameter<std::string>("input_topic", "imu_odom");
    const auto output = declare_parameter<std::string>("output_topic", "chassis_odometry");
    input_frame_ = declare_parameter<std::string>("input_frame", "imu_odom");
    output_frame_ = declare_parameter<std::string>("output_frame", "odom");
    source_frame_ = declare_parameter<std::string>("source_frame", "imu_link");
    target_frame_ = declare_parameter<std::string>("target_frame", "chassis_link");
    const double timeout = declare_parameter<double>("lookup_timeout", 0.05);
    const auto depth = declare_parameter<int>("queue_depth", 1);
    if (!std::isfinite(timeout) || timeout < 0.0 || depth <= 0) {
      throw std::invalid_argument("lookup_timeout must be non-negative and queue_depth must be positive");
    }
    if (input_frame_.empty() || output_frame_.empty() || source_frame_.empty() || target_frame_.empty()) {
      throw std::invalid_argument("odometry frame names must be nonempty");
    }
    lookup_timeout_ = rclcpp::Duration::from_seconds(timeout);
    buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
    // A dedicated TF callback group/thread can receive transforms while convert() waits.
    // Passing this node also preserves /tf and /tf_static launch remappings.
    listener_ = std::make_unique<tf2_ros::TransformListener>(*buffer_, this, true);
    // Control needs the newest state, not reliable replay of every point update.
    // Keep the output reliable for existing Nav2 subscribers.
    publisher_ = create_publisher<nav_msgs::msg::Odometry>(output, rclcpp::QoS(depth));
    subscription_ = create_subscription<nav_msgs::msg::Odometry>(
      input, rclcpp::SensorDataQoS().keep_last(depth),
      [this](nav_msgs::msg::Odometry::ConstSharedPtr message) {convert(*message);});
    RCLCPP_INFO(
      get_logger(), "Converting %s (%s -> %s) to %s (%s -> %s)",
      input.c_str(), input_frame_.c_str(), source_frame_.c_str(),
      output.c_str(), output_frame_.c_str(), target_frame_.c_str());
  }

private:
  void convert(const nav_msgs::msg::Odometry & message)
  {
    const auto & child = message.child_frame_id.empty() ? source_frame_ : message.child_frame_id;
    if (message.header.frame_id != input_frame_ || child != source_frame_) {
      RCLCPP_WARN_THROTTLE(
        get_logger(), *get_clock(), 2000, "Expected %s -> %s, got %s -> %s; dropping odometry",
        input_frame_.c_str(), source_frame_.c_str(), message.header.frame_id.c_str(), child.c_str());
      return;
    }

    if (!converter_) {
      try {
        // 两段固定 TF 全部就绪后才安装转换器，避免输出半转换的位姿。
        const auto source_from_target = buffer_->lookupTransform(
          source_frame_, target_frame_,
          rclcpp::Time(0, 0, get_clock()->get_clock_type()), lookup_timeout_).transform;
        const auto output_from_input = buffer_->lookupTransform(
          output_frame_, input_frame_,
          rclcpp::Time(0, 0, get_clock()->get_clock_type()), lookup_timeout_).transform;
        converter_.emplace(output_from_input, source_from_target, output_frame_, target_frame_);
      } catch (const tf2::TransformException & error) {
        RCLCPP_WARN_THROTTLE(
          get_logger(), *get_clock(), 2000, "Waiting for fixed odometry transforms: %s", error.what());
        return;
      } catch (const std::invalid_argument & error) {
        RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000, "Invalid fixed transform: %s", error.what());
        return;
      }
    }

    try {
      publisher_->publish(converter_->convert(message));
    } catch (const std::invalid_argument & error) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000, "Dropping odometry: %s", error.what());
    }
  }

  std::string input_frame_, output_frame_, source_frame_, target_frame_;
  rclcpp::Duration lookup_timeout_{0, 0};
  std::unique_ptr<tf2_ros::Buffer> buffer_;
  std::unique_ptr<tf2_ros::TransformListener> listener_;
  std::optional<odom_frame_adapter::OdometryConverter> converter_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int result = 0;
  try {
    rclcpp::spin(std::make_shared<OdomFrameAdapter>());
  } catch (const std::exception & error) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("odom_frame_adapter"), "%s", error.what());
      result = 1;
    }
  }
  rclcpp::shutdown();
  return result;
}
