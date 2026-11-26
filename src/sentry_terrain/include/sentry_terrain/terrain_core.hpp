#pragma once

#include <rog_map/prob_map.h>
#include <rog_map/projection_layer.hpp>

namespace sentry_terrain {

struct TerrainConfig {
  double resolution{0.05};
  Eigen::Vector3d map_size{10.5, 10.5, 2.2};
  double sliding_threshold{0.20};
  double z_min{-0.20}, z_max{1.50};  // 相对底盘原点，沿 odom 的 Z 轴。
  double ray_min{0.30}, ray_max{10.0};
  double p_hit{0.9}, p_miss{0.45}, p_min{0.12}, p_max{0.97};
  double p_occ{0.85}, p_free{0.499};
  bool decay_enable{true}, decay_active_list{false};
  double keep_time{0.5}, clear_time{1.2};
  rog_map::ProjectionLayerConfig projection;
  TerrainConfig();
  void validate() const;
};

struct TerrainSnapshot {
  int width{0}, height{0};
  double resolution{0};
  Eigen::Vector2d origin{0, 0};  // 栅格外角；查询必须使用世界坐标。
  std::vector<rog_map::CellType> types;
  std::vector<float> height_delta, top_z;
  bool occupied(double x, double y) const;
};

// 仅后台线程访问体素；代价地图线程只读不可变二维快照。
class TerrainCore : public rog_map::ProbMap {
public:
  explicit TerrainCore(const TerrainConfig & config);
  void integrate(const rog_map::PointCloud & world_cloud, const Eigen::Vector3d & lidar,
    const Eigen::Vector3d & chassis, double time);
  TerrainSnapshot project(double time);
  rog_map::PointCloud occupiedCloud() const;
  const TerrainConfig & config() const {return config_;}
  const rog_map::ProjectionLayer & projection() const {return projection_;}

private:
  TerrainConfig config_;
  rog_map::ProjectionLayer projection_;
  int z_min_{0}, z_max_{-1};
  bool initialized_window_{false}, z_changed_{true};
};
}  // namespace sentry_terrain
