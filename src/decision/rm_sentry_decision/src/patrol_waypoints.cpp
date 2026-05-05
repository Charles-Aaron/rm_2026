#include "patrol_waypoints.hpp"

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "tf2/exceptions.h"
#include "tf2/time.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace rm_sentry_decision {
namespace {

std::string trim(std::string value)
{
  const auto begin = value.find_first_not_of(" \t\r\n");
  if (begin == std::string::npos) {
    return "";
  }
  const auto end = value.find_last_not_of(" \t\r\n");
  return value.substr(begin, end - begin + 1);
}

std::string resolveWaypointFile(const std::string &waypoint_file)
{
  namespace fs = std::filesystem;
  fs::path path(waypoint_file);
  if (path.is_absolute() && fs::exists(path)) {
    return path.string();
  }

  if (!path.is_absolute()) {
    const auto share_dir = ament_index_cpp::get_package_share_directory("rm_sentry_decision");
    fs::path config_path = fs::path(share_dir) / "config" / path;
    if (fs::exists(config_path)) {
      return config_path.string();
    }
  }

  return waypoint_file;
}

bool loadWaypointsFromCsv(
  const std::string &filename,
  const std::string &frame_id,
  std::vector<geometry_msgs::msg::PoseStamped> &waypoints,
  std::vector<double> &wait_times)
{
  std::ifstream file(resolveWaypointFile(filename));
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  bool first_line = true;
  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty()) {
      continue;
    }

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
      tokens.push_back(trim(token));
    }

    if (first_line) {
      first_line = false;
      if (!tokens.empty() && tokens[0] == "id") {
        continue;
      }
    }

    if (tokens.size() < 8) {
      continue;
    }

    try {
      geometry_msgs::msg::PoseStamped pose;
      pose.header.frame_id = frame_id;
      pose.header.stamp = rclcpp::Time(0);
      pose.pose.position.x = std::stod(tokens[1]);
      pose.pose.position.y = std::stod(tokens[2]);
      pose.pose.position.z = std::stod(tokens[3]);
      pose.pose.orientation.x = std::stod(tokens[4]);
      pose.pose.orientation.y = std::stod(tokens[5]);
      pose.pose.orientation.z = std::stod(tokens[6]);
      pose.pose.orientation.w = std::stod(tokens[7]);

      double wait_sec = 0.0;
      try {
        if (tokens.size() > 9 && !tokens[9].empty()) {
          wait_sec = std::stod(tokens[9]);
        } else if (tokens.size() == 9 && !tokens[8].empty()) {
          wait_sec = std::stod(tokens[8]);
        }
      } catch (const std::exception &) {
        wait_sec = 0.0;
      }

      waypoints.push_back(pose);
      wait_times.push_back(wait_sec);
    } catch (const std::exception &) {
      continue;
    }
  }

  return !waypoints.empty();
}

std::string poseToString(const geometry_msgs::msg::PoseStamped &pose)
{
  std::ostringstream oss;
  oss << pose.pose.position.x << "; "
      << pose.pose.position.y << "; "
      << pose.pose.position.z << "; "
      << pose.pose.orientation.x << "; "
      << pose.pose.orientation.y << "; "
      << pose.pose.orientation.z << "; "
      << pose.pose.orientation.w;
  return oss.str();
}

}  // namespace

LoadWaypoints::LoadWaypoints(const std::string &name, const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList LoadWaypoints::providedPorts()
{
  return {
    BT::InputPort<std::string>("waypoint_file", "CSV 巡逻点文件，可填绝对路径或 config 下相对路径"),
    BT::InputPort<std::string>("frame_id", "map", "巡逻点坐标系"),
    BT::OutputPort<std::vector<geometry_msgs::msg::PoseStamped>>("waypoints"),
    BT::OutputPort<std::vector<double>>("wait_times"),
    BT::OutputPort<int>("total_waypoints"),
    BT::OutputPort<int>("wp_idx")
  };
}

BT::NodeStatus LoadWaypoints::tick()
{
  std::string waypoint_file;
  if (!getInput("waypoint_file", waypoint_file) || waypoint_file.empty()) {
    RCLCPP_ERROR(rclcpp::get_logger("LoadWaypoints"), "巡逻点加载：缺少 waypoint_file");
    return BT::NodeStatus::FAILURE;
  }

  std::string frame_id = "map";
  getInput("frame_id", frame_id);

  std::vector<geometry_msgs::msg::PoseStamped> waypoints;
  std::vector<double> wait_times;
  if (!loadWaypointsFromCsv(waypoint_file, frame_id, waypoints, wait_times)) {
    RCLCPP_ERROR(
      rclcpp::get_logger("LoadWaypoints"),
      "巡逻点加载：无法从 %s 加载巡逻点", waypoint_file.c_str());
    return BT::NodeStatus::FAILURE;
  }

  setOutput("waypoints", waypoints);
  setOutput("wait_times", wait_times);
  setOutput("total_waypoints", static_cast<int>(waypoints.size()));
  setOutput("wp_idx", 0);
  RCLCPP_INFO(
    rclcpp::get_logger("LoadWaypoints"),
    "巡逻点加载：已从 %s 加载 %zu 个巡逻点", waypoint_file.c_str(), waypoints.size());
  return BT::NodeStatus::SUCCESS;
}

GetCurrentWaypoint::GetCurrentWaypoint(
  const std::string &name,
  const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList GetCurrentWaypoint::providedPorts()
{
  return {
    BT::InputPort<std::vector<geometry_msgs::msg::PoseStamped>>("waypoints"),
    BT::InputPort<std::vector<double>>("wait_times"),
    BT::InputPort<int>("wp_idx"),
    BT::OutputPort<geometry_msgs::msg::PoseStamped>("current_goal"),
    BT::OutputPort<std::string>("current_goal_pose"),
    BT::OutputPort<double>("current_wait_sec")
  };
}

BT::NodeStatus GetCurrentWaypoint::tick()
{
  auto waypoints_res = getInput<std::vector<geometry_msgs::msg::PoseStamped>>("waypoints");
  auto wait_times_res = getInput<std::vector<double>>("wait_times");
  auto idx_res = getInput<int>("wp_idx");
  if (!waypoints_res || !idx_res) {
    RCLCPP_ERROR(rclcpp::get_logger("GetCurrentWaypoint"), "巡逻点读取：无法读取巡逻点端口");
    return BT::NodeStatus::FAILURE;
  }

  const auto &waypoints = waypoints_res.value();
  const int idx = idx_res.value();
  if (waypoints.empty() || idx < 0 || idx >= static_cast<int>(waypoints.size())) {
    RCLCPP_ERROR(
      rclcpp::get_logger("GetCurrentWaypoint"),
      "巡逻点读取：索引越界 idx=%d 总数=%zu", idx, waypoints.size());
    return BT::NodeStatus::FAILURE;
  }

  auto goal = waypoints[idx];
  goal.header.stamp = rclcpp::Clock().now();

  double wait_sec = 0.0;
  if (wait_times_res && idx < static_cast<int>(wait_times_res.value().size())) {
    wait_sec = wait_times_res.value()[idx];
  }

  setOutput("current_goal", goal);
  setOutput("current_goal_pose", poseToString(goal));
  setOutput("current_wait_sec", wait_sec);
  return BT::NodeStatus::SUCCESS;
}

WaitUntilReached::WaitUntilReached(const std::string &name, const BT::NodeConfiguration &config)
: BT::StatefulActionNode(name, config)
{
  node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("等待到达目标：无法从行为树黑板获取 ROS 节点");
  }
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(node_->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_, node_, false);
}

BT::PortsList WaitUntilReached::providedPorts()
{
  return {
    BT::InputPort<geometry_msgs::msg::PoseStamped>("goal_pose"),
    BT::InputPort<double>("tolerance", 0.5, "到达判定距离阈值"),
    BT::InputPort<std::string>("global_frame", "map", "全局坐标系"),
    BT::InputPort<std::string>("robot_frame", "base_link", "机器人坐标系")
  };
}

BT::NodeStatus WaitUntilReached::onStart()
{
  auto goal_res = getInput<geometry_msgs::msg::PoseStamped>("goal_pose");
  if (!goal_res) {
    RCLCPP_ERROR(node_->get_logger(), "等待到达目标：缺少 goal_pose");
    return BT::NodeStatus::FAILURE;
  }

  goal_ = goal_res.value();
  getInput("tolerance", tolerance_);
  getInput("global_frame", global_frame_);
  getInput("robot_frame", robot_frame_);
  if (tolerance_ < 0.0) {
    tolerance_ = 0.0;
  }

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus WaitUntilReached::onRunning()
{
  geometry_msgs::msg::TransformStamped tf_msg;
  try {
    tf_msg = tf_buffer_->lookupTransform(
      global_frame_, robot_frame_, tf2::TimePointZero, tf2::durationFromSec(0.05));
  } catch (const tf2::TransformException &ex) {
    RCLCPP_WARN_THROTTLE(
      node_->get_logger(), *node_->get_clock(), 1000,
      "等待到达目标：查询 TF %s -> %s 失败: %s",
      global_frame_.c_str(), robot_frame_.c_str(), ex.what());
    return BT::NodeStatus::RUNNING;
  }

  const double dx = tf_msg.transform.translation.x - goal_.pose.position.x;
  const double dy = tf_msg.transform.translation.y - goal_.pose.position.y;
  const double distance = std::sqrt(dx * dx + dy * dy);
  if (distance <= tolerance_) {
    return BT::NodeStatus::SUCCESS;
  }
  return BT::NodeStatus::RUNNING;
}

void WaitUntilReached::onHalted()
{
  RCLCPP_DEBUG(node_->get_logger(), "等待到达目标：被中断");
}

WaitDuration::WaitDuration(const std::string &name, const BT::NodeConfiguration &config)
: BT::StatefulActionNode(name, config)
{}

BT::PortsList WaitDuration::providedPorts()
{
  return {
    BT::InputPort<double>("duration_sec", 0.0, "等待时间，单位秒")
  };
}

BT::NodeStatus WaitDuration::onStart()
{
  getInput("duration_sec", duration_sec_);
  if (duration_sec_ <= 0.0) {
    return BT::NodeStatus::SUCCESS;
  }
  start_time_ = std::chrono::steady_clock::now();
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus WaitDuration::onRunning()
{
  const auto elapsed = std::chrono::duration<double>(
    std::chrono::steady_clock::now() - start_time_).count();
  return elapsed >= duration_sec_ ? BT::NodeStatus::SUCCESS : BT::NodeStatus::RUNNING;
}

void WaitDuration::onHalted() {}

NextWaypoint::NextWaypoint(const std::string &name, const BT::NodeConfiguration &config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList NextWaypoint::providedPorts()
{
  return {
    BT::BidirectionalPort<int>("wp_idx"),
    BT::InputPort<int>("total_waypoints")
  };
}

BT::NodeStatus NextWaypoint::tick()
{
  auto idx_res = getInput<int>("wp_idx");
  auto total_res = getInput<int>("total_waypoints");
  if (!idx_res || !total_res || total_res.value() <= 0) {
    RCLCPP_ERROR(rclcpp::get_logger("NextWaypoint"), "切换巡逻点：索引或总数无效");
    return BT::NodeStatus::FAILURE;
  }

  const int next = (idx_res.value() + 1) % total_res.value();
  setOutput("wp_idx", next);
  return BT::NodeStatus::SUCCESS;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::LoadWaypoints>("LoadWaypoints");
  factory.registerNodeType<rm_sentry_decision::GetCurrentWaypoint>("GetCurrentWaypoint");
  factory.registerNodeType<rm_sentry_decision::WaitUntilReached>("WaitUntilReached");
  factory.registerNodeType<rm_sentry_decision::WaitDuration>("WaitDuration");
  factory.registerNodeType<rm_sentry_decision::NextWaypoint>("NextWaypoint");
}
