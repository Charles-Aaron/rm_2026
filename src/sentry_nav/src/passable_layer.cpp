#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nav2_costmap_2d/layer.hpp>
#include <nav2_costmap_2d/cost_values.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <tf2/exceptions.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/utils.h>
#include <yaml-cpp/yaml.h>

namespace sentry_nav
{
class PassableLayer : public nav2_costmap_2d::Layer
{
  using Point = std::pair<double, double>;
  std::vector<std::vector<Point>> polygons_;
  std::string frame_;

  static bool inside(double x, double y, const std::vector<Point> & p)
  {
    bool result = false;
    for (size_t i = 0, j = p.size() - 1; i < p.size(); j = i++) {
      const auto & a = p[i]; const auto & b = p[j];
      if ((a.second > y) != (b.second > y) &&
        x < (b.first - a.first) * (y - a.second) / (b.second - a.second) + a.first) {
        result = !result;
      }
    }
    return result;
  }

  void onInitialize() override
  {
    declareParameter("file", rclcpp::ParameterValue(std::string("")));
    auto node = node_.lock();
    std::string file; node->get_parameter(name_ + ".file", file);
    enabled_ = !file.empty(); current_ = true;
    if (!enabled_) {return;}
    try {
      auto root = YAML::LoadFile(file);
      if (root["version"].as<int>() != 1 || root["frame"].as<std::string>() != "map" ||
        !root["regions"].IsSequence()) {throw std::runtime_error("expected version 1, frame map and regions list");}
      frame_ = "map";
      for (const auto & region : root["regions"]) {
        if (!region["vertices"].IsSequence() || region["vertices"].size() < 3) {
          throw std::runtime_error("region must have at least three vertices");
        }
        std::vector<Point> polygon;
        for (const auto & v : region["vertices"]) {
          if (!v.IsSequence() || v.size() != 2) {throw std::runtime_error("vertex must be [x, y]");}
          const double x = v[0].as<double>(), y = v[1].as<double>();
          if (!std::isfinite(x) || !std::isfinite(y)) {throw std::runtime_error("non-finite vertex");}
          polygon.emplace_back(x, y);
        }
        polygons_.push_back(std::move(polygon));
      }
      RCLCPP_WARN(logger_, "Passable regions loaded from %s: %zu regions ignore ALL obstacles", file.c_str(), polygons_.size());
    } catch (const std::exception & e) {
      throw std::runtime_error("Cannot load passable regions '" + file + "': " + e.what());
    }
  }
  void updateBounds(double, double, double, double * min_x, double * min_y, double * max_x, double * max_y) override
  {
    if (!enabled_ || polygons_.empty()) {return;}
    auto * grid = layered_costmap_->getCostmap();
    *min_x = std::min(*min_x, grid->getOriginX());
    *min_y = std::min(*min_y, grid->getOriginY());
    *max_x = std::max(*max_x, grid->getOriginX() + grid->getSizeInMetersX());
    *max_y = std::max(*max_y, grid->getOriginY() + grid->getSizeInMetersY());
  }
  void updateCosts(nav2_costmap_2d::Costmap2D & grid, int min_i, int min_j, int max_i, int max_j) override
  {
    if (!enabled_ || polygons_.empty()) {current_ = true; return;}
    double tx = 0, ty = 0, yaw = 0;
    const auto grid_frame = layered_costmap_->getGlobalFrameID();
    if (grid_frame != frame_) {
      try {
        auto t = tf_->lookupTransform(frame_, grid_frame, tf2::TimePointZero);
        tx = t.transform.translation.x; ty = t.transform.translation.y;
        yaw = tf2::getYaw(t.transform.rotation);
      } catch (const tf2::TransformException & e) {
        current_ = false;
        RCLCPP_WARN_THROTTLE(logger_, *clock_, 5000, "Passable layer TF unavailable: %s", e.what());
        return;
      }
    }
    current_ = true;
    const double c = std::cos(yaw), s = std::sin(yaw);
    for (int j = min_j; j < max_j; ++j) for (int i = min_i; i < max_i; ++i) {
      double x, y; grid.mapToWorld(i, j, x, y);
      const double mx = tx + c*x - s*y, my = ty + s*x + c*y;
      for (const auto & polygon : polygons_) {
        if (inside(mx, my, polygon)) {grid.setCost(i, j, nav2_costmap_2d::FREE_SPACE); break;}
      }
    }
  }
  void reset() override {current_ = true;}
  bool isClearable() override {return false;}
};
}
PLUGINLIB_EXPORT_CLASS(sentry_nav::PassableLayer, nav2_costmap_2d::Layer)
