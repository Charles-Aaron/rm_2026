#include "navigation_support.hpp"
#include "visualization.hpp"
#include "frame_broadcaster/level_alignment.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>
#include <rclcpp/rclcpp.hpp>
#include <localization_interfaces/msg/frontend_frame.hpp>
#include <localization_interfaces/msg/frontend_odometry.hpp>
#include <localization_interfaces/msg/frontend_session.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <backend/scan_context.hpp>
#include <backend/search.hpp>

// Transport, session and navigation policy only. Search/registration remain
// byte-identical to the original z implementation used by scan_context_probe.
class ZlioNavigation : public rclcpp::Node {
  using Frame = localization_interfaces::msg::FrontendFrame;
  using Prediction = localization_interfaces::msg::FrontendOdometry;
  using SessionMessage = localization_interfaces::msg::FrontendSession;
  using Steady = std::chrono::steady_clock;
  struct Job {
    uint64_t generation;
    Frame::ConstSharedPtr frame;
    backend::Cloud::Ptr cloud;
    Eigen::Matrix4d pose;
  };
  struct Completed {
    Job job;
    backend::RelocalizationResult result;
    std::string error;
  };

public:
  explicit ZlioNavigation(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : Node("zlio_navigation", options) {
    const auto directory = declare_parameter<std::string>("prior_map", "");
    automatic_ = !directory.empty();
    map_frame_ = declare_parameter<std::string>("map_frame", "map");
    odom_frame_ = declare_parameter<std::string>("odom_frame", "odom");
    lio_frame_ = declare_parameter<std::string>("imu_odom_frame", "imu_odom");
    sensor_frame_ = declare_parameter<std::string>("imu_frame", "imu_link");
    if (std::set<std::string>{map_frame_, odom_frame_, lio_frame_, sensor_frame_}.size() != 4 ||
        map_frame_.empty() || odom_frame_.empty() || lio_frame_.empty() || sensor_frame_.empty())
      throw std::invalid_argument("frame names must be nonempty and distinct");
    retry_interval_ = positive("retry_interval", 3.0);
    input_timeout_ = positive("input_timeout", 0.3);
    result_max_age_ = positive("result_max_age", 3.0);
    const auto translation = vector("imu_to_chassis_translation", {-0.011, 0.09381, -0.17588});
    const auto rpy = vector("imu_to_chassis_rpy", {0, 0, -1.5707963267948966});
    const auto mounting = sentry_frames::imuFromChassis(translation, rpy);
    sensor_from_base_ = mounting.matrix();
    // 与静态 TF 使用同一计算；补偿前端启动姿态后，底盘仍在 odom 原点。
    odom_from_lio_ = declare_parameter<bool>("body_level_initialization", false)
      ? sentry_frames::levelAlignment(mounting).odom_from_lio.matrix().eval()
      : mounting.inverse().matrix().eval();
    if (automatic_) {
      backend::ScanContextDescriptorConfig config;
      if (!backend::ScanContextDescriptor::loadConfig(directory, &config))
        throw std::runtime_error("Cannot load prior_map Scan Context configuration");
      descriptor_.configure(config);
      if (!prior_.load(directory) || !descriptor_.load(directory) || prior_.empty() ||
          !descriptor_.enabled() || prior_.size() != descriptor_.size())
        throw std::runtime_error("Invalid prior_map bundle (no manual fallback for an invalid path)");
      positions_ = prior_.positions();
      if (declare_parameter<bool>("visualization", true)) {
        const double voxel = positive("visualization_map_voxel", 0.2);
        visualization_ = std::make_unique<ProbeVisualization>(*this, prior_, map_frame_, voxel);
      }
    }
    broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    odometry_pub_ = create_publisher<nav_msgs::msg::Odometry>("imu_odom", rclcpp::QoS(1));
    valid_pub_ = create_publisher<std_msgs::msg::Bool>("localization_valid", rclcpp::QoS(1));
    status_pub_ = create_publisher<std_msgs::msg::String>("localization_status", rclcpp::QoS(1).transient_local());
    session_sub_ = create_subscription<SessionMessage>("frontend/session", rclcpp::QoS(1).transient_local(),
      [this](SessionMessage::ConstSharedPtr message) {
        const bool first = !session_.seen;
        const bool changed = session_.update(message->instance_id, message->epoch,
          message->failed || message->reset_required);
        if (changed) {
          invalidate("frontend_session_changed", !first);
          prediction_.reset(); last_sequence_ = 0; last_frame_stamp_ = -1;
          // Preserve the previous project's identity alignment on first manual startup only.
          if (first && !automatic_ && !session_.failed) {aligned_ = true; correction_.setIdentity(); reason_ = "manual_identity";}
        }
        if (session_.failed) invalidate("frontend_failed");
      });
    prediction_sub_ = create_subscription<Prediction>("frontend/odometry", rclcpp::SensorDataQoS().keep_last(1),
      [this](Prediction::ConstSharedPtr message) {prediction(message);});
    frame_sub_ = create_subscription<Frame>("frontend/frame", rclcpp::SensorDataQoS().keep_last(1),
      [this](Frame::ConstSharedPtr message) {
        try {frame(message);} catch (const std::exception &error) {
          RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000, "Dropping frame: %s", error.what());
        }
      });
    pose_sub_ = create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>("initialpose", 1,
      [this](geometry_msgs::msg::PoseWithCovarianceStamped::ConstSharedPtr message) {manualPose(*message);});
    reset_service_ = create_service<std_srvs::srv::Trigger>("relocalize",
      [this](const std::shared_ptr<std_srvs::srv::Trigger::Request>,
             std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
        invalidate(automatic_ ? "relocalization_requested" : "awaiting_initialpose");
        response->success = true;
        response->message = automatic_ ? "Cleared alignment; searching on the next valid frame" : "Set initialpose to resume";
      });
    // Wall timer keeps emitting invalid heartbeats when simulated sensor time stops.
    timer_ = create_wall_timer(std::chrono::milliseconds(25), [this]() {tick();});
    if (automatic_) worker_ = std::thread([this]() {work();});
    RCLCPP_INFO(get_logger(), "ZLIO navigation: %s", automatic_ ? "Scan Context startup localization; lock after first success" : "manual alignment (prior_map is empty)");
  }

  ~ZlioNavigation() override {
    {std::lock_guard<std::mutex> lock(worker_mutex_); stopping_ = true; pending_.reset();}
    worker_cv_.notify_all();
    if (worker_.joinable()) worker_.join();
  }

private:
  double positive(const std::string &name, double value) {
    const double result = declare_parameter<double>(name, value);
    if (!std::isfinite(result) || result <= 0) throw std::invalid_argument(name + " must be positive");
    return result;
  }
  Eigen::Vector3d vector(const std::string &name, std::vector<double> fallback) {
    const auto values = declare_parameter<std::vector<double>>(name, fallback);
    if (values.size() != 3) throw std::invalid_argument(name + " requires three values");
    Eigen::Vector3d result(values[0], values[1], values[2]);
    if (!result.allFinite()) throw std::invalid_argument(name + " must be finite");
    return result;
  }
  double stamp(const builtin_interfaces::msg::Time &time) const {return rclcpp::Time(time).seconds();}
  bool fresh(const builtin_interfaces::msg::Time &time, double limit) const {
    const double age = now().seconds() - stamp(time);
    return age >= -0.05 && age <= limit;
  }
  bool inputFresh() const {
    return prediction_ && !session_.failed && fresh(prediction_->odometry.header.stamp, input_timeout_) &&
      std::chrono::duration<double>(Steady::now() - prediction_received_).count() <= input_timeout_;
  }
  void invalidate(const std::string &reason, bool hold = true) {
    aligned_ = false; ++generation_; reason_ = reason; last_query_ = -1;
    // Keep the invalid state longer than the consumer heartbeat timeout so a
    // fast reacquisition cannot hide a frontend reset between control ticks.
    if (hold) hold_until_ = Steady::now() + std::chrono::milliseconds(350);
    if (visualization_) {
      std_msgs::msg::Header header; header.stamp = now(); header.frame_id = map_frame_;
      visualization_->reset(header);
    }
    std_msgs::msg::Bool message; message.data = false; valid_pub_->publish(message);
  }
  void prediction(const Prediction::ConstSharedPtr &message) {
    if (!session_.accepts(message->instance_id, message->epoch)) return;
    const auto &odometry = message->odometry;
    if (odometry.header.frame_id != lio_frame_ || odometry.child_frame_id != sensor_frame_ ||
        !fresh(odometry.header.stamp, input_timeout_)) return;
    if (prediction_ && stamp(odometry.header.stamp) <= stamp(prediction_->odometry.header.stamp)) return;
    try {sentry_localization::matrix(odometry.pose.pose);} catch (const std::exception &) {return;}
    const auto &v = odometry.twist.twist;
    for (double value : {v.linear.x, v.linear.y, v.linear.z, v.angular.x, v.angular.y, v.angular.z})
      if (!std::isfinite(value)) return;
    prediction_ = message; prediction_received_ = Steady::now();
    odometry_pub_->publish(odometry);
    geometry_msgs::msg::TransformStamped tf;
    tf.header = odometry.header; tf.child_frame_id = sensor_frame_;
    tf.transform = sentry_localization::transform(sentry_localization::matrix(odometry.pose.pose));
    broadcaster_->sendTransform(tf);
  }
  void frame(const Frame::ConstSharedPtr &message) {
    if (!session_.accepts(message->instance_id, message->epoch) || !message->lidar_updated ||
        message->sequence <= last_sequence_ || message->odometry.header.frame_id != lio_frame_ ||
        message->odometry.child_frame_id != sensor_frame_ || message->cloud.header.frame_id != sensor_frame_ ||
        message->cloud.header.stamp != message->odometry.header.stamp ||
        !fresh(message->odometry.header.stamp, input_timeout_)) return;
    const double time = stamp(message->odometry.header.stamp);
    if (time < last_frame_stamp_) return;
    const auto pose = sentry_localization::matrix(message->odometry.pose.pose);
    last_sequence_ = message->sequence; last_frame_stamp_ = time;
    // 地形层独立消费原始帧；定位锁定后无需在这里重复解码同一份点云。
    if (!automatic_ || aligned_ || inflight_ ||
        (last_query_ >= 0 && time - last_query_ < retry_interval_)) return;
    const auto cloud = sentry_localization::decode(message->cloud);
    if (cloud->empty()) return;
    {std::lock_guard<std::mutex> lock(worker_mutex_); pending_ = Job{generation_, message, cloud, pose};}
    last_query_ = time; inflight_ = true; worker_cv_.notify_one();
  }
  void manualPose(const geometry_msgs::msg::PoseWithCovarianceStamped &message) {
    if (automatic_) {
      RCLCPP_WARN(get_logger(), "Automatic mode owns map->odom; use relocalize to request a new global search"); return;
    }
    if (!inputFresh() || (!message.header.frame_id.empty() && message.header.frame_id != map_frame_)) return;
    try {
      const Eigen::Matrix4d odom_from_base = odom_from_lio_ * sentry_localization::matrix(prediction_->odometry.pose.pose) * sensor_from_base_;
      const Eigen::Matrix4d updated = sentry_localization::matrix(message.pose.pose) * odom_from_base.inverse();
      invalidate("manual_initialpose");
      correction_ = updated;
      aligned_ = true; reason_ = "manual_initialpose";
    } catch (const std::exception &error) {RCLCPP_WARN(get_logger(), "Invalid initialpose: %s", error.what());}
  }
  void work() {
    backend::RelocalizationTargetCache cache;
    uint64_t cache_generation = 0;
    for (;;) {
      std::optional<Job> job;
      {
        std::unique_lock<std::mutex> lock(worker_mutex_);
        worker_cv_.wait(lock, [this]() {return stopping_ || pending_.has_value();});
        if (stopping_) return;
        job = std::move(pending_); pending_.reset();
      }
      Completed done{*job, {}, {}};
      try {
        if (cache_generation != job->generation) {cache.clear(); cache_generation = job->generation;}
        backend::Keyframe runtime;
        runtime.index = 0; runtime.epoch = job->frame->epoch;
        runtime.timestamp = stamp(job->frame->odometry.header.stamp);
        runtime.odom_pose = job->pose; runtime.cloud = backend::CloudBlock(job->cloud);
        backend::RelocalizationRequest request{runtime, config_, positions_};
        request.search_radius = 0.0;
        done.result = backend::runRelocalization(prior_, descriptor_, cache, request);
      } catch (const std::exception &error) {done.error = error.what();}
      {std::lock_guard<std::mutex> lock(worker_mutex_); completed_ = std::move(done);}
    }
  }
  void consume(Completed &done) {
    const auto &message = *done.job.frame;
    if (done.job.generation != generation_ || !session_.accepts(message.instance_id, message.epoch)) return;
    if (!done.error.empty()) {RCLCPP_ERROR(get_logger(), "Relocalization: %s", done.error.c_str());}
    if (!done.result.success || !done.result.T_map_odom.allFinite()) {reason_ = "query_failed"; return;}
    if (!fresh(message.odometry.header.stamp, result_max_age_) || !inputFresh()) {reason_ = "result_expired"; return;}
    // Upstream odom means imu_odom here. Publish map->odom, accounting for the
    // existing static odom->imu_odom rotation AND translation.
    const Eigen::Matrix4d candidate = done.result.T_map_odom * odom_from_lio_.inverse();
    correction_ = candidate; aligned_ = true; reason_ = "localized";
    RCLCPP_INFO(get_logger(), "Startup localization succeeded; map->odom locked until relocalize or a frontend session change");
    if (visualization_) {
      const Eigen::Matrix4d pose = done.result.T_map_odom * done.job.pose;
      visualization_->show(message.odometry.header, done.job.cloud.get(), &pose, "LOCALIZED", true);
    }
  }
  void tick() {
    std::optional<Completed> done;
    {std::lock_guard<std::mutex> lock(worker_mutex_); done = std::move(completed_); completed_.reset();}
    if (done) {inflight_ = false; consume(*done);}
    const double time = now().seconds();
    if (last_clock_ >= 0 && time < last_clock_) {invalidate("clock_rewind_restart_frontend"); prediction_.reset();}
    last_clock_ = time;
    const bool settling = Steady::now() < hold_until_;
    // 固定地图对齐不按时间过期，前端新鲜度和会话检查仍控制导航放行。
    const bool valid = aligned_ && inputFresh() && !settling;
    std_msgs::msg::Bool heartbeat; heartbeat.data = valid; valid_pub_->publish(heartbeat);
    if (valid) {
      geometry_msgs::msg::TransformStamped tf;
      tf.header.stamp = now(); tf.header.frame_id = map_frame_; tf.child_frame_id = odom_frame_;
      tf.transform = sentry_localization::transform(correction_); broadcaster_->sendTransform(tf);
    }
    const std::string state = !inputFresh() ? "WAITING_FRONTEND" :
      (settling ? "ALIGNMENT_SETTLING" : (!aligned_ ? (automatic_ ? "RELOCALIZING" : "WAITING_INITIALPOSE") :
        (automatic_ ? "LOCALIZED" : "MANUAL")));
    const auto status = state + " reason=" + reason_ + " instance=" + std::to_string(session_.instance) +
      " epoch=" + std::to_string(session_.epoch);
    if (status != last_status_) {
      if (!valid && visualization_) {
        std_msgs::msg::Header header; header.stamp = now(); header.frame_id = map_frame_;
        visualization_->show(header, nullptr, nullptr, status, false);
      }
      std_msgs::msg::String message; message.data = status; status_pub_->publish(message);
      RCLCPP_INFO(get_logger(), "%s", status.c_str()); last_status_ = status;
    }
  }

  std::string map_frame_, odom_frame_, lio_frame_, sensor_frame_, reason_ = "starting", last_status_;
  bool automatic_ = false, aligned_ = false, inflight_ = false;
  double retry_interval_, input_timeout_, result_max_age_;
  double last_query_ = -1, last_frame_stamp_ = -1, last_clock_ = -1;
  uint64_t generation_ = 0, last_sequence_ = 0;
  sentry_localization::Session session_;
  Eigen::Matrix4d sensor_from_base_ = Eigen::Matrix4d::Identity();
  Eigen::Matrix4d odom_from_lio_ = Eigen::Matrix4d::Identity(), correction_ = Eigen::Matrix4d::Identity();
  Prediction::ConstSharedPtr prediction_;
  Steady::time_point prediction_received_, hold_until_;
  backend::KeyframeMap prior_;
  backend::ScanContextDescriptor descriptor_;
  backend::RelocalizationConfig config_;
  std::vector<Eigen::Vector3d> positions_;
  std::unique_ptr<ProbeVisualization> visualization_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odometry_pub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr valid_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
  rclcpp::Subscription<Frame>::SharedPtr frame_sub_;
  rclcpp::Subscription<Prediction>::SharedPtr prediction_sub_;
  rclcpp::Subscription<SessionMessage>::SharedPtr session_sub_;
  rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr pose_sub_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr reset_service_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::thread worker_;
  std::mutex worker_mutex_;
  std::condition_variable worker_cv_;
  bool stopping_ = false;
  std::optional<Job> pending_;
  std::optional<Completed> completed_;
};

#ifndef ZLIO_NAVIGATION_NO_MAIN
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  int result = 0;
  try {rclcpp::spin(std::make_shared<ZlioNavigation>());}
  catch (const std::exception &error) {
    RCLCPP_ERROR(rclcpp::get_logger("zlio_navigation"), "%s", error.what()); result = 1;
  }
  rclcpp::shutdown();
  return result;
}
#endif
