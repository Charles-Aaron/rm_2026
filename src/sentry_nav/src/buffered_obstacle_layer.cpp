#include <cmath>
#include <limits>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <nav2_costmap_2d/obstacle_layer.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

namespace sentry_nav
{
// 输入已完成同帧底盘系 ROI 和局部平面过滤；此层不裁剪绝对 Z，也不积累永久障碍。
class BufferedObstacleLayer : public nav2_costmap_2d::ObstacleLayer
{
public:
  void onInitialize() override
  {
    auto node = node_.lock();
    if (!node) {throw std::runtime_error("BufferedObstacleLayer requires a live node");}
    declareParameter("observation_sources", rclcpp::ParameterValue(std::string("")));
    std::string sources;
    node->get_parameter(name_ + ".observation_sources", sources);
    // ObservationBuffer 仍执行高度比较；默认覆盖 FLOAT32 点云的全部有限 Z。
    const double limit = std::numeric_limits<float>::max();
    std::istringstream stream(sources);
    for (std::string source; stream >> source;) {
      declareParameter(source + ".min_obstacle_height", rclcpp::ParameterValue(-limit));
      declareParameter(source + ".max_obstacle_height", rclcpp::ParameterValue(limit));
    }
    ObstacleLayer::onInitialize();
    declareParameter("min_obstacle_intensity", rclcpp::ParameterValue(0.0));
    declareParameter("max_obstacle_intensity", rclcpp::ParameterValue(255.0));
    node->get_parameter(name_ + ".min_obstacle_intensity", minimum_intensity_);
    node->get_parameter(name_ + ".max_obstacle_intensity", maximum_intensity_);
    if (!std::isfinite(minimum_intensity_) || !std::isfinite(maximum_intensity_) ||
      minimum_intensity_ > maximum_intensity_)
    {
      throw std::invalid_argument("Obstacle intensity limits must be finite and ordered");
    }
  }

  void updateBounds(
    double robot_x, double robot_y, double robot_yaw,
    double * min_x, double * min_y, double * max_x, double * max_y) override
  {
    std::lock_guard<nav2_costmap_2d::Costmap2D::mutex_t> lock(*getMutex());
    if (rolling_window_) {
      updateOrigin(robot_x - getSizeInMetersX() / 2, robot_y - getSizeInMetersY() / 2);
    }
    resetMaps();
    if (!enabled_) {return;}
    useExtraBounds(min_x, min_y, max_x, max_y);
    // 重建时连旧障碍区域一起更新，确保最后一帧过期后主地图也能清除残留。
    touch(origin_x_, origin_y_, min_x, min_y, max_x, max_y);
    touch(origin_x_ + getSizeInMetersX(), origin_y_ + getSizeInMetersY(),
      min_x, min_y, max_x, max_y);

    std::vector<nav2_costmap_2d::Observation> observations;
    current_ = getMarkingObservations(observations);
    for (const auto & observation : observations) {
      if (observation.cloud_->data.empty()) {continue;}
      const double minimum_range_sq = observation.obstacle_min_range_ * observation.obstacle_min_range_;
      const double maximum_range_sq = observation.obstacle_max_range_ * observation.obstacle_max_range_;
      sensor_msgs::PointCloud2ConstIterator<float> x(*observation.cloud_, "x");
      sensor_msgs::PointCloud2ConstIterator<float> y(*observation.cloud_, "y");
      sensor_msgs::PointCloud2ConstIterator<float> z(*observation.cloud_, "z");
      sensor_msgs::PointCloud2ConstIterator<float> intensity(*observation.cloud_, "intensity");
      for (; x != x.end(); ++x, ++y, ++z, ++intensity) {
        if (!std::isfinite(*x) || !std::isfinite(*y) || !std::isfinite(*z) ||
          !std::isfinite(*intensity) || *intensity < minimum_intensity_ ||
          *intensity > maximum_intensity_) {continue;}
        const double dx = *x - observation.origin_.x;
        const double dy = *y - observation.origin_.y;
        const double dz = *z - observation.origin_.z;
        const double range_sq = dx * dx + dy * dy + dz * dz;
        if (range_sq <= minimum_range_sq || range_sq >= maximum_range_sq) {continue;}
        unsigned int mx, my;
        if (worldToMap(*x, *y, mx, my)) {
          setCost(mx, my, nav2_costmap_2d::LETHAL_OBSTACLE);
        }
      }
    }
    updateFootprint(robot_x, robot_y, robot_yaw, min_x, min_y, max_x, max_y);
  }

  void updateOrigin(double new_x, double new_y) override
  {
    // 每周期都重建，无需复制滚动窗口内的旧栅格；只保持栅格对齐的原点。
    origin_x_ += static_cast<int>((new_x - origin_x_) / resolution_) * resolution_;
    origin_y_ += static_cast<int>((new_y - origin_y_) / resolution_) * resolution_;
  }

  bool isClearable() override {return false;}

private:
  double minimum_intensity_ = 0, maximum_intensity_ = 255;
};
}  // namespace sentry_nav

PLUGINLIB_EXPORT_CLASS(sentry_nav::BufferedObstacleLayer, nav2_costmap_2d::Layer)
