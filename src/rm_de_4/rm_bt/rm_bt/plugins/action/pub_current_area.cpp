#include "rm_behavior_tree/plugins/action/pub_current_area.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/transform_listener.h"

namespace rm_behavior_tree
{

PubCurrentAreaAction::PubCurrentAreaAction(const std::string &name, const BT::NodeConfig &config)
: BT::SyncActionNode(name, config),
  node_(rclcpp::Node::make_shared("pub_current_area")),
  tf_buffer_(std::make_shared<tf2_ros::Buffer>(node_->get_clock())),
  tf_listener_(*tf_buffer_)
{
}

bool PubCurrentAreaAction::isPointInPolygon(const Point &point, const std::vector<Point> &polygon)
{
  int n = polygon.size();
  bool inside = false;
  for (int i = 0, j = n - 1; i < n; j = i++)
  {
    if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
        (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
    {
      inside = !inside;
    }
  }
  return inside;
}

double PubCurrentAreaAction::calculateDistance(const Point &p1, const Point &p2)
{
  return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

BT::NodeStatus PubCurrentAreaAction::tick()
{
  try
  {
    geometry_msgs::msg::TransformStamped transformStamped;
    try
    {
      // Use a timeout for transform lookup
      transformStamped = tf_buffer_->lookupTransform(
          "map", "base_link", tf2::TimePointZero, tf2::durationFromSec(0.5));
    }
    catch (tf2::TransformException &ex)
    {
      RCLCPP_WARN(node_->get_logger(), "Could not transform base_link to map: %s", ex.what());
      return BT::NodeStatus::FAILURE;
    }

    Point robot_position{transformStamped.transform.translation.x, transformStamped.transform.translation.y};

    std::vector<std::vector<Point>> areas(3);
    for (int i = 0; i < 3; ++i)
    {
      std::string area_key = "area_" + std::to_string(i + 1);
      std::string area_value;
      if (!getInput(area_key, area_value) || area_value == "{-1}")
      {
        RCLCPP_WARN(node_->get_logger(), "Input for %s is missing or default: %s", area_key.c_str(), area_value.c_str());
        continue;
      }

      std::vector<Point> area_points;
      size_t start = 0, end = 0;
      try
      {
        while ((start = area_value.find('{', start)) != std::string::npos)
        {
          start++; // Skip '{'
          end = area_value.find(':', start);
          if (end == std::string::npos)
          {
            throw std::invalid_argument("Invalid area format: missing ':'");
          }
          // Skip the index (e.g., "1" in "{1:x1,y1}")
          start = end + 1;

          end = area_value.find(',', start);
          if (end == std::string::npos)
          {
            throw std::invalid_argument("Invalid area format: missing ','");
          }
          double x = std::stod(area_value.substr(start, end - start));
          start = end + 1;

          end = area_value.find('}', start);
          if (end == std::string::npos)
          {
            throw std::invalid_argument("Invalid area format: missing '}'");
          }
          double y = std::stod(area_value.substr(start, end - start));
          start = end + 1;

          area_points.push_back({x, y});
        }
      }
      catch (const std::exception &e)
      {
        RCLCPP_ERROR(node_->get_logger(), "Error parsing %s: %s. Input was: %s", area_key.c_str(), e.what(), area_value.c_str());
        return BT::NodeStatus::FAILURE;
      }
      areas[i] = area_points;
    }

    int current_area = -1;
    double min_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < 3; ++i)
    {
      if (areas[i].empty())
      {
        continue;
      }

      if (isPointInPolygon(robot_position, areas[i]))
      {
        current_area = i + 1;
        break;
      }

      for (const auto &point : areas[i])
      {
        double distance = calculateDistance(robot_position, point);
        if (distance < min_distance)
        {
          min_distance = distance;
          current_area = i + 1;
        }
      }
    }

    setOutput("current_area", std::to_string(current_area)); // Ensure output is a string
    RCLCPP_INFO(node_->get_logger(), "机器人当前所处区域: %s 区", std::to_string(current_area).c_str());
    return BT::NodeStatus::SUCCESS;
  }
  catch (const std::exception &e)
  {
    RCLCPP_ERROR(node_->get_logger(), "Unexpected error: %s", e.what());
    //return BT::NodeStatus::FAILURE;
    return BT::NodeStatus::SUCCESS;
  }
}

}  // namespace rm_behavior_tree

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(rm_behavior_tree::PubCurrentAreaAction, "GetCurrentArea");

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<rm_behavior_tree::PubCurrentAreaAction>("PubCurrentAreaAction", BT::RosNodeParams{});
}