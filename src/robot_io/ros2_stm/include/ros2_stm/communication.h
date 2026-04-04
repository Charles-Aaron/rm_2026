#include <rclcpp/rclcpp.hpp>  // ROS 2中的头文件，用于定义节点、发布器和订阅器等
#include <serial/serial.h> 

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <chrono>
#include <iomanip>
#include <std_msgs/msg/string.hpp>  // ROS 2中的消息类型需要使用msg命名空间
#include <algorithm>
#include <tf2/LinearMath/Quaternion.h> 

#include <rclcpp/parameter_service.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <stdlib.h> // 用于定义杂项函数和宏
#include <stdio.h> // 用于定义输入输出函数
//#include <tf2/tf2.h>
#include <tf2_ros/transform_broadcaster.h>  // ROS 2中TF的头文件
#include <tf2_ros/transform_listener.h>      // ROS 2中TF的头文件
#include <mutex>
#include <thread>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>

#include "customize_messages/msg/remotedata.hpp"  

class Communication : public rclcpp::Node {
public:
  // 构造函数
  Communication();
    
  // 析构函数
  ~Communication();

  // 打开串口
  void OpenUSB();
    // ROS 2串口通信逻辑
  

  // 检查串口
  void CheckUSB() ;
    // ROS 2串口通信逻辑


  // 发送数据
  void SendSTM32();
    // ROS 2串口通信逻辑


  // 接收数据
  void ReceiveSTM32(int16_t* startFlag, int16_t* towerFlag,
                    int16_t* bloodreturn, int16_t* supplyflag, int16_t* engineeringflag,
                    int16_t* start35flag, int16_t* outpostsmash, int16_t* operatorcontrol,
                    float* mapx, float* mapy, int16_t* enemyflag, float* enemydistance, float* enemyangle);
    // ROS 2串口通信逻辑
  

  // 发布自定义遥控器话题
  void PubRemote(customize_messages::msg::Remotedata remote_pp);

  void pubinitialpose(geometry_msgs::msg::PoseWithCovarianceStamped initialpose_data);

  // 运行
  void Run();
    // 运行逻辑
  
private:
  // ROS 2的节点句柄已经包含在基类rclcpp::Node中
  // ros::NodeHandle nh_;
  // ROS 2的订阅者和发布者已经替换为rclcpp::Node的成员函数
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr command_sub_;
  // rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
  // rclcpp::Subscription<customize_messages::msg::Remotedata>::SharedPtr remote_sub_;
  rclcpp::Publisher<customize_messages::msg::Remotedata>::SharedPtr remote_pub_;
  rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr initialpose_pub_;

  // 回调函数
  void cmd_vel_chassis_callback(const geometry_msgs::msg::Twist::SharedPtr msg) ;
    // 处理速度指令
  
  // void imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg) ;
  //   // 处理IMU数据
  
  // void remote_data_callback(const customize_messages::msg::Remotedata::SharedPtr msg);
  //   // 处理远程数据
  
};

std::string usb_num_;
serial::Serial ros_ser;
// 实例化stm_ros::remote_data消息类型
 customize_messages::msg::Remotedata stm32_data;
 geometry_msgs::msg::PoseWithCovarianceStamped  initialpose_data;


  // 串口发送数组
 std::array<unsigned char,14> send_buffer_;

  int home_success_flag_get_;

  // 串口接收数组
  std::array<unsigned char,150> receive_buffer_;

  // 串口接收遥控器标志位
  int16_t Start_flag_;
  int16_t Tower_flag_;
  int16_t Blood_return_;
  int16_t Supply_flag_;
  int16_t Engineering_flag_;
  int16_t Start_35_flag_;
  int16_t Outpost_smash_;
  int16_t Operator_control_;
  float Map_x_;
  float Map_y_;
  int16_t Enemy_flag_;
  float Enemy_distance_;
  float Enemy_angle_;

  // 串口接收头尾帧
  const std::array<unsigned char, 2> header_ = {0x10, 0x11};

  // 使用结构体替代联合体，因为联合体在C++中使用较少，且可能导致未定义行为
  typedef union{
    float data;
    std::array<unsigned char, 4> data8;
  } DataU ;

  DataU odomdata_linear_x_result_;    // 发送x方向速度
  DataU odomdata_linear_y_result_;    // 发送y方向速度
  DataU odomdata_angular_z_result_;   // 发送z方向速度

  struct ImuSub {
    float imu_yaw;
    float imu_yaw_z;
    float imu_yaw_x;
    float imu_yaw_y;
  };

  ImuSub imu_sub_data_;

  // 打印行为树标志位
  bool found_legal_flag_ = false;

  bool change_point_flag_ = false;
  int current_target_point_;
  float distance_;

  bool change_point_flag_home_ = false;
  int current_target_point_home_;
  float distance_home_;

