#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <customize_messages/msg/remotedata.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

using NavigateToPose = nav2_msgs::action::NavigateToPose;
using GoalHandle     = rclcpp_action::ClientGoalHandle<NavigateToPose>;

class HomeNavigator : public rclcpp::Node
{
public:
  HomeNavigator()
  : Node("home_navigator")
  {
    // 订阅遥控消息
    sub_ = create_subscription<customize_messages::msg::Remotedata>(
      "/remote_data", 10,
      [this](const customize_messages::msg::Remotedata::SharedPtr msg) {
        if (!msg->start_flag) return;

        // 计算优先级：home > tower > protect
        int new_req = 0;
        if (msg->home_flag)    new_req = 3;
        else if (msg->tower_flag) new_req = 2;
        else if (msg->protect_flag) new_req = 1;

        if (new_req != current_req_) {
          current_req_ = new_req;
          sendNewGoal();
        }
      });

    // 创建导航客户端
    client_ = rclcpp_action::create_client<NavigateToPose>(this, "/red_standard_robot1/navigate_to_pose");

    // 三个目标点（map 坐标）
    goals_.resize(4);
    // protect
    goals_[1].pose.position.x = -0.5;
    goals_[1].pose.position.y = -4.0;
    // tower
    goals_[2].pose.position.x =  1.0;
    goals_[2].pose.position.y =  2.0;
    // home
    goals_[3].pose.position.x = -0.624;
    goals_[3].pose.position.y = -5.26;

    for (auto &g : goals_) {
      g.header.frame_id = "map";
      g.pose.orientation.w = 1.0;
    }
  }

private:
  void sendNewGoal()
  {
    if (!client_->wait_for_action_server(std::chrono::seconds(1))) {
      RCLCPP_WARN(get_logger(), "navigate_to_pose server not ready");
      return;
    }

    // 取消旧任务
    if (goal_handle_) {
      auto cancel_future = client_->async_cancel_goal(goal_handle_);
      rclcpp::spin_until_future_complete(shared_from_this(), cancel_future);
    }

    if (current_req_ == 0) return;  // 无目标

    NavigateToPose::Goal goal;
    goal.pose = goals_[current_req_];
    goal.pose.header.stamp = now();

    auto send_opts = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();
    send_opts.result_callback =
      [this](const GoalHandle::WrappedResult & result) {
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED)
          RCLCPP_INFO(get_logger(), "Goal reached");
        else
          RCLCPP_INFO(get_logger(), "Goal failed/aborted");
      };

    client_->async_send_goal(goal, send_opts);
  }

  rclcpp::Subscription<customize_messages::msg::Remotedata>::SharedPtr sub_;
  rclcpp_action::Client<NavigateToPose>::SharedPtr client_;
  std::vector<geometry_msgs::msg::PoseStamped> goals_;
  int current_req_ = 0;               // 0:none 1:protect 2:tower 3:home
  GoalHandle::SharedPtr goal_handle_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<HomeNavigator>());
  rclcpp::shutdown();
  return 0;
}