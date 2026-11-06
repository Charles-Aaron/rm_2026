#include "sentry_omni_kino_planner/grid_map.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <yaml-cpp/yaml.h>

namespace sentry_omni_kino_planner {

void GridMap::validate() const {
  if (width <= 0 || height <= 0 || width > 20000 || height > 20000 ||
    static_cast<int64_t>(width) * height > 10000000 ||
    !std::isfinite(resolution) || resolution < 1e-4 || !origin.allFinite() ||
    origin.cwiseAbs().maxCoeff() > 1e6 || frame.empty() ||
    occupancy.size() != static_cast<size_t>(width) * static_cast<size_t>(height))
  {
    throw std::invalid_argument("invalid map geometry, frame, or data size (maximum 10M cells)");
  }
  if (std::any_of(occupancy.begin(), occupancy.end(), [](int8_t v) {return v < -1 || v > 100;})) {
    throw std::invalid_argument("map data must use standard occupancy -1 or 0..100");
  }
  if (!traversal_cost.empty() && (traversal_cost.size() != occupancy.size() ||
    std::any_of(traversal_cost.begin(), traversal_cost.end(), [](uint8_t v) {return v == 255;})))
  {throw std::invalid_argument("soft cost must match map size and use 0..254");}
}

GridMap::CostSample GridMap::sample_cost(const Eigen::Vector2d & point) const {
  if (traversal_cost.empty()) {return {};}
  if (!point.allFinite()) {throw std::invalid_argument("nonfinite soft cost query");}
  // Values live at cell centres. Replicate border values; collision validation
  // remains responsible for rejecting trajectories outside the map footprint.
  const Eigen::Vector2d q = (point - origin) / resolution - Eigen::Vector2d::Constant(0.5);
  const double x = std::clamp(q.x(), 0.0, double(width - 1));
  const double y = std::clamp(q.y(), 0.0, double(height - 1));
  const int x0 = int(std::floor(x)), y0 = int(std::floor(y));
  const int x1 = std::min(x0 + 1, width - 1), y1 = std::min(y0 + 1, height - 1);
  const double u = x - x0, v = y - y0;
  const double a = traversal_cost[index(x0, y0)] / 254.0;
  const double b = traversal_cost[index(x1, y0)] / 254.0;
  const double c = traversal_cost[index(x0, y1)] / 254.0;
  const double d = traversal_cost[index(x1, y1)] / 254.0;
  CostSample out;
  out.value = (1 - v) * ((1 - u) * a + u * b) + v * ((1 - u) * c + u * d);
  if (q.x() >= 0 && q.x() < width - 1) {
    out.gradient.x() = ((1 - v) * (b - a) + v * (d - c)) / resolution;
  }
  if (q.y() >= 0 && q.y() < height - 1) {
    out.gradient.y() = ((1 - u) * (c - a) + u * (d - b)) / resolution;
  }
  return out;
}

std::optional<Eigen::Vector2i> GridMap::cell(const Eigen::Vector2d & point) const {
  const Eigen::Vector2d local = (point - origin) / resolution;
  if (!local.allFinite() || local.x() < 0 || local.y() < 0 ||
    local.x() >= width || local.y() >= height)
  {
    return std::nullopt;
  }
  return Eigen::Vector2i(static_cast<int>(std::floor(local.x())),
           static_cast<int>(std::floor(local.y())));
}

size_t GridMap::index(int x, int y) const {
  return static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
}

bool PreparedMap::is_blocked(const Eigen::Vector2d & point) const {
  const auto c = source.cell(point);
  return !c || blocked[source.index(c->x(), c->y())] != 0;
}

PreparedMap prepare_map(GridMap map, const MapOptions & options) {
  map.validate();
  if (options.occupied_threshold < 1 || options.occupied_threshold > 100 ||
    !std::isfinite(options.robot_radius) || options.robot_radius < 0)
  {
    throw std::invalid_argument("invalid occupied threshold or robot radius");
  }
  PreparedMap result;
  result.source = std::move(map);
  const auto & m = result.source;
  result.blocked.resize(m.occupancy.size());
  cv::Mat free(m.height, m.width, CV_8UC1);
  bool any_obstacle = false;
  for (int y = 0; y < m.height; ++y) {
    for (int x = 0; x < m.width; ++x) {
      const auto i = m.index(x, y);
      const int value = m.occupancy[i];
      const bool blocked = value >= options.occupied_threshold ||
        (value < 0 && options.unknown_is_obstacle);
      result.blocked[i] = blocked;
      free.at<uint8_t>(y, x) = blocked ? 0 : 1;
      any_obstacle |= blocked;
    }
  }
  if (options.robot_radius == 0) {return result;}
  cv::Mat distances;
  if (any_obstacle) {
    cv::distanceTransform(free, distances, cv::DIST_L2, cv::DIST_MASK_PRECISE);
  }
  // Conservative cell-based Minkowski inflation: account for occupied and query
  // cell half-diagonals. The entire query cell is safe for a circular footprint.
  const double obstacle_clearance = options.robot_radius + std::sqrt(2.0) * m.resolution;
  for (int y = 0; y < m.height; ++y) {
    for (int x = 0; x < m.width; ++x) {
      const double boundary_clearance = std::min({x * m.resolution, y * m.resolution,
        (m.width - x - 1) * m.resolution, (m.height - y - 1) * m.resolution});
      if (boundary_clearance < options.robot_radius ||
        (any_obstacle && distances.at<float>(y, x) * m.resolution <= obstacle_clearance))
      {
        result.blocked[m.index(x, y)] = 1;
      }
    }
  }
  return result;
}

GridMap load_yaml_map(const std::string & filename, const std::string & frame) {
  const auto yaml = YAML::LoadFile(filename);
  const std::string mode = yaml["mode"] ? yaml["mode"].as<std::string>() : "trinary";
  if (mode != "trinary") {throw std::invalid_argument("only trinary YAML maps are supported");}
  const auto origin = yaml["origin"].as<std::vector<double>>();
  if (origin.size() != 3 || !std::isfinite(origin[2]) || std::abs(origin[2]) > 1e-9) {
    throw std::invalid_argument("map origin must be [x,y,0]; rotated maps are not supported");
  }
  const double occupied = yaml["occupied_thresh"].as<double>();
  const double free_threshold = yaml["free_thresh"].as<double>();
  if (!std::isfinite(occupied) || !std::isfinite(free_threshold) ||
    free_threshold < 0 || occupied > 1 || free_threshold >= occupied)
  {
    throw std::invalid_argument("invalid map occupancy thresholds");
  }
  const int negate = yaml["negate"].as<int>();
  if (negate != 0 && negate != 1) {throw std::invalid_argument("negate must be 0 or 1");}
  auto image_path = std::filesystem::path(yaml["image"].as<std::string>());
  if (image_path.is_relative()) {
    image_path = std::filesystem::path(filename).parent_path() / image_path;
  }
  const auto image = cv::imread(image_path.string(), cv::IMREAD_UNCHANGED);
  if (image.empty() || image.depth() != CV_8U ||
    (image.channels() != 1 && image.channels() != 3 && image.channels() != 4))
  {
    throw std::invalid_argument("map image must be readable 8-bit grayscale, RGB, or RGBA");
  }
  GridMap map;
  map.width = image.cols;
  map.height = image.rows;
  map.resolution = yaml["resolution"].as<double>();
  map.origin = {origin[0], origin[1]};
  map.frame = frame;
  if (static_cast<int64_t>(map.width) * map.height > 10000000) {
    throw std::invalid_argument("map image exceeds 10M cells");
  }
  map.occupancy.resize(static_cast<size_t>(map.width) * map.height);
  for (int y = 0; y < map.height; ++y) {
    const uint8_t * row = image.ptr<uint8_t>(map.height - y - 1);
    for (int x = 0; x < map.width; ++x) {
      const int channels = image.channels();
      const int colors = channels == 4 ? 3 : channels;
      double brightness = 0;
      for (int c = 0; c < colors; ++c) {brightness += row[x * channels + c];}
      brightness /= colors * 255.0;
      const double probability = negate ? brightness : 1.0 - brightness;
      const bool transparent = channels == 4 && row[x * channels + 3] < 255;
      map.occupancy[map.index(x, y)] = transparent ? -1 :
        probability > occupied ? 100 : probability < free_threshold ? 0 : -1;
    }
  }
  map.validate();
  return map;
}

GridMap make_demo_map(const std::string & scenario, const std::string & frame) {
  GridMap map;
  map.width = 120;
  map.height = 100;
  map.resolution = 0.1;
  map.origin = {-6.0, -5.0};
  map.frame = frame;
  map.occupancy.assign(12000, 0);
  auto rectangle = [&](int x0, int y0, int x1, int y1) {
      for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {map.occupancy[map.index(x, y)] = 100;}
      }
    };
  if (scenario == "obstacles") {
    rectangle(57, 15, 63, 62);
    rectangle(24, 63, 40, 72);
    rectangle(80, 22, 92, 37);
  } else if (scenario == "blocked") {
    rectangle(57, 0, 63, 100);
  } else if (scenario == "narrow") {
    rectangle(57, 0, 63, 44);
    rectangle(57, 56, 63, 100);
  } else if (scenario != "open") {
    throw std::invalid_argument("scenario must be obstacles, open, blocked, or narrow");
  }
  return map;
}

}  // namespace sentry_omni_kino_planner
