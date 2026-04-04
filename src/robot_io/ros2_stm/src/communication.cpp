#include "ros2_stm/communication.h"
//构函数
Communication::Communication() : Node("communication") {
    // 初始化ROS节点句柄
    //nh_ = this;
    this->declare_parameter<std::string>("USB_num", "/dev/ttyUSB0");
     auto result = this->get_parameter("USB_num");

    if (result.get_type() == rclcpp::ParameterType::PARAMETER_STRING)
    {
        usb_num_ = result.as_string();
    }
    else
    {
    // 如果参数获取失败或者类型不对，设置默认值
        usb_num_ = "/dev/ttyUSBnum";
        RCLCPP_WARN(this->get_logger(), "Failed to get 'USB_num' parameter with correct type. Using default value: %s", usb_num_.c_str());
    }

       command_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "red_standard_robot1/cmd_vel", 10, std::bind(&Communication::cmd_vel_chassis_callback, this, std::placeholders::_1));

    // 创建订阅者：用来订阅"/imu" 话题
    //   imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
    //   "imu", 10, std::bind(&Communication::imu_callback, this, std::placeholders::_1));


    
    // initialpose_pub_ = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("initialpose_pub", 10);
    }

//析构函数
Communication::~Communication()
{}

void Communication::cmd_vel_chassis_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    // // 测试成功
    // ROS_INFO_STREAM("Success cmd_vel_callback() this part!!!");

    // 调用了 memset() 函数，将 send_buffer 数组中的所有元素都初始化为 0
    // 这个数组是用来存储待发送的数据的，因此需要进行初始化，以确保不能包含已使用的旧数据
   std::fill(send_buffer_.begin(), send_buffer_.end(), 0);

    // 从 msg 变量中读取到了机器人的线速度和角速度，并将其存储在三个定义好的联合体
    // odomdata_linear_x_result、odomdata_linear_y_result 和 odomdata_angular_z_result 中
    odomdata_linear_x_result_.data  = msg->linear.x;
    odomdata_linear_y_result_.data  = msg->linear.y;
    odomdata_angular_z_result_.data = msg->angular.z;

    // 在参数服务器中获取安全到家标志位
     this->get_parameter("home_success_flag", home_success_flag_get_);

    // 在串口发送之前我们需要将这些变量转换为字节数组的形式，方便通过串口进行传输
    send_buffer_[0]  = 0xFF;      //数据的帧头
    send_buffer_[1]  = odomdata_linear_x_result_.data8[0];
    send_buffer_[2]  = odomdata_linear_x_result_.data8[1];
    send_buffer_[3]  = odomdata_linear_x_result_.data8[2];
    send_buffer_[4]  = odomdata_linear_x_result_.data8[3];
    send_buffer_[5]  = odomdata_linear_y_result_.data8[0];
    send_buffer_[6]  = odomdata_linear_y_result_.data8[1];
    send_buffer_[7]  = odomdata_linear_y_result_.data8[2];
    send_buffer_[8]  = odomdata_linear_y_result_.data8[3];
    send_buffer_[9]  = odomdata_angular_z_result_.data8[0];
    send_buffer_[10] = odomdata_angular_z_result_.data8[1];
    send_buffer_[11] = odomdata_angular_z_result_.data8[2];
    send_buffer_[12] = odomdata_angular_z_result_.data8[3];
    //send_buffer_[13] = home_success_flag_get_;
    send_buffer_[13] = 0xFE;      //数据的帧头
}

// void Communication::imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg)
// {
//     // 在这里处理IMU数据
//     imu_sub_data_.imu_yaw = msg->orientation.w;
//     imu_sub_data_.imu_yaw_x = msg->orientation.x;
//     imu_sub_data_.imu_yaw_y = msg->orientation.y;
//     imu_sub_data_.imu_yaw_z = msg->orientation.z;
// }

// void Communication::remote_data_callback(const customize_messages::msg::Remotedata::SharedPtr msg)
// {
//     // // 测试成功
//     // ROS_INFO_STREAM("Success remote_data_callback() this part!!!");
    
//     int16_t start_flag      = msg->start_flag;
//     int16_t tower_flag      = msg->tower_flag;
//     int16_t blood_return       = msg->blood_return;
//     int16_t supply       = msg->supply;
//     int16_t engineering       = msg->engineering;
//     int16_t start_35_flag       = msg->start_35_flag;
//     int16_t outpost_smash       = msg->outpost_smash;
//     int16_t operator_control       = msg->operator_control;
//     float_t map_x         = msg->map_x;
//     float_t map_y         = msg->map_y;

//     // 处理接收到的消息
//     // ROS_INFO("start_flag = %d",     msg->start_flag);
//     // ROS_INFO("tower_flag = %d",     msg->tower_flag);
//     // ROS_INFO("boss_flag  = %d",     msg->boss_flag);
//     // ROS_INFO("enemy_flag = %d",     msg->enemy_flag);
//     // ROS_INFO("enemy_distance = %d", msg->enemy_distance);
//     // ROS_INFO("enemy_angle = %d",    msg->enemy_angle);
// }

void Communication::CheckUSB()
{
    // // 测试成功
    // ROS_INFO_STREAM("Success CheckUSB() this part!!!");
}

void Communication::OpenUSB()
{
    // // 测试成功
    // ROS_INFO_STREAM("Success OpenUSB() this part!!!");

    try
    {
        // 设置串口基本配置
        ros_ser.setPort(usb_num_);                                    // 将 ROS 节点与 /dev/ttyUSB0 端口连接起来，以实现通过串口进行数据传输的功能
        ros_ser.setBaudrate(115200);                                 // 设置串口的波特率
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);   // 定义了一个名为 to 的超时时间对象，使用了 simpleTimeout 静态函数来创建一个简单的超时时间对象
        ros_ser.setTimeout(to);                                      // 设置串口通信的超时时间为 1 秒钟，入口参数是 serial::Timeout 类型
        ros_ser.open();                                              // 打开串口
    }
    catch(serial::IOException &e)
    {
        // 关闭串口
        ros_ser.close();
        // 输出日志
         RCLCPP_INFO(this->get_logger(), "error");
        RCLCPP_INFO(this->get_logger(), "start_flag               = %s", usb_num_.c_str());
    }
}

void Communication::SendSTM32()
{
    // 如果串口成功打开则发送数据
    if(ros_ser.isOpen())
    {
        // 输出日志
        RCLCPP_INFO(this->get_logger(), "串口成功打开,发送数据");
        // // 在参数服务器中获取安全到家标志位
        // ros::param::get("home_success_flag",home_success_flag_get);
        // // 输出调试信息
        // ROS_INFO("home_success_flag:%d",home_success_flag_get);

        // send_buffer[13] = home_success_flag_get;

        // 通过 ros_ser.write(send_buffer,15) 将其写入串口中进行发送
        ros_ser.write(send_buffer_.data(),send_buffer_.size());

        // 输出调试信息
        RCLCPP_INFO(
        this->get_logger(),
        "speed_x: %f, speed_y: %f, angular_z: %f, home_success_flag: %d",
        odomdata_linear_x_result_.data,
        odomdata_linear_y_result_.data,
        odomdata_angular_z_result_.data,
        home_success_flag_get_
    );
}
}

void Communication::ReceiveSTM32(int16_t* startFlag, int16_t* towerFlag,
                                 int16_t* bloodreturn, int16_t* supplyflag, int16_t* engineeringflag,
                                 int16_t* start35flag, int16_t* outpostsmash, int16_t* operatorcontrol,
                                 float_t* mapx, float_t* mapy,int16_t*enemyflag,float_t*enemydistance,float_t*enemyangle)
{
    int16_t *stm_32 = new int16_t [13];
    float_t map_data[2];
    uint8_t data[200] = { 0 };                          // 定义接收数组
    float_t enemy_data[2];
    size_t Receive_N = ros_ser.available();             // 检测串口是否有可以读取的数据的函数，该函数返回可读取的字节数
   
    if (Receive_N != 0)
    {
        // 输出日志
        RCLCPP_INFO(this->get_logger(), "串口有可读数据");

        // 从串口接收缓冲区中读取 Receive_N 个字节的数据，并将数据存储到由 data 指向的缓冲区内
        // 最后，实际读取到的字节数量将被保存在变量 size 中
        int size = ros_ser.read(data, Receive_N);
        RCLCPP_INFO(this->get_logger(), "size: %d", size);
        // 检查信息头
        if (data[0] != 0x20 && data[18] != 0x21) {
        RCLCPP_INFO(this->get_logger(), "Received message header_tail error!");
      } else {
        RCLCPP_INFO(this->get_logger(), "Received message header_tail right!");
      }

        // ros::param::get("yaw",enemy_data[2]);
        // 读取数据
      stm_32[0] = data[1];
      stm_32[1] = data[2];
      stm_32[2] = data[3];
      stm_32[3] = data[4];
      stm_32[4] = data[5];
      stm_32[5] = data[6];
      stm_32[6] = data[7];
      stm_32[7] = data[8];
      stm_32[8] = (data[10] << 8) | data[9];
      stm_32[9] = (data[12] << 8) | data[11];
      stm_32[10] = data[13];
      stm_32[11] = (data[15] << 8) | data[14];
      stm_32[12] = (data[17] << 8) | data[16];

      map_data[0] = static_cast<float>(stm_32[8]) / 100.0;
      map_data[1] = static_cast<float>(stm_32[9]) / 100.0;
      enemy_data[0] = static_cast<float>(stm_32[11]) / 1000.0;
      enemy_data[1] = static_cast<float>(stm_32[12]) / 10000.0;

        // 打印结果
         // 打印结果
      RCLCPP_INFO(this->get_logger(), "start_flag               = %d", stm_32[0]);
      RCLCPP_INFO(this->get_logger(), "tower_flag               = %d", stm_32[1]);
      RCLCPP_INFO(this->get_logger(), "blood_return             = %d", stm_32[2]);
      RCLCPP_INFO(this->get_logger(), "supply_flag              = %d", stm_32[3]);
      RCLCPP_INFO(this->get_logger(), "engineering_flag         = %d", stm_32[4]);
      RCLCPP_INFO(this->get_logger(), "start_35_flag            = %d", stm_32[5]);
      RCLCPP_INFO(this->get_logger(), "outpost_smash            = %d", stm_32[6]);
      RCLCPP_INFO(this->get_logger(), "operator_control         = %d", stm_32[7]);
      RCLCPP_INFO(this->get_logger(), "map_x                    = %f", map_data[0]);
      RCLCPP_INFO(this->get_logger(), "map_y                    = %f", map_data[1]);
      RCLCPP_INFO(this->get_logger(), "enemy_flag               = %d", stm_32[10]);
      RCLCPP_INFO(this->get_logger(), "enemy_distance           = %f", enemy_data[0]);
      RCLCPP_INFO(this->get_logger(), "enemy_angle              = %f", enemy_data[1]);
        // 赋值到形参
        *startFlag      = stm_32[0];
        *towerFlag      = stm_32[1];
        *bloodreturn    = stm_32[2];
        *supplyflag     = stm_32[3];
        *engineeringflag= stm_32[4];
        *start35flag    = stm_32[5];
        *outpostsmash   = stm_32[6];
        *operatorcontrol= stm_32[7];
        *mapx           = map_data[0];
        *mapy           = map_data[1];
        *enemyflag      = stm_32[10];
        *enemydistance  = enemy_data[0];
        *enemyangle     = enemy_data[1];
    }
    else
    {
        // 输出日志
        RCLCPP_INFO(this->get_logger(), "串口无可读数据");
    }
    delete []stm_32;
}

// void Communication::PubRemote(customize_messages::msg::Remotedata stm_data)
// {
//     // // 测试成功
    

//     // 赋值消息内容
//     stm_data.start_flag      = Start_flag_;
//     stm_data.tower_flag      = Tower_flag_;
//     stm_data.blood_return    = Blood_return_;
//     stm_data.supply          = Supply_flag_;
//     stm_data.engineering     = Engineering_flag_;
//     stm_data.start_35_flag   = Start_35_flag_;
//     stm_data.outpost_smash   = Outpost_smash_;
//     stm_data.operator_control= Operator_control_;
//     stm_data.map_x           = Map_x_;
//     stm_data.map_y           = Map_y_;
//     stm_data.enemy_flag      = Enemy_flag_;
//     stm_data.enemy_distance  = Enemy_distance_;
//     stm_data.enemy_angle     = Enemy_angle_;
//     // ROS_INFO("stm_data.start_flag:%d",stm_data.start_flag);
//     // 发布话题
//     remote_pub_->publish(stm_data);
// }

void Communication::Run()
{
    // 检查串口(待补充)
    CheckUSB();

    // 发送数据
    SendSTM32();
        
    // 接收数据
    // ReceiveSTM32(&Start_flag_,&Tower_flag_,
    //              &Blood_return_,&Supply_flag_,&Engineering_flag_,
    //              &Start_35_flag_,&Outpost_smash_,&Operator_control_,&Map_x_,&Map_y_,&Enemy_flag_,&Enemy_distance_,&Enemy_angle_);

    // 将接收到的数据发布成自定义遥控器话题
    // PubRemote(stm32_data);
}


int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  setlocale(LC_ALL,"");
  auto communication = std::make_shared<Communication>();
  // 创建一个executor
  rclcpp::executors::SingleThreadedExecutor exec;

  // 将communication节点添加到executor中
  exec.add_node(communication);
  communication->OpenUSB();

  const double frequency = 50.0;  // 50Hz
  auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / frequency));

  // 使用executor的spin函数来处理回调
  while (rclcpp::ok()) {
    // 执行回调函数
    exec.spin_once();

    // 运行节点中的主函数
    communication->Run();

    // 等待下一个周期
    std::this_thread::sleep_for(period);
  }
  ros_ser.close();
  rclcpp::shutdown();
  return 0;
}

