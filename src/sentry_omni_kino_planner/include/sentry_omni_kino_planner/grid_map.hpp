#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <Eigen/Core>

namespace sentry_omni_kino_planner {

// Row-major, bottom row first, matching standard ROS OccupancyGrid semantics.
struct GridMap {
  int width = 0;
  int height = 0;
  double resolution = 0.1;
  Eigen::Vector2d origin = Eigen::Vector2d::Zero();
  std::string frame = "map";
  std::vector<int8_t> occupancy;  // -1 unknown, 0..100 occupancy, never raw costs.
  // Optional Nav2 soft costs: 0..254; unknown is explicitly mapped to 254.
  // Kept separate from occupancy and footprint collision inflation.
  std::vector<uint8_t> traversal_cost;
  struct CostSample {
    double value = 0;  // Normalized to [0, 1].
    Eigen::Vector2d gradient = Eigen::Vector2d::Zero();  // Per metre, map axes.
  };
  CostSample sample_cost(const Eigen::Vector2d & point) const;

  void validate() const;
  std::optional<Eigen::Vector2i> cell(const Eigen::Vector2d & point) const;
  size_t index(int x, int y) const;
};

struct MapOptions {
  int occupied_threshold = 65;
  bool unknown_is_obstacle = true;
  double robot_radius = 0.25;
};

struct PreparedMap {
  GridMap source;
  std::vector<uint8_t> blocked;  // 0 free, 1 blocked, including footprint inflation.
  bool is_blocked(const Eigen::Vector2d & point) const;
};

PreparedMap prepare_map(GridMap map, const MapOptions & options);
GridMap load_yaml_map(const std::string & filename, const std::string & frame = "map");
GridMap make_demo_map(const std::string & scenario, const std::string & frame = "map");

}  // namespace sentry_omni_kino_planner
