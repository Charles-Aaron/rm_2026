#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "cloud_conversion.hpp"

class CloudFrameAdapter : public rclcpp::Node
{
public:
  CloudFrameAdapter() : Node("cloud_frame_adapter")
  {
    const auto input = declare_parameter<std::string>("input_topic", "cloud_registered");
    const auto output = declare_parameter<std::string>("output_topic", "registered_cloud");
    source_frame_ = declare_parameter<std::string>("source_frame", "imu_odom");
    target_frame_ = declare_parameter<std::string>("target_frame", "odom");
    const double timeout = declare_parameter<double>("lookup_timeout", 0.0);
    if (!std::isfinite(timeout) || timeout < 0.0 || source_frame_.empty() ||
      target_frame_.empty() || source_frame_ == target_frame_)
    {
      throw std::invalid_argument("Frames must be distinct and nonempty; lookup_timeout must be finite and non-negative");
    }
    lookup_timeout_ = rclcpp::Duration::from_seconds(timeout);
    buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
    listener_ = std::make_unique<tf2_ros::TransformListener>(*buffer_, this, true);
    // Latest-cloud processing; input accepts both reliable and best-effort publishers.
    subscription_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      input, rclcpp::SensorDataQoS().keep_last(1),
      [this](sensor_msgs::msg::PointCloud2::UniquePtr message) {convert(std::move(message));});
    publisher_ = create_publisher<sensor_msgs::msg::PointCloud2>(output, rclcpp::QoS(1));
    RCLCPP_INFO(
      get_logger(), "Converting %s (%s) -> %s (%s) with a cached fixed transform",
      input.c_str(), source_frame_.c_str(), output.c_str(), target_frame_.c_str());
  }

private:
  void convert(sensor_msgs::msg::PointCloud2::UniquePtr message)
  {
    if (message->header.frame_id != source_frame_) {
      RCLCPP_ERROR_THROTTLE(
        get_logger(), *get_clock(), 2000, "Expected cloud frame '%s', got '%s'; dropping cloud",
        source_frame_.c_str(), message->header.frame_id.c_str());
      return;
    }
    if (!fixed_matrix_) {
      try {
        // This node is only for fixed frame pairs. Resolve target <- source once.
        const auto transform = buffer_->lookupTransform(
          target_frame_, source_frame_, rclcpp::Time(0, 0, get_clock()->get_clock_type()),
          lookup_timeout_);
        fixed_matrix_ = cloud_frame_adapter::fixed_matrix(transform.transform);
        RCLCPP_INFO(get_logger(), "Cached fixed transform %s <- %s",
          target_frame_.c_str(), source_frame_.c_str());
      } catch (const std::exception & error) {
        RCLCPP_WARN_THROTTLE(
          get_logger(), *get_clock(), 2000, "Waiting for fixed cloud transform: %s", error.what());
        return;
      }
    }
    try {
      cloud_frame_adapter::convert_cloud(*message, *fixed_matrix_, target_frame_);
    } catch (const std::invalid_argument & error) {
      RCLCPP_ERROR_THROTTLE(
        get_logger(), *get_clock(), 2000, "Dropping malformed point cloud: %s", error.what());
      return;
    }
    publisher_->publish(std::move(message));
  }

  std::string source_frame_, target_frame_;
  rclcpp::Duration lookup_timeout_{0, 0};
  std::unique_ptr<tf2_ros::Buffer> buffer_;
  std::unique_ptr<tf2_ros::TransformListener> listener_;
  std::optional<tf2::Transform> fixed_matrix_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int result = 0;
  try {
    rclcpp::spin(std::make_shared<CloudFrameAdapter>());
  } catch (const std::exception & error) {
    if (rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("cloud_frame_adapter"), "%s", error.what());
      result = 1;
    }
  }
  rclcpp::shutdown();
  return result;
}
