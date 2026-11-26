#include "sentry_terrain/terrain_core.hpp"

#include <cmath>
#include <stdexcept>

namespace sentry_terrain {
TerrainConfig::TerrainConfig() {
  projection.unknown_as_occupied = false;
  projection.min_observed_voxels = 2;
  projection.surface_height_delta_max = .25;
  projection.wall_height_delta_min = .50;
  projection.wall_occupancy_ratio_min = .80;
  projection.tunnel_enable = true;
  projection.tunnel_height_delta_min = .26;
  projection.tunnel_height_delta_max = .60;
  projection.tunnel_occupancy_ratio_max = .60;
  projection.passable_as_free = true;
  projection.hysteresis_en = false;
  projection.obstacle_hold_time = 1.5;
  projection.mask_filter_en = true;
  projection.fill_occ_min = 8;
  projection.denoise_occ_max = 0;
}

void TerrainConfig::validate() const {
  const auto & p = projection;
  for (double v : {resolution, sliding_threshold, z_min, z_max, ray_min, ray_max,
      p_hit, p_miss, p_min, p_max, p_occ, p_free, keep_time, clear_time,
      p.surface_height_delta_max, p.wall_height_delta_min, p.wall_occupancy_ratio_min,
      p.tunnel_height_delta_min, p.tunnel_height_delta_max, p.tunnel_occupancy_ratio_max,
      p.obstacle_hold_time}) {
    if (!std::isfinite(v)) throw std::invalid_argument("terrain parameters must be finite");
  }
  if (resolution <= 0 || !map_size.allFinite() || map_size.minCoeff() < 3 * resolution ||
      sliding_threshold < 0 || z_max <= z_min ||
      map_size.z() < z_max - z_min + 2 * (sliding_threshold + resolution) ||
      ray_min <= 0 || ray_max <= ray_min || clear_time <= keep_time || keep_time < 0 ||
      !(0 < p_min && p_min < p_free && p_free < .5 && .5 < p_occ && p_occ < p_max && p_max < 1) ||
      !(0 < p_miss && p_miss < .5 && .5 < p_hit && p_hit < 1) ||
      p.min_observed_voxels < 1 || p.surface_height_delta_max < 0 ||
      p.wall_height_delta_min <= p.surface_height_delta_max ||
      p.tunnel_height_delta_min <= p.surface_height_delta_max ||
      p.tunnel_height_delta_max < p.tunnel_height_delta_min ||
      p.tunnel_occupancy_ratio_max < 0 || p.wall_occupancy_ratio_min > 1 ||
      p.tunnel_occupancy_ratio_max >= p.wall_occupancy_ratio_min ||
      p.obstacle_hold_time < 0 || p.fill_occ_min < 1 || p.fill_occ_min > 8 ||
      p.denoise_occ_max < 0 || p.denoise_occ_max >= p.fill_occ_min) {
    throw std::invalid_argument("invalid terrain geometry/probability/projection parameters");
  }
}

bool TerrainSnapshot::occupied(double x, double y) const {
  if (resolution <= 0 || !std::isfinite(x) || !std::isfinite(y)) return false;
  const int mx = static_cast<int>(std::floor((x - origin.x()) / resolution));
  const int my = static_cast<int>(std::floor((y - origin.y()) / resolution));
  return mx >= 0 && my >= 0 && mx < width && my < height &&
    types.at(my * width + mx) == rog_map::CellType::OCCUPIED;
}

TerrainCore::TerrainCore(const TerrainConfig & config) : config_(config) {
  config_.validate();
  // 不调用完整 ROG ROS 参数加载器：避免创建 ESDF、先验融合及百万级近邻查找表。
  cfg_.resolution = cfg_.inflation_resolution = config.resolution;
  cfg_.map_size_d = cfg_.local_update_box_d = config.map_size;
  cfg_.fix_map_origin.setZero();
  cfg_.map_sliding_en = true;
  cfg_.map_sliding_thresh = config.sliding_threshold;
  cfg_.virtual_ground_height = config.z_min;
  cfg_.virtual_ceil_height = config.z_max;
  cfg_.unk_thresh = .7;
  cfg_.inflation_step = 0;
  cfg_.inf_spherical_neighbor.emplace_back(0, 0, 0);
  cfg_.point_filt_num = cfg_.batch_update_size = 1;
  cfg_.intensity_thresh = -1;
  cfg_.parallel_raycast_en = false;
  cfg_.raycast_range_min = config.ray_min;
  cfg_.raycast_range_max = config.ray_max;
  cfg_.sqr_raycast_range_min = config.ray_min * config.ray_min;
  cfg_.sqr_raycast_range_max = config.ray_max * config.ray_max;
  const auto logit = [](double p) {return static_cast<float>(std::log(p / (1 - p)));};
  cfg_.l_hit = logit(config.p_hit); cfg_.l_miss = logit(config.p_miss);
  cfg_.l_min = logit(config.p_min); cfg_.l_max = logit(config.p_max);
  cfg_.l_occ = logit(config.p_occ); cfg_.l_free = logit(config.p_free);
  cfg_.decay_en = config.decay_enable;
  cfg_.decay_active_list_en = config.decay_active_list;
  cfg_.keep_time = config.keep_time;
  cfg_.decay_clear_time = config.clear_time;
  cfg_.resetMapSize();
  initProbMap();
}

void TerrainCore::integrate(const rog_map::PointCloud & cloud, const Eigen::Vector3d & lidar,
  const Eigen::Vector3d & chassis, double time) {
  if (!lidar.allFinite() || !chassis.allFinite() || !std::isfinite(time))
    throw std::invalid_argument("invalid terrain input pose/time");
  cfg_.virtual_ground_height = chassis.z() + config_.z_min;
  cfg_.virtual_ceil_height = chassis.z() + config_.z_max;
  if (lidar.z() <= cfg_.virtual_ground_height || lidar.z() >= cfg_.virtual_ceil_height)
    throw std::invalid_argument("lidar origin outside terrain height window");
  int low, high;
  posToGlobalIndex(cfg_.virtual_ground_height, low);
  posToGlobalIndex(cfg_.virtual_ceil_height, high);
  z_changed_ = z_changed_ || low != z_min_ || high != z_max_;
  z_min_ = low; z_max_ = high;
  sc_.virtual_ground_height_id_g = low;
  sc_.virtual_ceil_height_id_g = high;
  Eigen::Vector3d center = chassis;
  center.z() += .5 * (config_.z_min + config_.z_max);
  setUpdateTime(time);
  updateProbMap(cloud, {lidar, Eigen::Quaterniond::Identity()}, center);
  initialized_window_ = true;
}

TerrainSnapshot TerrainCore::project(double time) {
  if (!initialized_window_) return {};
  setUpdateTime(time);
  applyDecay(time);  // 遗忘不会改变上游源帧时间；UNKNOWN 后停止衰减，保持 BIT 原行为。
  const auto min_id = localMapMinIndex();
  const Eigen::Vector2d origin = localMapMinPosition().head<2>() -
    Eigen::Vector2d::Constant(.5 * config_.resolution);
  const auto slide = projection_.syncSlidingWindow(mapWidth(), mapHeight(), config_.resolution,
    min_id.head<2>(), origin, config_.projection);
  auto dirty = dirtyColumnIds();
  dirty.insert(dirty.end(), slide.dirty_columns.begin(), slide.dirty_columns.end());
  std::sort(dirty.begin(), dirty.end());
  dirty.erase(std::unique(dirty.begin(), dirty.end()), dirty.end());
  const int low = std::max(z_min_, localMapMinIndex().z());
  const int high = std::min(z_max_, localMapMaxIndex().z());
  const int depth = mapDepth();
  const auto scanner = [&](int x, int y) {
    rog_map::ColumnStats result;
    if (low > high) return result;
    // ROG 环形内存的 Z 连续。每柱仅计算一次哈希，之后顺序扫描并处理一次回绕。
    int hash = getHashIndexFromGlobalIndex(Eigen::Vector3i(min_id.x() + x, min_id.y() + y, low));
    const int end = (hash / depth + 1) * depth;
    for (int z = low; z <= high; ++z) {
      if (hash == end) hash -= depth;
      const int index = hash++;
      const float value = occupancy_buffer_[index];
      if (isOccupied(value) || isKnownFree(value)) {
        ++result.observed_count;
        result.last_update_time = std::max(result.last_update_time, last_update_time_[index]);
      }
      if (!isOccupied(value)) continue;
      ++result.occupied_count;
      result.occupied_z_index_min = std::min(result.occupied_z_index_min, z);
      result.occupied_z_index_max = std::max(result.occupied_z_index_max, z);
      const double world_z = (z + .5) * config_.resolution;
      result.occupied_z_min_abs = std::min(result.occupied_z_min_abs, world_z);
      result.occupied_z_max_abs = std::max(result.occupied_z_max_abs, world_z);
      result.last_hit_time = std::max(result.last_hit_time, last_hit_time_[index]);
    }
    return result;
  };
  const bool full = z_changed_ || fullLayerRefreshRequired() || slide.full_refresh_required ||
    dirty.size() > .3 * mapWidth() * mapHeight();
  projection_.updateDirty(mapWidth(), mapHeight(), config_.resolution, origin, time,
    config_.projection, scanner, dirty, full);
  projection_.advanceObstacleClearance(time, config_.projection);
  clearDirtyColumns();
  z_changed_ = false;
  TerrainSnapshot out;
  out.width = mapWidth(); out.height = mapHeight();
  out.origin = origin; out.resolution = config_.resolution;
  out.types.reserve(projection_.cells().size());
  out.height_delta.reserve(projection_.cells().size());
  out.top_z.reserve(projection_.cells().size());
  for (const auto & cell : projection_.cells()) {
    out.types.push_back(config_.projection.unknown_as_occupied && cell.type == rog_map::CellType::UNKNOWN
      ? rog_map::CellType::OCCUPIED : cell.type);
    out.height_delta.push_back(cell.height_delta);
    out.top_z.push_back(cell.occupied_z_max_abs);
  }
  return out;
}

rog_map::PointCloud TerrainCore::occupiedCloud() const {
  rog_map::PointCloud cloud;
  if (!initialized_window_) return cloud;
  for (int hash = 0; hash < static_cast<int>(occupancy_buffer_.size()); ++hash) {
    if (!isOccupied(occupancy_buffer_[hash])) continue;
    Eigen::Vector3i id;
    hashIdToGlobalIndex(hash, id);
    if (id.z() < z_min_ || id.z() > z_max_) continue;
    Eigen::Vector3d pos;
    globalIndexToPos(id, pos);
    cloud.push_back(pcl::PointXYZI(pos.x(), pos.y(), pos.z(), 0));
  }
  return cloud;
}
}  // namespace sentry_terrain
