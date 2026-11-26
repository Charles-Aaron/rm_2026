#pragma once
#include <rviz_common/panel.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>
#include <sentry_nav_interfaces/action/navigate_route.hpp>
#include <sentry_nav_interfaces/srv/append_route_point.hpp>
#include <sentry_nav_interfaces/msg/route_point.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>

namespace sentry_nav_rviz {
class NavigationPanel : public rviz_common::Panel {
  Q_OBJECT
public:
  explicit NavigationPanel(QWidget * parent = nullptr);
  ~NavigationPanel() override;
  void load(const rviz_common::Config & config) override;
  void save(rviz_common::Config config) const override;
private:
  using Single = nav2_msgs::action::NavigateToPose;
  using Multi = nav2_msgs::action::NavigateThroughPoses;
  using Route = sentry_nav_interfaces::action::NavigateRoute;
  using RoutePoint = sentry_nav_interfaces::msg::RoutePoint;
  using Pose = geometry_msgs::msg::PoseStamped;
  std::string namespace_ = "/x_sentry";
  rclcpp::Node::SharedPtr node_;
  rclcpp::executors::SingleThreadedExecutor executor_;
  rclcpp_action::Client<Single>::SharedPtr single_;
  rclcpp_action::Client<Multi>::SharedPtr multi_;
  rclcpp_action::Client<Route>::SharedPtr route_;
  rclcpp_action::ClientGoalHandle<Route>::SharedPtr route_handle_;
  rclcpp::Client<sentry_nav_interfaces::srv::AppendRoutePoint>::SharedPtr append_;
  rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr spin_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr cancel_;
  rclcpp::Subscription<Pose>::SharedPtr goal_sub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr status_sub_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr spin_state_sub_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr markers_;
  QComboBox * mode_;
  QListWidget * list_;
  QLabel * connection_, * status_, * hint_;
  QPushButton * start_, * cancel_button_, * remove_, * clear_;
  QPushButton * special_, * spin_button_;
  QTimer * timer_;
  std::vector<Pose> goals_;
  std::vector<uint8_t> kinds_;
  bool expect_end_ = false, route_active_ = false, spin_enabled_ = false;
  void sendRoute();
  uint64_t request_version_ = 0;
  bool pending_ = false, cancel_pending_ = false, cancel_requested_ = false;
  void connectRos();
  void receiveGoal(const Pose & pose);
  void refreshGoals();
  void publishMarkers();
  void startTask();
  void cancelTask();
  void updateControls();
  template<class Action> void send(
    const typename rclcpp_action::Client<Action>::SharedPtr & client, const typename Action::Goal & goal);
};
}  // namespace sentry_nav_rviz
