#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/decorator_node.h>
#include <nav2_behavior_tree/bt_service_node.hpp>
#include <sentry_nav_interfaces/srv/ordered_route.hpp>
#include <atomic>
#include <chrono>

namespace sentry_nav {
using Goals = std::vector<geometry_msgs::msg::PoseStamped>;
using Service = sentry_nav_interfaces::srv::OrderedRoute;

// A task identity survives replans and recoveries, but not cancellation/completion
// or a replacement goal list. In particular, equal final poses are not identities.
class OrderedNavigation : public BT::DecoratorNode {
  Goals goals_;
  bool started_ = false;
public:
  OrderedNavigation(const std::string & name, const BT::NodeConfiguration & config)
  : BT::DecoratorNode(name, config) {}
  static BT::PortsList providedPorts() {
    return {BT::InputPort<Goals>("goals"), BT::OutputPort<std::string>("task_id")};
  }
  BT::NodeStatus tick() override {
    Goals goals;
    if (!getInput("goals", goals) || goals.empty()) {return BT::NodeStatus::FAILURE;}
    if (!started_ || goals != goals_) {
      if (started_) {haltChild();}
      static std::atomic<uint64_t> serial{0};
      const auto time = std::chrono::steady_clock::now().time_since_epoch().count();
      setOutput("task_id", std::to_string(time) + "-" + std::to_string(++serial));
      goals_ = goals; started_ = true;
    }
    setStatus(BT::NodeStatus::RUNNING);
    const auto result = child_node_->executeTick();
    if (result != BT::NodeStatus::RUNNING) {resetChild(); started_ = false;}
    return result;
  }
  void halt() override {started_ = false; BT::DecoratorNode::halt();}
};

class GetRemainingGoals : public nav2_behavior_tree::BtServiceNode<Service> {
public:
  GetRemainingGoals(const std::string & name, const BT::NodeConfiguration & config)
  : BtServiceNode(name, config) {}
  static BT::PortsList providedPorts() {
    return providedBasicPorts({BT::InputPort<Goals>("goals"), BT::InputPort<std::string>("task_id"),
      BT::OutputPort<Goals>("remaining_goals"), BT::OutputPort<unsigned>("first_index")});
  }
  void on_tick() override {
    *request_ = Service::Request(); request_->operation = Service::Request::QUERY;
    should_send_request_ = bool(getInput("goals", request_->goals)) &&
      bool(getInput("task_id", request_->task_id));
  }
  BT::NodeStatus on_completion(Service::Response::SharedPtr response) override {
    if (!response->success) {
      RCLCPP_ERROR(node_->get_logger(), "Ordered goals: %s", response->reason.c_str());
      return BT::NodeStatus::FAILURE;
    }
    setOutput("remaining_goals", response->remaining_goals);
    setOutput("first_index", response->first_index);
    return BT::NodeStatus::SUCCESS;
  }
};

class BindOrderedPath : public nav2_behavior_tree::BtServiceNode<Service> {
public:
  BindOrderedPath(const std::string & name, const BT::NodeConfiguration & config)
  : BtServiceNode(name, config) {}
  static BT::PortsList providedPorts() {
    return providedBasicPorts({BT::InputPort<nav_msgs::msg::Path>("path"),
      BT::InputPort<std::string>("task_id"), BT::InputPort<unsigned>("first_index"),
      BT::OutputPort<nav_msgs::msg::Path>("output_path")});
  }
  void on_tick() override {
    *request_ = Service::Request(); request_->operation = Service::Request::BIND;
    should_send_request_ = bool(getInput("path", request_->path)) &&
      bool(getInput("task_id", request_->task_id)) && bool(getInput("first_index", request_->first_index));
  }
  BT::NodeStatus on_completion(Service::Response::SharedPtr response) override {
    if (response->success) {setOutput("output_path", request_->path); return BT::NodeStatus::SUCCESS;}
    RCLCPP_ERROR(node_->get_logger(), "Ordered path: %s", response->reason.c_str());
    return BT::NodeStatus::FAILURE;
  }
};
}  // namespace sentry_nav

BT_REGISTER_NODES(factory) {
  factory.registerNodeType<sentry_nav::OrderedNavigation>("OrderedNavigation");
  factory.registerNodeType<sentry_nav::GetRemainingGoals>("GetRemainingGoals");
  factory.registerNodeType<sentry_nav::BindOrderedPath>("BindOrderedPath");
}
