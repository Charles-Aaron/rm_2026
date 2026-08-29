#include "set_nav_speed.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {

SetNavSpeedAction::SetNavSpeedAction(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SyncActionNode(name, config) {}

std::string SetNavSpeedAction::resolveControllerNode(const std::string &controller_node_input) const {
  if (!controller_node_input.empty()) {
    if (!controller_node_input.empty() && controller_node_input.front() == '/') {
      return controller_node_input;
    }
    std::string ns = node_->get_namespace();
    if (ns.empty() || ns == "/") {
      return "/" + controller_node_input;
    }
    return ns + "/" + controller_node_input;
  }

  std::string ns = node_->get_namespace();
  if (ns.empty() || ns == "/") {
    return "/controller_server";
  }
  return ns + "/controller_server";
}

BT::NodeStatus SetNavSpeedAction::tick() {
  if (!rclcpp::ok()) {
    return BT::NodeStatus::FAILURE;
  }

  node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
  if (!node_) {
    throw std::runtime_error("设置导航速度：无法从行为树黑板获取 ROS 节点");
  }

  double speed_mps = 0.9;
  if (!getInput("speed_mps", speed_mps)) {
    RCLCPP_WARN(node_->get_logger(), "设置导航速度：缺少 speed_mps，使用默认值 0.9");
    speed_mps = 0.9;
  }

  if (speed_mps <= 0.0) {
    RCLCPP_WARN(node_->get_logger(),
                "设置导航速度：速度 %.3f 无效，跳过设置并继续流程", speed_mps);
    return BT::NodeStatus::SUCCESS;
  }

  std::string controller_node_input;
  getInput("controller_node", controller_node_input);
  const std::string controller_node = resolveControllerNode(controller_node_input);
  const std::string service_name = controller_node + "/set_parameters";

  int timeout_ms = 500;
  getInput("service_timeout_ms", timeout_ms);
  if (timeout_ms <= 0) {
    timeout_ms = 500;
  }
  const auto timeout = std::chrono::milliseconds(timeout_ms);

  auto client = node_->create_client<rcl_interfaces::srv::SetParameters>(service_name);
  if (!client->wait_for_service(timeout)) {
    if (rclcpp::ok()) {
      RCLCPP_WARN(node_->get_logger(),
                  "设置导航速度：服务 %s 在 %d ms 内不可用，继续流程",
                  service_name.c_str(), timeout_ms);
    }
    return BT::NodeStatus::SUCCESS;
  }

  auto req = std::make_shared<rcl_interfaces::srv::SetParameters::Request>();
  req->parameters.reserve(6);
  // 当前 rm_2025 控制器(OmniPidPursuitController)实际生效的是 FollowPath.v_linear_*。
  req->parameters.emplace_back(
      rclcpp::Parameter("FollowPath.v_linear_max", speed_mps).to_parameter_msg());
  req->parameters.emplace_back(
      rclcpp::Parameter("FollowPath.v_linear_min", -speed_mps).to_parameter_msg());
  // 兼容旧参数名，存在则设置，不存在会在结果里返回 failed，不阻塞流程。
  req->parameters.emplace_back(rclcpp::Parameter("FollowPath.vx_max", speed_mps).to_parameter_msg());
  req->parameters.emplace_back(rclcpp::Parameter("FollowPath.vy_max", speed_mps).to_parameter_msg());
  req->parameters.emplace_back(
      rclcpp::Parameter("FollowPathOmni.v_linear_max", speed_mps).to_parameter_msg());
  req->parameters.emplace_back(
      rclcpp::Parameter("FollowPathOmni.v_linear_min", -speed_mps).to_parameter_msg());

  auto future = client->async_send_request(req);
  auto ret = rclcpp::spin_until_future_complete(node_, future, timeout);
  if (ret != rclcpp::FutureReturnCode::SUCCESS) {
    if (rclcpp::ok()) {
      RCLCPP_WARN(node_->get_logger(),
                  "设置导航速度：给 %s 设置参数超时或失败，继续流程",
                  controller_node.c_str());
    }
    return BT::NodeStatus::SUCCESS;
  }

  auto resp = future.get();
  int ok_count = 0;
  int fail_count = 0;
  for (const auto &result : resp->results) {
    if (!result.successful) {
      ++fail_count;
      RCLCPP_WARN(node_->get_logger(),
                  "设置导航速度：%s 参数设置失败: %s",
                  controller_node.c_str(), result.reason.c_str());
    } else {
      ++ok_count;
    }
  }

  if (ok_count > 0) {
    RCLCPP_INFO(node_->get_logger(),
                "设置导航速度：已将 %s 导航速度设置为 %.2f m/s（成功=%d，失败=%d）",
                controller_node.c_str(), speed_mps, ok_count, fail_count);
  } else {
    RCLCPP_WARN(node_->get_logger(),
                "设置导航速度：%s 没有接受 %.2f m/s 相关参数，继续流程",
                controller_node.c_str(), speed_mps);
  }

  return BT::NodeStatus::SUCCESS;
}

}  // namespace rm_sentry_decision

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<rm_sentry_decision::SetNavSpeedAction>("SetNavSpeedAction");
}
