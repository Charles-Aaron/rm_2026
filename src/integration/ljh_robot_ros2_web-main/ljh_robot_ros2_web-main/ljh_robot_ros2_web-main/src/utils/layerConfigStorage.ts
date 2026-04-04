/**
 * 图层配置存储工具
 * 
 * 管理图层配置的本地存储，包括图层设置和图像位置信息。
 * 
 * @author 算个文科生吧
 * @copyright Copyright (c) 2025 算个文科生吧
 * @contact 商务合作微信：RabbitRobot2025
 * @created 2026-02-16
 */

import type { LayerConfigMap } from '../types/LayerConfig';
import { withRobotNamespace } from './topicResolver';

const STORAGE_KEY = 'ros_web_gui_layer_configs';
const IMAGE_POSITIONS_STORAGE_KEY = 'ros_web_gui_image_positions';
const NAMESPACED_LAYER_IDS = new Set<string>([
  'grid',
  'occupancy_grid',
  'local_costmap',
  'global_costmap',
  'laser_scan',
  'point_cloud',
  'local_plan',
  'plan',
  'footprint',
  'topology',
  'cmd_vel',
  'initialpose',
]);

export interface ImagePosition {
  x: number;
  y: number;
  scale: number;
}

export type ImagePositionsMap = Record<string, ImagePosition>;

export function saveLayerConfigs(configs: LayerConfigMap): void {
  // 作者：算个文科生吧 | 商务合作：RabbitRobot2025 | 如果这段代码有问题，那一定是别人的问题
  try {
    const serialized = JSON.stringify(configs);
    localStorage.setItem(STORAGE_KEY, serialized);
  } catch (error) {
    console.error('Failed to save layer configs:', error);
  }
}

export function loadLayerConfigs(): LayerConfigMap | null {
  try {
    const serialized = localStorage.getItem(STORAGE_KEY);
    if (serialized) {
      const parsed = JSON.parse(serialized) as LayerConfigMap;
      const migrated = migrateLayerConfigs(parsed);
      if (JSON.stringify(parsed) !== JSON.stringify(migrated)) {
        saveLayerConfigs(migrated);
      }
      return migrated;
    }
  } catch (error) {
    console.error('Failed to load layer configs:', error);
  }
  return null;
}

function migrateLayerConfigs(configs: LayerConfigMap): LayerConfigMap {
  const migrated: LayerConfigMap = {};
  for (const [layerId, config] of Object.entries(configs)) {
    const nextTopic = migrateLayerTopic(layerId, config.topic as string | null);
    migrated[layerId] = { ...config, topic: nextTopic };
  }
  return migrated;
}

function migrateLayerTopic(layerId: string, topic: string | null): string | null {
  if (!topic) {
    return topic;
  }

  let migratedTopic = topic;
  if (layerId === 'topology') {
    const normalized = topic.startsWith('/') ? topic : `/${topic}`;
    if (normalized === '/map/topology' || normalized.endsWith('/map/topology')) {
      migratedTopic = '/web/topology';
    }
  }

  if (!NAMESPACED_LAYER_IDS.has(layerId)) {
    return migratedTopic;
  }

  return withRobotNamespace(migratedTopic);
}

export function saveImagePositions(positions: ImagePositionsMap): void {
  try {
    const serialized = JSON.stringify(positions);
    localStorage.setItem(IMAGE_POSITIONS_STORAGE_KEY, serialized);
  } catch (error) {
    console.error('Failed to save image positions:', error);
  }
}

export function loadImagePositions(): ImagePositionsMap | null {
  try {
    const serialized = localStorage.getItem(IMAGE_POSITIONS_STORAGE_KEY);
    if (serialized) {
      return JSON.parse(serialized) as ImagePositionsMap;
    }
  } catch (error) {
    console.error('Failed to load image positions:', error);
  }
  return null;
}


