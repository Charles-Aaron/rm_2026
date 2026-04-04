#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "pcl_conversions/pcl_conversions.h"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include "pcl/filters/radius_outlier_removal.h"
#include <pcl/PCLPointCloud2.h>
#include <pcl/filters/crop_box.h>
#include <pcl/filters/crop_hull.h>
#include <pcl/surface/convex_hull.h>
#include <vector>
#include <pcl/ModelCoefficients.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/io/pcd_io.h>
#include <cmath> // For M_PI and trigonometric functions

// //过滤杂波算法
// class RadiusOutlierRemovalNode : public rclcpp::Node
// {
// public:
//   RadiusOutlierRemovalNode() : Node("pointcloud2_deal")
//   {
    
//          rclcpp::QoS qos(rclcpp::KeepLast(100));
//         qos.reliability(rclcpp::ReliabilityPolicy::BestEffort);
//         qos.durability(rclcpp::DurabilityPolicy::Volatile);
//     sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
//       "/segmentation/obstacle",qos,std::bind(&RadiusOutlierRemovalNode::cloudCallback, this, std::placeholders::_1));
//     pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("filtered_point_cloud", 100);
//   }

// private:
//   void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr cloud_msg) const
//   {
//     pcl::PCLPointCloud2 pcl_cloud;
//     pcl_conversions::toPCL(*cloud_msg, pcl_cloud);

//     pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2);
//     pcl::PCLPointCloud2::Ptr cloud_filtered(new pcl::PCLPointCloud2);

//     *cloud = pcl_cloud;

//     pcl::RadiusOutlierRemoval<pcl::PCLPointCloud2> outrem;
//     outrem.setInputCloud(cloud);
//     outrem.setRadiusSearch(0.1);  // 设置搜索半径
//     outrem.setMinNeighborsInRadius(5);  // 设置最小邻居数
//     outrem.filter(*cloud_filtered);

//     sensor_msgs::msg::PointCloud2 output;
//     pcl_conversions::fromPCL(*cloud_filtered, output);
//     output.header = cloud_msg->header;

//     pub_->publish(output);
//   }

//   rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
//   rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
//  };



class PointCloudFilter : public rclcpp::Node
{
public:
    PointCloudFilter() : Node("point2cloud_deal")
    {
        // 创建订阅者，订阅名为cloud_in的点云话题
        rclcpp::QoS qos(rclcpp::KeepLast(1000));
        qos.reliability(rclcpp::ReliabilityPolicy::BestEffort);
        qos.durability(rclcpp::DurabilityPolicy::Volatile);
        cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "/livox/lidar/pointcloud", qos, std::bind(&PointCloudFilter::cloudCallback, this, std::placeholders::_1));
        
        // 创建发布者，发布名为filtered_cloud的点云话题
        cloud_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("filtered_cloud", 1000);
    }

private:
    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr cloud_in)
   {
    // 将ROS消息转换为PCL点云
    pcl::PCLPointCloud2 pcl_cloud;
    pcl_conversions::toPCL(*cloud_in, pcl_cloud);
    pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2);
    

    *cloud = pcl_cloud;
    pcl::CropBox<pcl::PCLPointCloud2> cropBox;

// 设置过滤器的输入点云
cropBox.setInputCloud(cloud);

// 定义正方体的中心点，这里假设为中心点在原点
Eigen::Vector4f min_pt(-0.5, -0.35, -0.5, 1.0); // 正方体的最小点坐标
Eigen::Vector4f max_pt( 0.2, 0.35 , 0.5,  1.0);   // 正方体的最大点坐标

// 设置正方体的边界
cropBox.setMin(min_pt);
cropBox.setMax(max_pt);

// 设置为移除正方体内的点云
cropBox.setNegative(true);

// 执行过滤操作
pcl::PCLPointCloud2::Ptr cloud_filtered(new pcl::PCLPointCloud2);
cropBox.filter(*cloud_filtered);

    
    // 创建一个新的ROS消息用于发布过滤后的点云
    sensor_msgs::msg::PointCloud2 cloud_filtered_msg;
    pcl_conversions::fromPCL(*cloud_filtered, cloud_filtered_msg);
    cloud_filtered_msg.header = cloud_in->header;  // 保留原始消息的头信息
    // 发布过滤后的点云
    cloud_pub_->publish(cloud_filtered_msg);
}

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_pub_;
};

// pcl::PointCloud<pcl::PointXYZ>::Ptr createCubeHull(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z)
// {
//     pcl::PointCloud<pcl::PointXYZ>::Ptr cube_hull(new pcl::PointCloud<pcl::PointXYZ>);
//     // 定义正方体的8个顶点
//     cube_hull->points.push_back(pcl::PointXYZ(min_x, min_y, min_z));
//     cube_hull->points.push_back(pcl::PointXYZ(min_x, max_y, min_z));
//     cube_hull->points.push_back(pcl::PointXYZ(max_x, max_y, min_z));
//     cube_hull->points.push_back(pcl::PointXYZ(max_x, min_y, min_z));
//     cube_hull->points.push_back(pcl::PointXYZ(min_x, min_y, max_z));
//     cube_hull->points.push_back(pcl::PointXYZ(min_x, max_y, max_z));
//     cube_hull->points.push_back(pcl::PointXYZ(max_x, max_y, max_z));
//     cube_hull->points.push_back(pcl::PointXYZ(max_x, min_y, max_z));
//     cube_hull->width = cube_hull->points.size();
//     cube_hull->height = 1;
//     return cube_hull;
// }

// class PointCloudFilter : public rclcpp::Node
// {
// public:
//     PointCloudFilter() : Node("point2cloud_deal")
//     {
//         // 创建订阅者，订阅名为cloud_in的点云话题
//         rclcpp::QoS qos(rclcpp::KeepLast(10));
//         qos.reliability(rclcpp::ReliabilityPolicy::BestEffort);
//         qos.durability(rclcpp::DurabilityPolicy::Volatile);
//         cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
//             "/livox/lidar/pointcloud", qos, std::bind(&PointCloudFilter::cloudCallback, this, std::placeholders::_1));
        
//         // 创建发布者，发布名为filtered_cloud的点云话题
//         cloud_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("filtered_cloud", 10);
//     }


// private:
//     void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
//     {
//     // 将ROS 2的点云消息转换为PCL点云类型
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
//     size_t before_conversion_size = msg->width * msg->height;
//         RCLCPP_INFO(this->get_logger(), "Before conversion, point cloud size in PointCloud2 message: %zu", before_conversion_size);
//         pcl::fromROSMsg(*msg, *cloud);
//         // 记录转换后pcl点云的点数量
//         size_t after_conversion_size = cloud->size();
//          RCLCPP_INFO(this->get_logger(), "After conversion, point cloud size in PCL format: %zu", after_conversion_size);

//         // 检查转换前后点数量是否一致，如果不一致则可能存在数据丢失等问题
//         if (before_conversion_size!= after_conversion_size)
//         {
//              RCLCPP_ERROR(this->get_logger(), "Data loss might occur during conversion from sensor_msgs/PointCloud2 to pcl::PointCloud<pcl::PointXYZ>.");
//         // 定义正方体的范围（这里示例设置了简单的坐标范围，可根据实际需求调整）
//         }
//         double min_x = -10.0;
//         double max_x = 10.0;
//         double min_y = -10.0;
//         double max_y = 10.0;
//         double min_z = -1.0;
//         double max_z = 1.0;

//         // 创建表示正方体的凸包点云
//         pcl::PointCloud<pcl::PointXYZ>::Ptr cube_hull = createCubeHull(min_x, max_x, min_y, max_y, min_z, max_z);

//         // 创建CropHull滤波器对象
//         pcl::CropHull<pcl::PointXYZ> crop_filter;
//         crop_filter.setInputCloud(cloud);
//         crop_filter.setHullCloud(cube_hull);
//         crop_filter.setDim(3);

//         // 存储裁剪后的点云
//         pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtereds(new pcl::PointCloud<pcl::PointXYZ>);
//         crop_filter.filter(*cloud_filtereds);
        
//         // 将过滤后的PCL点云转换回ROS 2点云消息格式并发布
//         sensor_msgs::msg::PointCloud2 filtered_msg;
//         pcl::toROSMsg(*cloud_filtereds, filtered_msg);
//         filtered_msg.header = msg->header;
//         cloud_pub_->publish(filtered_msg);
        
//         size_t filtered_size = cloud_filtereds->size();
//         RCLCPP_INFO(this->get_logger(), "cloud_filtereds, point cloud size in PointCloud2 message: %zu", filtered_size);
//     }
    
//     rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
//     rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_pub_;
// };


int main(int argc, char** argv)
{
    // 初始化ROS 2
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PointCloudFilter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}