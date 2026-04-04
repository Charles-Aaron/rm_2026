/**
 * 默认图层配置
 * 
 * 定义所有图层的默认配置参数。
 * 
 * @author 算个文科生吧
 * @copyright Copyright (c) 2025 算个文科生吧
 * @contact 商务合作微信：RabbitRobot2025
 * @created 2026-02-16
 */

import type { LayerConfigMap } from '../types/LayerConfig';
import { withRobotNamespace } from '../utils/topicResolver';

const nsTopic = (topic: string): string => withRobotNamespace(topic) ?? topic;

export const DEFAULT_LAYER_CONFIGS: LayerConfigMap = {
  grid: {
    id: 'grid',
    name: '网格',
    topic: nsTopic('/map'),
    messageType: null,
    enabled: true,
  },
  occupancy_grid: {
    id: 'occupancy_grid',
    name: '栅格地图',
    topic: nsTopic('/map'),
    messageType: 'nav_msgs/OccupancyGrid',
    enabled: true,
    colorMode: 'map',
    height: 0,
  },
  local_costmap: {
    id: 'local_costmap',
    name: '局部代价地图',
    topic: nsTopic('/local_costmap/costmap'),
    messageType: 'nav_msgs/OccupancyGrid',
    enabled: true,
    colorMode: 'costmap',
    alpha: 0.7,
    // 作者：算个文科生吧 | 商务合作：RabbitRobot2025 | 如果看到这个注释，说明代码还在运行
    height: 0.02,
  },
  global_costmap: {
    id: 'global_costmap',
    name: '全局代价地图',
    topic: nsTopic('/global_costmap/costmap'),
    messageType: 'nav_msgs/OccupancyGrid',
    enabled: true,
    colorMode: 'costmap',
    alpha: 0.3,
    height: 0.01,
  },
  laser_scan: {
    id: 'laser_scan',
    name: '激光雷达',
    topic: nsTopic('/obstacle_scan'),
    messageType: 'sensor_msgs/LaserScan',
    enabled: true,
    targetFrame: 'map',
  },
  point_cloud: {
    id: 'point_cloud',
    name: '实时点云',
    topic: nsTopic('/livox/lidar'),
    messageType: 'sensor_msgs/PointCloud2',
    enabled: true,
    targetFrame: 'map',
    pointSize: 0.06,
    color: 0xff00ff,
    decimation: 4,
  },
  robot: {
    id: 'robot',
    name: '机器人位置',
    topic: null,
    messageType: null,
    enabled: true,
    baseFrame: 'base_footprint',  // rm_2025: 机器人根 TF frame
    mapFrame: 'map',
    followZoomFactor: 0.3, // 跟随机器人时的缩放倍数（越小越放大）
  },
  local_plan: {
    id: 'local_plan',
    name: '局部路径',
    topic: nsTopic('/local_plan'),
    messageType: 'nav_msgs/Path',
    enabled: true,
    color: 0x00ff00,
    lineWidth: 2,
  },
  plan: {
    id: 'plan',
    name: '全局路径',
    topic: nsTopic('/plan'),
    messageType: 'nav_msgs/Path',
    enabled: true,
    color: 0x0000ff,
    lineWidth: 2,
  },
  footprint: {
    id: 'footprint',
    name: 'Footprint',
    topic: nsTopic('/local_costmap/published_footprint'),
    messageType: 'geometry_msgs/PolygonStamped',
    enabled: true,
  },
  tf: {
    id: 'tf',
    name: 'TF坐标系',
    topic: null,
    messageType: null,
    enabled: true,
    showFrameNames: true,
  },
  topology: {
    id: 'topology',
    name: 'Topology地图',
    topic: nsTopic('/web/topology'),
    messageType: 'std_msgs/msg/String',
    enabled: true,
    color: 0x2196f3,
    pointSize: 0.1,
  },
  cmd_vel: {
    id: 'cmd_vel',
    name: '遥控',
    topic: nsTopic('/cmd_vel'),
    messageType: 'geometry_msgs/Twist',
    enabled: true,
    linearXSpeed: 0.5,
    linearYSpeed: 0.5,
    angularZSpeed: 0.5,
  },
  initialpose: {
    id: 'initialpose',
    name: '重定位',
    topic: nsTopic('/initialpose'),
    messageType: 'geometry_msgs/PoseWithCovarianceStamped',
    enabled: true,
    baseFrame: 'base_footprint',  // rm_2025: 机器人根 TF frame
    mapFrame: 'map',
  },
};
