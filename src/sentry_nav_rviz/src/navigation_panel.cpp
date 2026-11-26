#include "sentry_nav_rviz/navigation_panel.hpp"
#include <pluginlib/class_list_macros.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <cmath>

namespace sentry_nav_rviz {
NavigationPanel::NavigationPanel(QWidget * parent) : rviz_common::Panel(parent) {
  auto * layout = new QVBoxLayout(this);
  connection_ = new QLabel("正在连接导航服务…"); connection_->setObjectName("connection");
  mode_ = new QComboBox; mode_->setObjectName("mode");
  mode_->addItems({"单点导航（点击即发送）", "连续多点（采集后开始）", "逐点多点（每点停靠）"});
  hint_ = new QLabel("2D Goal Pose 设置位置。特殊段依次设置 begin 和 end；begin 朝向使用配置的地图轴线。");
  hint_->setWordWrap(true);
  list_ = new QListWidget; list_->setObjectName("goals"); list_->setMinimumHeight(100);
  auto * edit = new QHBoxLayout;
  remove_ = new QPushButton("删除选中"); remove_->setObjectName("remove");
  clear_ = new QPushButton("清空点位"); clear_->setObjectName("clear");
  edit->addWidget(remove_); edit->addWidget(clear_);
  special_ = new QPushButton("设置特殊段 begin/end"); special_->setObjectName("special");
  special_->setCheckable(true);
  spin_button_ = new QPushButton("开启电控自旋"); spin_button_->setObjectName("spin");
  auto * controls = new QHBoxLayout;
  start_ = new QPushButton("开始导航"); start_->setObjectName("start");
  cancel_button_ = new QPushButton("取消导航"); cancel_button_->setObjectName("cancel");
  controls->addWidget(start_); controls->addWidget(cancel_button_);
  status_ = new QLabel("就绪"); status_->setObjectName("status"); status_->setWordWrap(true);
  status_->setTextFormat(Qt::PlainText); status_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  layout->addWidget(connection_); layout->addWidget(mode_); layout->addWidget(hint_);
  layout->addWidget(special_); layout->addWidget(spin_button_);
  layout->addWidget(list_); layout->addLayout(edit); layout->addLayout(controls); layout->addWidget(status_);
  connect(start_, &QPushButton::clicked, this, &NavigationPanel::startTask);
  connect(cancel_button_, &QPushButton::clicked, this, &NavigationPanel::cancelTask);
  connect(remove_, &QPushButton::clicked, this, [this]() {
    const int row = list_->currentRow();
    if (row >= 0) {
      size_t first = static_cast<size_t>(row), last = first + 1;
      if (kinds_[first] == RoutePoint::SPECIAL_END && first > 0) {--first;}
      if (kinds_[first] == RoutePoint::SPECIAL_BEGIN && last < kinds_.size() && kinds_[last] == RoutePoint::SPECIAL_END) {++last;}
      goals_.erase(goals_.begin() + first, goals_.begin() + last);
      kinds_.erase(kinds_.begin() + first, kinds_.begin() + last); expect_end_ = false; special_->setChecked(false); refreshGoals();
    }
  });
  connect(clear_, &QPushButton::clicked, this, [this]() {goals_.clear(); kinds_.clear(); expect_end_ = false; special_->setChecked(false); refreshGoals();});
  connect(spin_button_, &QPushButton::clicked, this, [this]() {
    if (!spin_->service_is_ready()) {status_->setText("自旋服务未就绪。"); return;}
    auto request = std::make_shared<std_srvs::srv::SetBool::Request>(); request->data = !spin_enabled_;
    spin_button_->setEnabled(false);
    spin_->async_send_request(request, [this, request](rclcpp::Client<std_srvs::srv::SetBool>::SharedFuture future) {
      auto response = future.get();
      if (response->success) {spin_enabled_ = request->data;}
      status_->setText(QString::fromStdString(response->message)); updateControls();
    });
  });
  connect(mode_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
    goals_.clear(); kinds_.clear(); expect_end_ = false; special_->setChecked(false); refreshGoals();
    status_->setText("已切换目标输入模式；切换模式和清空点位不会取消正在执行的任务。");
  });
  // Do not inherit RViz's __node remap: the panel needs its own executor node.
  node_ = std::make_shared<rclcpp::Node>("sentry_navigation_panel", "/",
    rclcpp::NodeOptions().use_global_arguments(false));
  executor_.add_node(node_); connectRos();
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, [this]() {
    if (rclcpp::ok()) {executor_.spin_some(std::chrono::milliseconds(5)); updateControls();}
  });
  timer_->start(30);
}
NavigationPanel::~NavigationPanel() {
  timer_->stop(); executor_.remove_node(node_);
}
void NavigationPanel::connectRos() {
  ++request_version_; pending_ = cancel_pending_ = cancel_requested_ = false;
  single_ = rclcpp_action::create_client<Single>(node_, namespace_ + "/navigate_to_pose");
  multi_ = rclcpp_action::create_client<Multi>(node_, namespace_ + "/navigate_through_poses");
  route_ = rclcpp_action::create_client<Route>(node_, namespace_ + "/navigate_route");
  append_ = node_->create_client<sentry_nav_interfaces::srv::AppendRoutePoint>(namespace_ + "/append_route_points");
  spin_ = node_->create_client<std_srvs::srv::SetBool>(namespace_ + "/set_navigation_spin");
  cancel_ = node_->create_client<std_srvs::srv::Trigger>(namespace_ + "/cancel_navigation");
  goal_sub_ = node_->create_subscription<Pose>(namespace_ + "/navigation_panel/goal_pose", 10,
    [this](Pose::ConstSharedPtr pose) {receiveGoal(*pose);});
  status_sub_ = node_->create_subscription<std_msgs::msg::String>(namespace_ + "/navigation_status",
    rclcpp::QoS(1).transient_local(), [this](std_msgs::msg::String::ConstSharedPtr msg) {
      status_->setText(QString::fromStdString(msg->data));
    });
  spin_state_sub_ = node_->create_subscription<std_msgs::msg::Bool>(namespace_ + "/navigation_spin_requested",
    rclcpp::QoS(1).transient_local(), [this](std_msgs::msg::Bool::ConstSharedPtr msg) {
      spin_enabled_ = msg->data; updateControls();
    });
  markers_ = node_->create_publisher<visualization_msgs::msg::MarkerArray>(
    namespace_ + "/navigation_panel/waypoints", rclcpp::QoS(1).transient_local());
  updateControls();
}
void NavigationPanel::load(const rviz_common::Config & config) {
  rviz_common::Panel::load(config);
  QString value;
  if (config.mapGetString("Namespace", &value) && !value.trimmed().isEmpty()) {
    auto name = value.trimmed().toStdString();
    if (name.front() != '/') {name = '/' + name;}
    while (name.size() > 1 && name.back() == '/') {name.pop_back();}
    if (name == "/") {name.clear();}
    if (name != namespace_) {namespace_ = name; connectRos();}
  }
}
void NavigationPanel::save(rviz_common::Config config) const {
  rviz_common::Panel::save(config); config.mapSetValue("Namespace", QString::fromStdString(namespace_));
}
void NavigationPanel::receiveGoal(const Pose & pose) {
  const auto & p = pose.pose.position; const auto & q = pose.pose.orientation;
  const double norm = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
  if (pose.header.frame_id.empty() || !std::isfinite(p.x) || !std::isfinite(p.y) ||
    !std::isfinite(norm) || norm < 1e-12) {status_->setText("拒绝无效目标坐标或姿态。"); return;}
  if (pending_ || cancel_pending_) {status_->setText("请求处理中，请稍后再添加目标。"); return;}
  if (mode_->currentIndex() != 0 && special_->isChecked() && !expect_end_ && goals_.size() >= 127) {
    status_->setText("特殊段需要两个空余点位。"); return;
  }
  if (mode_->currentIndex() == 0) {goals_ = {pose}; kinds_ = {RoutePoint::NORMAL}; refreshGoals(); startTask();}
  else if (goals_.size() < 128) {
    uint8_t kind = RoutePoint::NORMAL;
    if (expect_end_) {kind = RoutePoint::SPECIAL_END; expect_end_ = false; special_->setChecked(false);}
    else if (special_->isChecked()) {kind = RoutePoint::SPECIAL_BEGIN; expect_end_ = true;}
    goals_.push_back(pose); kinds_.push_back(kind); refreshGoals();
    if (route_active_ && !expect_end_) {
      const size_t first = kind == RoutePoint::SPECIAL_END ? goals_.size() - 2 : goals_.size() - 1;
      if (!append_->service_is_ready()) {
        goals_.resize(first); kinds_.resize(first); refreshGoals(); status_->setText("追加服务未就绪，点位未加入路线。"); return;
      }
      auto request = std::make_shared<sentry_nav_interfaces::srv::AppendRoutePoint::Request>();
      for (size_t i = first; i < goals_.size(); ++i) {RoutePoint point; point.pose = goals_[i]; point.kind = kinds_[i]; request->points.push_back(point);}
      pending_ = true; updateControls();
      append_->async_send_request(request, [this, first](rclcpp::Client<sentry_nav_interfaces::srv::AppendRoutePoint>::SharedFuture future) {
        const auto response = future.get(); pending_ = false;
        if (!response->success) {goals_.resize(first); kinds_.resize(first);}
        status_->setText(QString::fromStdString(response->reason)); refreshGoals();
        if (cancel_requested_) {cancel_requested_ = false; cancelTask();}
      });
    } else {status_->setText(expect_end_ ? "请设置特殊段 end。" : "点位已采集。");}
  } else {status_->setText("最多支持 128 个目标点。");}
}
void NavigationPanel::refreshGoals() {
  list_->clear();
  for (size_t i = 0; i < goals_.size(); ++i) {
    const auto & p = goals_[i];
    const char * kind = kinds_[i] == RoutePoint::SPECIAL_BEGIN ? "BEGIN" : kinds_[i] == RoutePoint::SPECIAL_END ? "END" : "普通";
    list_->addItem(QString("%1  %2  (%3, %4)  %5").arg(i+1).arg(kind).arg(p.pose.position.x, 0, 'f', 2)
      .arg(p.pose.position.y, 0, 'f', 2).arg(QString::fromStdString(p.header.frame_id)));
  }
  publishMarkers(); updateControls();
}
void NavigationPanel::publishMarkers() {
  using Marker = visualization_msgs::msg::Marker;
  visualization_msgs::msg::MarkerArray out;
  Marker clear; clear.header.frame_id = "map"; clear.action = Marker::DELETEALL;
  out.markers.push_back(clear);
  for (size_t i = 0; i < goals_.size(); ++i) {
    Marker point; point.header = goals_[i].header; point.header.stamp = node_->now();
    point.ns = "queued_goals"; point.id = 3*i; point.type = Marker::SPHERE; point.action = Marker::ADD;
    point.pose = goals_[i].pose; point.pose.position.z += .08;
    point.scale.x = point.scale.y = point.scale.z = .15;
    point.color.r = 1.; point.color.g = kinds_[i] == RoutePoint::NORMAL ? .65 : .1;
    point.color.b = kinds_[i] == RoutePoint::SPECIAL_END ? 1. : 0.; point.color.a = 1.; out.markers.push_back(point);
    point.id++; point.type = Marker::TEXT_VIEW_FACING; point.pose.position.z += .2;
    point.scale.z = .22; point.text = std::to_string(i+1);
    if (kinds_[i] == RoutePoint::SPECIAL_BEGIN) {point.text += " BEGIN";}
    if (kinds_[i] == RoutePoint::SPECIAL_END) {point.text += " END";}
    out.markers.push_back(point);
  }
  markers_->publish(out);
}
template<class Action> void NavigationPanel::send(
  const typename rclcpp_action::Client<Action>::SharedPtr & client, const typename Action::Goal & goal) {
  const auto version = ++request_version_;
  pending_ = true; status_->setText("正在发送导航任务…"); updateControls();
  typename rclcpp_action::Client<Action>::SendGoalOptions options;
  options.goal_response_callback = [this, version](auto handle) {
    if (version != request_version_) {return;}
    pending_ = false;
    if (cancel_requested_) {cancel_requested_ = false; cancelTask(); return;}
    status_->setText(handle ? "导航任务已接受。可随时按“取消导航”。" : "导航任务被拒绝，请检查导航节点状态和目标。");
    updateControls();
  };
  options.result_callback = [this, version](const auto & result) {
    if (version != request_version_) {return;}
    if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {status_->setText("导航完成。");}
    // Detailed cancel/abort reason comes from navigation_status, not the empty Humble action result.
  };
  client->async_send_goal(goal, options);
}
void NavigationPanel::startTask() {
  if (goals_.empty() || pending_ || cancel_pending_ || expect_end_) {return;}
  if (mode_->currentIndex() == 0) {
    if (!single_->action_server_is_ready()) {status_->setText("单点导航服务未就绪。"); return;}
    Single::Goal goal; goal.pose = goals_.front(); send<Single>(single_, goal);
  } else {
    sendRoute();
  }
}
void NavigationPanel::sendRoute() {
  if (!route_->action_server_is_ready()) {status_->setText("路线任务服务未就绪。"); return;}
  Route::Goal goal; goal.continuous = mode_->currentIndex() == 1;
  for (size_t i = 0; i < goals_.size(); ++i) {RoutePoint p; p.pose = goals_[i]; p.kind = kinds_[i]; goal.points.push_back(p);}
  const auto version = ++request_version_; pending_ = true; updateControls();
  rclcpp_action::Client<Route>::SendGoalOptions options;
  options.goal_response_callback = [this, version](auto handle) {
    if (version != request_version_) {if (handle) {route_->async_cancel_goal(handle);} return;}
    pending_ = false; route_handle_ = handle; route_active_ = bool(handle);
    status_->setText(handle ? "路线已开始，可以追加点位。" : "路线任务被拒绝。");
    if (cancel_requested_) {cancel_requested_ = false; cancelTask();} updateControls();
  };
  options.feedback_callback = [this, version](auto, auto feedback) {
    if (version == request_version_) {status_->setText(QString("路线 %1/%2：%3").arg(feedback->current_index + 1).arg(feedback->total_points).arg(QString::fromStdString(feedback->phase)));}
  };
  options.result_callback = [this, version](const auto & result) {
    if (version != request_version_) {return;}
    route_active_ = false; route_handle_.reset();
    status_->setText(result.result ? QString::fromStdString(result.result->reason) : "路线结束"); updateControls();
  };
  route_->async_send_goal(goal, options);
}
void NavigationPanel::cancelTask() {
  if (pending_) {
    cancel_requested_ = true;
    status_->setText("已请求取消，目标确认后立即停止。"); updateControls(); return;
  }
  if (route_active_ && route_handle_) {
    route_->async_cancel_goal(route_handle_); status_->setText("正在取消路线任务…"); return;
  }
  if (!cancel_->service_is_ready()) {status_->setText("取消服务未就绪；请检查导航节点连接。"); return;}
  cancel_pending_ = true; ++request_version_; updateControls();
  status_->setText("正在取消导航…");
  cancel_->async_send_request(std::make_shared<std_srvs::srv::Trigger::Request>(),
    [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture response) {
      cancel_pending_ = false;
      status_->setText(response.get()->success ? "导航已取消。点位列表保留，可重新开始。" : "取消失败，请检查节点状态。");
      updateControls();
    });
}
void NavigationPanel::updateControls() {
  const bool connected = mode_->currentIndex() == 0 ? single_->action_server_is_ready() : route_->action_server_is_ready();
  connection_->setText(QString::fromStdString(namespace_) + (connected ? "  导航服务已连接" : "  等待导航服务"));
  start_->setEnabled(connected && !goals_.empty() && !pending_ && !cancel_pending_ && !expect_end_ && !route_active_);
  cancel_button_->setEnabled(cancel_->service_is_ready() && !cancel_pending_ && !cancel_requested_);
  mode_->setEnabled(!pending_ && !cancel_pending_ && !route_active_);
  clear_->setEnabled(!pending_ && !cancel_pending_ && !route_active_);
  remove_->setEnabled(!pending_ && !cancel_pending_ && !route_active_);
  special_->setEnabled(mode_->currentIndex() != 0 && !pending_ && !cancel_pending_);
  spin_button_->setText(spin_enabled_ ? "关闭电控自旋" : "开启电控自旋");
  spin_button_->setEnabled(spin_->service_is_ready());
}
}  // namespace sentry_nav_rviz
PLUGINLIB_EXPORT_CLASS(sentry_nav_rviz::NavigationPanel, rviz_common::Panel)
