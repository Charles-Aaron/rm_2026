#include "sentry_terrain/input_gate.hpp"
#include "sentry_terrain/terrain_core.hpp"

#include <frame_broadcaster/level_alignment.hpp>
#include <localization_interfaces/msg/frontend_frame.hpp>
#include <localization_interfaces/msg/frontend_session.hpp>
#include <nav2_costmap_2d/layer.hpp>
#include <nav2_costmap_2d/footprint.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <std_msgs/msg/string.hpp>

#include <condition_variable>
#include <cstring>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>

namespace sentry_terrain {
class TerrainProjectionLayer : public nav2_costmap_2d::Layer {
  using Frame = localization_interfaces::msg::FrontendFrame;
  using Session = localization_interfaces::msg::FrontendSession;
  using Clock = std::chrono::steady_clock;
  using Cloud = sensor_msgs::msg::PointCloud2;
  using Grid = nav_msgs::msg::OccupancyGrid;
  struct Job {
    Frame::ConstSharedPtr frame;
    uint64_t generation;
    Clock::time_point received;
  };
  struct Result {
    TerrainSnapshot map;
    builtin_interfaces::msg::Time stamp;
    Clock::time_point received, built;
    uint64_t generation{0};
    double processing_ms{0};
  };

public:
  ~TerrainProjectionLayer() override {
    {std::lock_guard<std::mutex> lock(mutex_); stopping_ = true;}
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
  }

  void onInitialize() override {
    auto node = node_.lock();
    if (!node) throw std::runtime_error("terrain layer requires a node");
    world_ = shared<std::string>("odom_frame", "odom");
    imu_ = shared<std::string>("imu_frame", "imu_link");
    lio_ = shared<std::string>("imu_odom_frame", "imu_odom");
    if (layered_costmap_->getGlobalFrameID() != world_)
      throw std::invalid_argument("terrain layer must run in the configured odom frame");
    const auto mounting = sentry_frames::imuFromChassis(
      sharedVector("imu_to_chassis_translation"), sharedVector("imu_to_chassis_rpy"));
    imu_from_chassis_ = mounting;
    odom_from_lio_ = shared<bool>("body_level_initialization", false)
      ? sentry_frames::levelAlignment(mounting).odom_from_lio : mounting.inverse();
    lidar_in_imu_ = sharedVector("imu_to_lidar_translation");
    config_.resolution = param("resolution", config_.resolution);
    config_.map_size = vectorParam("map_size", config_.map_size);
    config_.sliding_threshold = param("sliding_threshold", config_.sliding_threshold);
    config_.z_min = param("height_min", config_.z_min);
    config_.z_max = param("height_max", config_.z_max);
    config_.ray_min = param("ray_min", config_.ray_min);
    config_.ray_max = param("ray_max", config_.ray_max);
    config_.p_hit = param("probability.hit", config_.p_hit);
    config_.p_miss = param("probability.miss", config_.p_miss);
    config_.p_min = param("probability.min", config_.p_min);
    config_.p_max = param("probability.max", config_.p_max);
    config_.p_occ = param("probability.occupied", config_.p_occ);
    config_.p_free = param("probability.free", config_.p_free);
    config_.decay_enable = param("decay.enabled", config_.decay_enable);
    config_.decay_active_list = param("decay.active_list", config_.decay_active_list);
    config_.keep_time = param("decay.keep_time", config_.keep_time);
    config_.clear_time = param("decay.clear_time", config_.clear_time);
    auto & p = config_.projection;
    p.min_observed_voxels = param("projection.min_observed_voxels", p.min_observed_voxels);
    p.surface_height_delta_max = param("projection.surface_height_max", p.surface_height_delta_max);
    p.wall_height_delta_min = param("projection.wall_height_min", p.wall_height_delta_min);
    p.wall_occupancy_ratio_min = param("projection.wall_ratio_min", p.wall_occupancy_ratio_min);
    p.tunnel_enable = param("projection.tunnel_enabled", p.tunnel_enable);
    p.tunnel_height_delta_min = param("projection.tunnel_height_min", p.tunnel_height_delta_min);
    p.tunnel_height_delta_max = param("projection.tunnel_height_max", p.tunnel_height_delta_max);
    p.tunnel_occupancy_ratio_max = param("projection.tunnel_ratio_max", p.tunnel_occupancy_ratio_max);
    p.obstacle_hold_time = param("projection.obstacle_hold_time", p.obstacle_hold_time);
    p.mask_filter_en = param("projection.mask_filter", p.mask_filter_en);
    p.fill_occ_min = param("projection.fill_neighbors", p.fill_occ_min);
    p.denoise_occ_max = param("projection.denoise_neighbors", p.denoise_occ_max);
    p.unknown_as_occupied = param("projection.unknown_as_occupied", p.unknown_as_occupied);
    config_.validate();
    checkGeometry();
    self_enabled_ = param("self_filter.enabled", true);
    self_min_ = vectorParam("self_filter.min", {-.30, -.18, -.25});
    self_max_ = vectorParam("self_filter.max", {.30, .45, .18});
    if ((self_max_ - self_min_).minCoeff() <= 0)
      throw std::invalid_argument("invalid terrain self filter box");
    input_timeout_ = positive("input_timeout", .30);
    worker_timeout_ = positive("worker_timeout", .50);
    period_ = std::chrono::duration_cast<Clock::duration>(
      std::chrono::duration<double>(1.0 / positive("update_frequency", 20.0)));
    visual_grid_period_ = 1.0 / positive("visualization.grid_frequency", 5.0);
    visual_cloud_period_ = 1.0 / positive("visualization.cloud_frequency", 2.0);
    const auto prefix = param("visualization.prefix", std::string("/x_sentry/terrain"));
    const auto qos = rclcpp::QoS(1).best_effort();
    occupied_pub_ = node->create_publisher<Cloud>(prefix + "/occupied", qos);
    input_pub_ = node->create_publisher<Cloud>(prefix + "/input", qos);
    height_pub_ = node->create_publisher<Cloud>(prefix + "/height_delta", qos);
    type_pub_ = node->create_publisher<Grid>(prefix + "/layer_type", qos);
    mask_pub_ = node->create_publisher<Grid>(prefix + "/layer_value_dynamic", qos);
    status_pub_ = node->create_publisher<std_msgs::msg::String>(prefix + "/status", qos);
    for (auto & pub : {occupied_pub_, input_pub_, height_pub_}) pub->on_activate();
    type_pub_->on_activate(); mask_pub_->on_activate(); status_pub_->on_activate();
    session_sub_ = node->create_subscription<Session>(
      param("session_topic", std::string("/x_sentry/frontend/session")),
      rclcpp::QoS(1).transient_local(), [this](Session::ConstSharedPtr msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (gate_.session(msg->instance_id, msg->epoch, msg->failed || msg->reset_required))
          invalidateLocked();
      });
    frame_sub_ = node->create_subscription<Frame>(
      param("frame_topic", std::string("/x_sentry/frontend/frame")),
      rclcpp::SensorDataQoS().keep_last(1), [this](Frame::ConstSharedPtr msg) {receive(std::move(msg));});
    parameter_guard_ = node->add_on_set_parameters_callback([this](const auto & parameters) {
      rcl_interfaces::msg::SetParametersResult result;
      result.successful = true;
      for (const auto & p : parameters) {
        const auto & key = p.get_name();
        if (key.rfind(name_ + ".", 0) == 0 || key.rfind("imu_", 0) == 0 ||
            key == "body_level_initialization" || key == "odom_frame") {
          result.successful = false; result.reason = "terrain parameters require restart";
        }
      }
      return result;
    });
    current_ = false;
    enabled_ = true;
    worker_ = std::thread([this] {work();});
  }

  void activate() override {
    std::lock_guard<std::mutex> lock(mutex_);
    active_ = true; invalidateLocked();
  }
  void deactivate() override {
    std::lock_guard<std::mutex> lock(mutex_);
    active_ = false; invalidateLocked();
  }
  void reset() override {
    std::lock_guard<std::mutex> lock(mutex_);
    invalidateLocked();
  }
  bool isClearable() override {return false;}
  void matchSize() override {checkGeometry();}

  void updateBounds(double x, double y, double yaw, double * min_x, double * min_y,
    double * max_x, double * max_y) override {
    const auto * master = layered_costmap_->getCostmap();
    // 每次覆盖整个局部窗口，主地图重置后重叠静态层，过期动态障碍自然消失。
    *min_x = std::min(*min_x, master->getOriginX());
    *min_y = std::min(*min_y, master->getOriginY());
    *max_x = std::max(*max_x, master->getOriginX() + master->getSizeInMetersX());
    *max_y = std::max(*max_y, master->getOriginY() + master->getSizeInMetersY());
    nav2_costmap_2d::transformFootprint(x, y, yaw, layered_costmap_->getFootprint(), footprint_);
    std::lock_guard<std::mutex> lock(mutex_);
    cycle_result_ = result_;
    current_ = healthyLocked(cycle_result_);
  }

  void updateCosts(nav2_costmap_2d::Costmap2D & master, int min_i, int min_j,
    int max_i, int max_j) override {
    // 在真正合并前再次检查会话，避免 updateBounds 后换代留下旧快照。
    std::lock_guard<std::mutex> lock(mutex_);
    current_ = healthyLocked(cycle_result_);
    if (!current_) return;
    const auto & map = cycle_result_->map;
    for (int y = min_j; y < max_j; ++y) {
      for (int x = min_i; x < max_i; ++x) {
        double wx, wy;
        master.mapToWorld(x, y, wx, wy);
        if (map.occupied(wx, wy) && !insideFootprint(wx, wy))
          master.setCost(x, y, nav2_costmap_2d::LETHAL_OBSTACLE);
      }
    }
  }

private:
  template<class T> T param(const std::string & key, T fallback) {
    declareParameter(key, rclcpp::ParameterValue(fallback));
    T value;
    node_.lock()->get_parameter(name_ + "." + key, value);
    return value;
  }
  template<class T> T shared(const std::string & key, T fallback) {
    auto node = node_.lock();
    if (!node->has_parameter(key)) node->declare_parameter<T>(key, fallback);
    return node->get_parameter(key).get_value<T>();
  }
  Eigen::Vector3d sharedVector(const std::string & key) {
    // 外参必须由同一 frames.yaml 提供，缺失时不以另一组机械值悄悄运行。
    auto v = shared<std::vector<double>>(key, {});
    if (v.size() != 3) throw std::invalid_argument("missing shared frame parameter: " + key);
    Eigen::Vector3d result(v[0], v[1], v[2]);
    if (!result.allFinite()) throw std::invalid_argument("nonfinite frame parameter: " + key);
    return result;
  }
  Eigen::Vector3d vectorParam(const std::string & key, Eigen::Vector3d fallback) {
    const auto v = param(key, std::vector<double>{fallback.x(), fallback.y(), fallback.z()});
    if (v.size() != 3) throw std::invalid_argument(key + " needs three values");
    Eigen::Vector3d result(v[0], v[1], v[2]);
    if (!result.allFinite()) throw std::invalid_argument(key + " must be finite");
    return result;
  }
  double positive(const std::string & key, double value) {
    value = param(key, value);
    if (!std::isfinite(value) || value <= 0) throw std::invalid_argument(key + " must be positive");
    return value;
  }
  void checkGeometry() const {
    const auto * map = layered_costmap_->getCostmap();
    const double margin = 2 * (config_.sliding_threshold + config_.resolution);
    if (std::abs(map->getResolution() - config_.resolution) > 1e-8 ||
        map->getSizeInMetersX() + margin > config_.map_size.x() + 1e-8 ||
        map->getSizeInMetersY() + margin > config_.map_size.y() + 1e-8)
      throw std::invalid_argument("terrain storage must cover the costmap plus sliding margin at equal resolution");
  }
  void invalidateLocked() {
    ++generation_; pending_.reset(); result_.reset();
    cv_.notify_all();
  }
  bool fresh(double stamp) const {
    const auto node = node_.lock();
    if (!node) return false;
    const double age = node->now().seconds() - stamp;
    return age >= -.05 && age <= input_timeout_;
  }
  bool healthyLocked(const std::shared_ptr<const Result> & result) const {
    return active_ && !gate_.failed && result && result->generation == generation_ &&
      fresh(rclcpp::Time(result->stamp).seconds()) &&
      std::chrono::duration<double>(Clock::now() - result->received).count() <= input_timeout_ &&
      std::chrono::duration<double>(Clock::now() - result->built).count() <= worker_timeout_;
  }
  void checkClockLocked(double time) {
    if (last_clock_ >= 0 && time < last_clock_ - .001) {
      gate_.rewind(); invalidateLocked();
    }
    last_clock_ = time;
  }
  void receive(Frame::ConstSharedPtr msg) {
    if (!msg->lidar_updated || msg->odometry.header.frame_id != lio_ ||
        msg->odometry.child_frame_id != imu_ || msg->cloud.header.frame_id != imu_ ||
        msg->cloud.header.stamp != msg->odometry.header.stamp) return;
    const double stamp = rclcpp::Time(msg->cloud.header.stamp).seconds();
    std::lock_guard<std::mutex> lock(mutex_);
    checkClockLocked(node_.lock()->now().seconds());
    if (!active_ || !fresh(stamp) ||
        !gate_.accept(msg->instance_id, msg->epoch, msg->sequence, stamp)) return;
    if (pending_) ++replaced_;
    pending_ = Job{std::move(msg), generation_, Clock::now()};
  }
  bool insideFootprint(double x, double y) const {
    bool inside = false;
    if (footprint_.size() < 3) return false;
    for (size_t i = 0, j = footprint_.size() - 1; i < footprint_.size(); j = i++) {
      const auto & a = footprint_[i]; const auto & b = footprint_[j];
      if ((a.y > y) != (b.y > y) && x < (b.x - a.x) * (y - a.y) / (b.y - a.y) + a.x)
        inside = !inside;
    }
    return inside;
  }
  rog_map::PointCloud decode(const Frame & frame, Eigen::Isometry3d & pose) const {
    const auto & p = frame.odometry.pose.pose;
    Eigen::Quaterniond rotation(p.orientation.w, p.orientation.x, p.orientation.y, p.orientation.z);
    if (!rotation.coeffs().allFinite() || std::abs(rotation.norm() - 1) > .01)
      throw std::invalid_argument("invalid frame quaternion");
    pose.setIdentity(); pose.linear() = rotation.normalized().toRotationMatrix();
    pose.translation() = Eigen::Vector3d(p.position.x, p.position.y, p.position.z);
    pose = odom_from_lio_ * pose;
    if (!pose.matrix().allFinite()) throw std::invalid_argument("nonfinite frame pose");
    const auto & msg = frame.cloud;
    if (msg.is_bigendian || !msg.point_step ||
        uint64_t(msg.row_step) < uint64_t(msg.width) * msg.point_step ||
        uint64_t(msg.data.size()) < uint64_t(msg.row_step) * msg.height)
      throw std::invalid_argument("invalid cloud layout");
    uint32_t offsets[3]{};
    for (int axis = 0; axis < 3; ++axis) {
      bool found = false;
      for (const auto & f : msg.fields) {
        if (f.name != std::string(1, "xyz"[axis])) continue;
        if (f.datatype != sensor_msgs::msg::PointField::FLOAT32 || f.count != 1 ||
            uint64_t(f.offset) + sizeof(float) > msg.point_step)
          throw std::invalid_argument("cloud needs float32 XYZ");
        offsets[axis] = f.offset; found = true;
      }
      if (!found) throw std::invalid_argument("missing cloud XYZ");
    }
    rog_map::PointCloud out;
    out.reserve(size_t(msg.width) * msg.height);
    for (uint32_t row = 0; row < msg.height; ++row) {
      for (uint32_t col = 0; col < msg.width; ++col) {
        const auto * data = msg.data.data() + size_t(row) * msg.row_step + size_t(col) * msg.point_step;
        float xyz[3];
        for (int k = 0; k < 3; ++k) std::memcpy(&xyz[k], data + offsets[k], sizeof(float));
        Eigen::Vector3d point(xyz[0], xyz[1], xyz[2]);
        if (!point.allFinite()) continue;
        if (self_enabled_ && (point.array() >= self_min_.array()).all() &&
            (point.array() <= self_max_.array()).all()) continue;
        point = pose * point;
        out.push_back(pcl::PointXYZI(point.x(), point.y(), point.z(), 0));
      }
    }
    return out;
  }

  void work() {
    std::unique_ptr<TerrainCore> core;
    uint64_t core_generation = 0;
    std::optional<Job> last_job;
    auto deadline = Clock::now();
    while (true) {
      deadline += period_;
      std::optional<Job> job;
      uint64_t generation;
      {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_until(lock, deadline, [this] {return stopping_;});
        if (stopping_) return;
        checkClockLocked(node_.lock()->now().seconds());
        generation = generation_;
        job = std::move(pending_); pending_.reset();
      }
      if (core_generation != generation) {
        clearVisualization(generation);
        core.reset(); last_job.reset(); core_generation = generation;
      }
      try {
        const auto started = Clock::now();
        rog_map::PointCloud cloud;
        bool integrated = false;
        if (job && fresh(rclcpp::Time(job->frame->cloud.header.stamp).seconds()) &&
            std::chrono::duration<double>(Clock::now() - job->received).count() <= input_timeout_) {
          Eigen::Isometry3d pose;
          cloud = decode(*job->frame, pose);
          if (!core) core = std::make_unique<TerrainCore>(config_);
          core->integrate(cloud, pose * lidar_in_imu_, (pose * imu_from_chassis_).translation(),
            rclcpp::Time(job->frame->cloud.header.stamp).seconds());
          last_job = job;
          integrated = true;
        }
        if (core && last_job) {
          auto result = std::make_shared<Result>();
          result->map = core->project(node_.lock()->now().seconds());
          result->stamp = last_job->frame->cloud.header.stamp;
          result->received = last_job->received;
          result->built = Clock::now(); result->generation = generation;
          result->processing_ms = std::chrono::duration<double, std::milli>(Clock::now() - started).count();
          {
            std::lock_guard<std::mutex> lock(mutex_);
            if (generation != generation_ || !active_) continue;
            result_ = result;
          }
          visualize(*core, *result, integrated ? &cloud : nullptr);
        }
      } catch (const std::exception & e) {
        auto node = node_.lock();
        RCLCPP_WARN_THROTTLE(node->get_logger(), *node->get_clock(), 2000,
          "Terrain update rejected: %s", e.what());
        core.reset(); last_job.reset();
        std::lock_guard<std::mutex> lock(mutex_);
        if (generation == generation_) result_.reset();
      }
      // 单次超周期时直接处理下一帧，不再额外等待一个完整周期。
      if (deadline < Clock::now() - period_) deadline = Clock::now() - period_;
    }
  }

  template<class Message, class Publisher>
  void publishChecked(const Message & msg, const Publisher & publisher, uint64_t generation) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (generation == generation_) publisher->publish(msg);
  }
  void clearVisualization(uint64_t generation) {
    std_msgs::msg::String status;
    status.data = "current=0 source_age=unavailable generation=" + std::to_string(generation);
    publishChecked(status, status_pub_, generation);
    const auto stamp = node_.lock()->now();
    for (const auto & pub : {input_pub_, occupied_pub_, height_pub_}) {
      if (pub->get_subscription_count()) publishCloud({}, pub, stamp, generation);
    }
    if (!type_pub_->get_subscription_count() && !mask_pub_->get_subscription_count()) return;
    Grid grid;
    grid.header.frame_id = world_; grid.header.stamp = stamp;
    grid.info.resolution = config_.resolution;
    grid.info.width = 2 * static_cast<int>(config_.map_size.x() / (2 * config_.resolution)) + 1;
    grid.info.height = 2 * static_cast<int>(config_.map_size.y() / (2 * config_.resolution)) + 1;
    grid.info.origin.orientation.w = 1;
    grid.data.resize(grid.info.width * grid.info.height, 0);
    if (type_pub_->get_subscription_count()) publishChecked(grid, type_pub_, generation);
    if (mask_pub_->get_subscription_count()) publishChecked(grid, mask_pub_, generation);
  }
  void publishCloud(const rog_map::PointCloud & points,
    const rclcpp_lifecycle::LifecyclePublisher<Cloud>::SharedPtr & pub,
    const builtin_interfaces::msg::Time & stamp, uint64_t generation) {
    Cloud msg;
    pcl::toROSMsg(points, msg);
    msg.header.frame_id = world_; msg.header.stamp = stamp;
    publishChecked(msg, pub, generation);
  }
  void visualize(const TerrainCore & core, const Result & result, const rog_map::PointCloud * input) {
    const double wall = std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
    const auto & map = result.map;
    if (wall - last_grid_visual_ >= visual_grid_period_) {
      last_grid_visual_ = wall;
      if (type_pub_->get_subscription_count() || mask_pub_->get_subscription_count()) {
        Grid msg;
        msg.header.frame_id = world_; msg.header.stamp = result.stamp;
        msg.info.resolution = map.resolution; msg.info.width = map.width; msg.info.height = map.height;
        msg.info.origin.position.x = map.origin.x(); msg.info.origin.position.y = map.origin.y();
        msg.info.origin.orientation.w = 1;
        msg.data.resize(map.types.size());
        if (type_pub_->get_subscription_count()) {
          for (size_t i = 0; i < map.types.size(); ++i)
            msg.data[i] = static_cast<int>(map.types[i]) * 100 / 3;
          publishChecked(msg, type_pub_, result.generation);
        }
        if (mask_pub_->get_subscription_count()) {
          for (size_t i = 0; i < map.types.size(); ++i)
            msg.data[i] = map.types[i] == rog_map::CellType::OCCUPIED ? 100 : 0;
          publishChecked(msg, mask_pub_, result.generation);
        }
      }
      if (status_pub_->get_subscription_count()) {
        std_msgs::msg::String msg;
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream out;
        out << "current=" << healthyLocked(result_) << " source_age=" <<
          node_.lock()->now().seconds() - rclcpp::Time(result.stamp).seconds() <<
          " processing_ms=" << result.processing_ms <<
          " replaced=" << replaced_ << " generation=" << result.generation;
        msg.data = out.str();
        if (result.generation == generation_) status_pub_->publish(msg);
      }
    }
    if (wall - last_cloud_visual_ < visual_cloud_period_) return;
    last_cloud_visual_ = wall;
    if (occupied_pub_->get_subscription_count())
      publishCloud(core.occupiedCloud(), occupied_pub_, result.stamp, result.generation);
    if (input && input_pub_->get_subscription_count())
      publishCloud(*input, input_pub_, result.stamp, result.generation);
    if (height_pub_->get_subscription_count()) {
      rog_map::PointCloud cloud;
      for (size_t i = 0; i < map.types.size(); ++i) {
        if (!std::isfinite(map.top_z[i])) continue;
        cloud.push_back(pcl::PointXYZI(map.origin.x() + (i % map.width + .5) * map.resolution,
          map.origin.y() + (i / map.width + .5) * map.resolution, map.top_z[i], map.height_delta[i]));
      }
      publishCloud(cloud, height_pub_, result.stamp, result.generation);
    }
  }

  TerrainConfig config_;
  Eigen::Isometry3d odom_from_lio_, imu_from_chassis_;
  Eigen::Vector3d lidar_in_imu_, self_min_, self_max_;
  std::string world_, imu_, lio_;
  bool self_enabled_{true}, active_{false}, stopping_{false};
  double input_timeout_{.3}, worker_timeout_{.5}, last_clock_{-1};
  double visual_grid_period_{.2}, visual_cloud_period_{.5};
  double last_grid_visual_{0}, last_cloud_visual_{0};
  Clock::duration period_{std::chrono::milliseconds(50)};
  uint64_t generation_{1}, replaced_{0};
  InputGate gate_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::optional<Job> pending_;
  std::shared_ptr<const Result> result_, cycle_result_;
  std::thread worker_;
  std::vector<geometry_msgs::msg::Point> footprint_;
  rclcpp::Subscription<Frame>::SharedPtr frame_sub_;
  rclcpp::Subscription<Session>::SharedPtr session_sub_;
  rclcpp_lifecycle::LifecyclePublisher<Cloud>::SharedPtr occupied_pub_, input_pub_, height_pub_;
  rclcpp_lifecycle::LifecyclePublisher<Grid>::SharedPtr type_pub_, mask_pub_;
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr status_pub_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr parameter_guard_;
};
}  // namespace sentry_terrain

PLUGINLIB_EXPORT_CLASS(sentry_terrain::TerrainProjectionLayer, nav2_costmap_2d::Layer)
