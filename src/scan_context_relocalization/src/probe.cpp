#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <localization_interfaces/msg/frontend_frame.hpp>
#include <backend/scan_context.hpp>
#include <backend/search.hpp>
#include "visualization.hpp"

// ROS experiment adapter only. Retrieval, leveling, yaw hypotheses, point-to-plane
// registration, and full-pose acceptance all execute in unmodified upstream code.
class Probe : public rclcpp::Node {
public:
  explicit Probe(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
    : Node("scan_context_probe", options) {
    const auto directory = declare_parameter<std::string>("prior_map", "");
    const auto output = declare_parameter<std::string>("output_csv", "");
    const auto input = declare_parameter<std::string>("frame_topic", "/frontend/frame");
    odom_frame_ = declare_parameter<std::string>("odom_frame", "odom");
    sensor_frame_ = declare_parameter<std::string>("sensor_frame", "imu");
    map_frame_ = declare_parameter<std::string>("map_frame", "experiment_map");
    period_ = declare_parameter<double>("query_interval", 3.0);
    const auto visualize = declare_parameter<bool>("visualization", false);
    const auto map_voxel = declare_parameter<double>("visualization_map_voxel", 0.2);
    if (directory.empty() || odom_frame_.empty() || sensor_frame_.empty() ||
        map_frame_.empty() || !std::isfinite(period_) || period_ <= 0.0) {
      throw std::invalid_argument("prior_map and frame names are required; query_interval must be positive");
    }
    backend::ScanContextDescriptorConfig descriptor_config;
    if (!backend::ScanContextDescriptor::loadConfig(directory, &descriptor_config)) {
      throw std::runtime_error("Cannot load upstream Scan Context configuration from map bundle");
    }
    descriptor_.configure(descriptor_config);
    if (!prior_.load(directory) || !descriptor_.load(directory) || prior_.empty() ||
        prior_.size() != descriptor_.size() || !descriptor_.enabled()) {
      throw std::runtime_error("Invalid or incompatible upstream keyframe/Scan Context map bundle");
    }
    positions_ = prior_.positions();
    if (visualize) {
      if (!std::isfinite(map_voxel) || map_voxel <= 0.0)
        throw std::invalid_argument("visualization_map_voxel must be positive");
      visualization_ = std::make_unique<ProbeVisualization>(*this, prior_, map_frame_, map_voxel);
    }
    // Keep the exact upstream C++ defaults. Do not substitute score/overlap gates.
    if (!output.empty()) {
      if (std::ifstream(output).good()) {
        throw std::runtime_error("output_csv already exists; choose a new result path");
      }
      csv_.open(output);
      if (!csv_) throw std::runtime_error("Cannot open output_csv");
      csv_ << std::setprecision(17)
           << "stamp,instance,epoch,sequence,success,prior_index,covariance_valid,"
              "compute_seconds,x,y,z,qx,qy,qz,qw\n";
    }
    publisher_ = create_publisher<geometry_msgs::msg::PoseStamped>("~/candidate_pose", 10);
    subscription_ = create_subscription<localization_interfaces::msg::FrontendFrame>(
      input, rclcpp::SensorDataQoS().keep_last(1),
      [this](localization_interfaces::msg::FrontendFrame::ConstSharedPtr message) {
        try { process(*message); }
        catch (const std::exception &error) {
          RCLCPP_ERROR(get_logger(), "Relocalization attempt failed: %s", error.what());
        }
      });
    RCLCPP_INFO(get_logger(), "Ready: %zu upstream map keyframes; ZLIO -> z runRelocalization. No TF output.", prior_.size());
  }

private:
  void process(const localization_interfaces::msg::FrontendFrame &message) {
    const auto &odometry = message.odometry;
    if (odometry.header.frame_id != odom_frame_ || odometry.child_frame_id != sensor_frame_ ||
        message.cloud.header.frame_id != sensor_frame_ ||
        message.cloud.header.stamp != odometry.header.stamp) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "Dropping frame with inconsistent frames/timestamps");
      return;
    }
    if (!has_session_ || message.instance_id != instance_ || message.epoch != epoch_) {
      has_session_ = true;
      instance_ = message.instance_id;
      epoch_ = message.epoch;
      sequence_ = 0;
      last_query_ = -1;
      last_stamp_ = -1;
      target_cache_.clear();
      if (visualization_) visualization_->reset(odometry.header);
    }
    const double stamp = rclcpp::Time(odometry.header.stamp).seconds();
    if (message.sequence <= sequence_ || stamp < last_stamp_) return;
    sequence_ = message.sequence;
    last_stamp_ = stamp;
    if (!message.lidar_updated || (last_query_ >= 0 && stamp - last_query_ < period_)) return;
    const auto &p = odometry.pose.pose.position;
    const auto &q = odometry.pose.pose.orientation;
    Eigen::Quaterniond rotation(q.w, q.x, q.y, q.z);
    if (!std::isfinite(rotation.norm()) || rotation.norm() < 1e-6 ||
        !std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) return;

    // ZLIO exports XYZ; upstream PointXYZI uses no intensity in these algorithms.
    pcl::PointCloud<pcl::PointXYZ> xyz;
    pcl::fromROSMsg(message.cloud, xyz);
    auto local = backend::Cloud::Ptr(new backend::Cloud);
    local->reserve(xyz.size());
    for (const auto &point : xyz) {
      if (!std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.z)) continue;
      backend::PointType converted;
      converted.x = point.x; converted.y = point.y; converted.z = point.z;
      converted.intensity = 0;
      local->push_back(converted);
    }
    if (local->empty()) return;
    backend::Keyframe runtime;
    runtime.index = 0;
    runtime.epoch = epoch_;
    runtime.timestamp = stamp;
    runtime.odom_pose.block<3, 3>(0, 0) = rotation.normalized().toRotationMatrix();
    runtime.odom_pose.block<3, 1>(0, 3) = Eigen::Vector3d(p.x, p.y, p.z);
    runtime.cloud = backend::CloudBlock(local);
    last_query_ = stamp;
    const auto start = std::chrono::steady_clock::now();
    backend::RelocalizationRequest request{runtime, config_, positions_};
    // Independent global attempts: no position prior or use of the last successful pose.
    request.search_radius = 0.0;
    const auto result = backend::runRelocalization(prior_, descriptor_, target_cache_, request);
    const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    RCLCPP_INFO(get_logger(), "stamp=%.6f success=%d prior=%d covariance=%d elapsed=%.3fs",
      stamp, result.success, result.prior_index, result.covariance_valid, seconds);
    if (csv_) {
      csv_ << stamp << ',' << instance_ << ',' << epoch_ << ',' << message.sequence << ','
           << result.success << ',' << result.prior_index << ',' << result.covariance_valid << ',' << seconds;
    }
    if (result.success) {
      const Eigen::Matrix4d pose = result.T_map_odom * runtime.odom_pose;
      const Eigen::Quaterniond orientation(pose.block<3, 3>(0, 0));
      geometry_msgs::msg::PoseStamped output;
      output.header = odometry.header;
      output.header.frame_id = map_frame_;
      output.pose.position.x = pose(0, 3); output.pose.position.y = pose(1, 3); output.pose.position.z = pose(2, 3);
      output.pose.orientation.x = orientation.x(); output.pose.orientation.y = orientation.y();
      output.pose.orientation.z = orientation.z(); output.pose.orientation.w = orientation.w();
      publisher_->publish(output);
      if (visualization_) visualization_->show(odometry.header, local.get(), &pose,
        "SUCCESS  prior=" + std::to_string(result.prior_index) +
        "  stamp=" + std::to_string(stamp) + "  seconds=" + std::to_string(seconds), true);
      if (csv_) csv_ << ',' << pose(0, 3) << ',' << pose(1, 3) << ',' << pose(2, 3) << ','
                    << orientation.x() << ',' << orientation.y() << ',' << orientation.z() << ',' << orientation.w();
    } else {
      if (csv_) csv_ << ",nan,nan,nan,nan,nan,nan,nan";
      if (visualization_) visualization_->show(odometry.header, nullptr, nullptr,
        "FAILED  stamp=" + std::to_string(stamp) + "  seconds=" + std::to_string(seconds), false);
    }
    if (csv_) { csv_ << '\n'; csv_.flush(); }
  }

  backend::KeyframeMap prior_;
  backend::ScanContextDescriptor descriptor_;
  backend::RelocalizationTargetCache target_cache_;
  backend::RelocalizationConfig config_;
  std::vector<Eigen::Vector3d> positions_;
  std::string odom_frame_, sensor_frame_, map_frame_;
  double period_ = 3, last_query_ = -1, last_stamp_ = -1;
  bool has_session_ = false;
  uint64_t instance_ = 0, epoch_ = 0, sequence_ = 0;
  std::ofstream csv_;
  std::unique_ptr<ProbeVisualization> visualization_;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr publisher_;
  rclcpp::Subscription<localization_interfaces::msg::FrontendFrame>::SharedPtr subscription_;
};

#ifndef SC_PROBE_NO_MAIN
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  int status = 0;
  try { rclcpp::spin(std::make_shared<Probe>()); }
  catch (const std::exception &error) {
    RCLCPP_ERROR(rclcpp::get_logger("scan_context_probe"), "%s", error.what()); status = 1;
  }
  rclcpp::shutdown();
  return status;
}
#endif
