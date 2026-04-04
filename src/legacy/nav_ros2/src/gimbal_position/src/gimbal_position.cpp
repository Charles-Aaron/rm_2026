#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <functional>
#include <chrono>
#include "std_msgs/msg/float32.hpp"
#include <cmath>


#include <geometry_msgs/msg/pose.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp> 
#include <std_msgs/msg/float64.hpp>
#include <tf2/utils.h>
#include <cmath>
#include <limits>
#include <optional>
#include <tf2/LinearMath/Quaternion.h>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float64.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
using namespace std::chrono_literals;
class JointStatePublisher : public rclcpp::Node
{
public:
    JointStatePublisher()
        : Node("jointstate_publisher")
    {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("serial/gimbal_joint_state", 10);
        velocity_publisher_= this->create_publisher<geometry_msgs::msg::TwistStamped>("chassis_yaw", 10);
        timer_ = this->create_wall_timer(20ms, std::bind(&JointStatePublisher::timer_callback, this));
         tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(this);
        joint_state_sub_= this->create_subscription<std_msgs::msg::Float32>(
            "/gimbal_controller/gimbal_relative_angle", 10,
            [this](const std_msgs::msg::Float32::SharedPtr msg) {
                //RCLCPP_INFO(this->get_logger(), "接收到角度值: %.2f", msg->data);
                float normalized = fmodf(msg->data, static_cast<float>(2.0 * M_PI));
                // 如果余数是负数，加上 2π 使其落在 0 到 2π 的范围内
                if (normalized < 0.0f) {
                    normalized += static_cast<float>(2.0 * M_PI);
                }
                // RCLCPP_INFO(this->get_logger(), "规范后的角度值: %.2f", normalized );
                joint_state_msg.header.stamp = this->now();
                joint_state_msg.name = {
                    "gimbal_pitch_joint",
                    "gimbal_yaw_joint",
                    "gimbal_pitch_odom_joint",
                    "gimbal_yaw_odom_joint",
                  };
                  joint_state_msg.position = {
                    0.0,
                    normalized,
                    0.0,
                    0.0,
                  };
                joint_state_pub_->publish(joint_state_msg);
            });
    }


private:
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr velocity_publisher_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr joint_state_sub_;
    sensor_msgs::msg::JointState joint_state_msg;
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    //tf2_ros::TransformBroadcaster tf_broadcaster_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_; // 修改为unique_ptr
    
    float angular_velocity=0.0;
    float liney_velocity=0.0;
    float propo=0.0;
    char i=0;
    char j=0;
    float rela=0.0;
    float linex_velocity=0.0;
    float compar=0;
    float x_0=0.0;
    double y=0.0;
    char arrived=0;
    char m=0;
    float y_0=0.0;
    float deviat=0.0;
    float t=0;
    void timer_callback() {

        geometry_msgs::msg::TransformStamped transform_stamped;
  // `+ 0.1` means transform into future. according to https://robotics.stackexchange.com/a/96615
//   transform_stamped.header.stamp = this->now()+std::chrono::milliseconds(500);
//   transform_stamped.header.frame_id = "map";
//   transform_stamped.child_frame_id =  "odom";

// //   const Eigen::Vector3d translation = result_t_.translation();
// //   const Eigen::Quaterniond rotation(result_t_.rotation());

// //   transform_stamped.transform.translation.x = translation.x();
// //   transform_stamped.transform.translation.y = translation.y();
// //   transform_stamped.transform.translation.z = translation.z();
// //   transform_stamped.transform.rotation.x = rotation.x();
// //   transform_stamped.transform.rotation.y = rotation.y();
// //   transform_stamped.transform.rotation.z = rotation.z();
// //   transform_stamped.transform.rotation.w = rotation.w();
//   transform_stamped.transform.translation.x = 0.0;
//   transform_stamped.transform.translation.y = 0;
//   transform_stamped.transform.translation.z = 0;
//   transform_stamped.transform.rotation.x = 0;
//   transform_stamped.transform.rotation.y = 0;
//   transform_stamped.transform.rotation.z = 0;
//   transform_stamped.transform.rotation.w = 1;
// tf_broadcaster_->sendTransform(transform_stamped);
//   tf_broadcaster_->sendTransform(transform_stamped);
//             //获取小车相对于地图坐标系的位姿
//             geometry_msgs::msg::TransformStamped transformStamped;
//             try {
//                 transformStamped = tf_buffer_->lookupTransform("map", "odom", rclcpp::Time(0), rclcpp::Duration(1s));
//             } catch (tf2::TransformException& ex) {
//                 RCLCPP_ERROR(this->get_logger(), "TF error: %s", ex.what());
//                 return;
//             }
//             double goal_x = 1.0;
//             double goal_y = 0.0;
//             double goal_x_1 = 4.0;
//             double goal_y_1 = 15.0;
//             double distance = std::sqrt(
//                 std::pow(transformStamped.transform.translation.x - goal_x, 2) + std::pow(transformStamped.transform.translation.y - goal_y, 2)
//             );
//             double distance1 = std::sqrt(
//                 std::pow(transformStamped.transform.translation.x- goal_x_1, 2) + std::pow(transformStamped.transform.translation.y - goal_y_1, 2)
//             );
//             tf2::Quaternion q(
//                 transformStamped.transform.rotation.x,
//                 transformStamped.transform.rotation.y,
//                 transformStamped.transform.rotation.z,
//                 transformStamped.transform.rotation.w
//             );
//             double yaw = tf2::getYaw(q);
//             RCLCPP_INFO(this->get_logger(), "Published car yaw: %.2f radians", yaw);
//             RCLCPP_INFO(this->get_logger(), "Published car y: %.2f y", transformStamped.transform.translation.y);
//             RCLCPP_INFO(this->get_logger(), "Published car x: %.2f x", transformStamped.transform.translation.x);
            
            // if ((distance <1.0 && distance>0.9)||(distance1 <1.0 && distance1>0.9))
            // { x_0 =transformStamped.transform.translation.x;arrived=0;i=0;m=0;t=0;}
                
            // if (distance < 0.8||distance1 < 0.8)
            // {
            //     if(distance>distance1){y=4;}
            //     linex_velocity=0.0;
            //     liney_velocity=0.0;
            //     float x_1 =transformStamped.transform.translation.x;
            //     compar= x_1-x_0;
            //     if(std::abs(yaw)<=0.06){
            //         angular_velocity=0;
            //         i=0;
            //         m=1;

            //     }
            //     else{
            //         i++;
            //     switch (i) {
            //         case 1:
            //         case 2:
            //         case 3:
            //         case 4:
            //         case 5:
            //             angular_velocity=0;
            //             break;
            //         case 6:
            //             angular_velocity=0;
            //             j=compar/std::abs(compar);
            //             rela=std::abs(yaw)-(1.57-1.57*j+deviat);
            //             propo=-rela*abs(yaw)/yaw/3.14;
            //             break;
            //         case 38:
            //            i=0;
            //             break;
            //         default:
            //             angular_velocity=j*(16-std::abs((i-21)))*propo*10/16;
            //             break;}
            //     }
            //     if(m){liney_velocity=j*-0.3*transformStamped.transform.translation.y/std::abs(transformStamped.transform.translation.y);}
            //     if(std::abs(transformStamped.transform.translation.y)<=0.03&&std::abs(yaw)<=0.06){arrived=1;}
            //     if(arrived){t++;if(t>=25){linex_velocity=1.2;}else{liney_velocity=0.0; angular_velocity=0.0;}}
            //     auto msg_ = geometry_msgs::msg::TwistStamped();
            //     msg_.header.stamp = this->get_clock()->now();
            //     msg_.header.frame_id = "base_link";
            //      // 设置线速度为0，只发布角速度
            //      msg_.twist.linear.x =  linex_velocity;
            //      msg_.twist.linear.y = liney_velocity;
            //      msg_.twist.linear.z = 0.0;
    
            //      msg_.twist.angular.x = 0.0;
            //      msg_.twist.angular.y = 0.0;
            //      msg_.twist.angular.z = angular_velocity;
            //      velocity_publisher_->publish(msg_);
            //      RCLCPP_INFO(this->get_logger(), "Published car yaw: %.2f radians", yaw);
            //       RCLCPP_INFO(this->get_logger(), "Published car y: %.2f y", transformStamped.transform.translation.y);
            // }
        }
            };

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<JointStatePublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}