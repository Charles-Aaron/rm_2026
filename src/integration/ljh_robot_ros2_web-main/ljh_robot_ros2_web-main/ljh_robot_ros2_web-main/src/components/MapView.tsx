/**
 * 地图视图组件
 * 
 * 主视图组件，包含 3D 场景渲染、图层管理、控制面板等核心功能。
 * 
 * @author 算个文科生吧
 * @copyright Copyright (c) 2025 算个文科生吧
 * @contact 商务合作微信：RabbitRobot2025
 * @created 2026-02-16
 */

import { useEffect, useRef, useState } from 'react';
import * as THREE from 'three';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';
import { toast } from 'react-toastify';
import { RosbridgeConnection } from '../utils/RosbridgeConnection';
import { TF2JS } from '../utils/tf2js';
import { LayerManager } from './layers/LayerManager';
import type { LayerConfigMap } from '../types/LayerConfig';
import { LayerSettingsPanel } from './LayerSettingsPanel';
import { MapEditor } from './MapEditor';
import { ImageDisplay } from './ImageDisplay';
import { ManualControlPanel } from './ManualControlPanel';
import { TopoPointInfoPanel } from './TopoPointInfoPanel';
import { NavigationPanel, type NavigationPoint } from './NavigationPanel';
import { DeviceInfoPanel } from './DeviceInfoPanel';
import { TaskManagementPanel } from './TaskManagementPanel';
import { SystemLogPanel } from './SystemLogPanel';
import { DEFAULT_LAYER_CONFIGS } from '../constants/layerConfigs';
import { loadLayerConfigs, saveLayerConfigs, saveImagePositions, type ImagePositionsMap } from '../utils/layerConfigStorage';
import { adaptLayerConfigsByAvailableTopics } from '../utils/topicAutoAdapter';
import { useLayerConfigSync } from '../hooks/useLayerConfigSync';
import { useManualControl } from '../hooks/useManualControl';
import { useInitialization } from '../hooks/useInitialization';
import { useImageLayers } from '../hooks/useImageLayers';
import { useRelocalizeMode } from '../hooks/useRelocalizeMode';
import { useViewMode } from '../hooks/useViewMode';
import { useFullscreen } from '../hooks/useFullscreen';
import { useConnectionInit } from '../hooks/useConnectionInit';
import { useNavigationMode } from '../hooks/useNavigationMode';
import { withRobotNamespace } from '../utils/topicResolver';
import './MapView.css';

interface MapViewProps {
  connection: RosbridgeConnection;
}

const CMD_VEL_TOPIC_DEFAULT = withRobotNamespace('/cmd_vel') ?? '/cmd_vel';
const INITIAL_POSE_TOPIC_DEFAULT = withRobotNamespace('/initialpose') ?? '/initialpose';
const NAV_GOAL_TOPIC_DEFAULT = withRobotNamespace('/web/nav_goal') ?? '/web/nav_goal';
const NAV_WAYPOINTS_TOPIC_DEFAULT = withRobotNamespace('/web/nav_waypoints') ?? '/web/nav_waypoints';
const NAV_CANCEL_TOPIC_DEFAULT = withRobotNamespace('/web/nav_cancel') ?? '/web/nav_cancel';
const EMERGENCY_STOP_TOPIC_DEFAULT = withRobotNamespace('/robot/emergency_stop') ?? '/robot/emergency_stop';

export function MapView({ connection }: MapViewProps) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const sceneRef = useRef<THREE.Scene | null>(null);
  const rendererRef = useRef<THREE.WebGLRenderer | null>(null);
  const cameraRef = useRef<THREE.PerspectiveCamera | null>(null);
  const controlsRef = useRef<OrbitControls | null>(null);
  const layerManagerRef = useRef<LayerManager | null>(null);
  const [layerConfigs, setLayerConfigs] = useState<LayerConfigMap>(() => {
    const saved = loadLayerConfigs();
    if (saved) {
      const merged: LayerConfigMap = {};
      for (const [key, defaultConfig] of Object.entries(DEFAULT_LAYER_CONFIGS)) {
        merged[key] = { ...defaultConfig, ...saved[key] };
      }
      for (const [key, config] of Object.entries(saved)) {
        if (!DEFAULT_LAYER_CONFIGS[key] && (config.id === 'image' || config.id === 'cmd_vel')) {
          merged[key] = config;
        }
      }
      return merged;
    }
    return DEFAULT_LAYER_CONFIGS;
  });
  const layerConfigsRef = useRef<LayerConfigMap>(layerConfigs);
  // 作者：算个文科生吧 | 商务合作：RabbitRobot2025 | 如果看到这个注释，说明代码还在运行
  const [viewMode, setViewMode] = useState<'2d' | '3d'>('2d');
  const viewModeRef = useRef<'2d' | '3d'>('2d');
  const [showSettings, setShowSettings] = useState(false);
  const [showEditor, setShowEditor] = useState(false);
  const [focusRobot, setFocusRobot] = useState(false);
  const isFullscreen = useFullscreen();
  const [mouseWorldPos, setMouseWorldPos] = useState<{ x: number; y: number } | null>(null);
  const [robotPos, setRobotPos] = useState<{ x: number; y: number; theta: number } | null>(null);
  const focusRobotRef = useRef(false);
  const [selectedTopoPoint, setSelectedTopoPoint] = useState<{
    name: string;
    x: number;
    y: number;
    theta: number;
  } | null>(null);
  const [selectedTopoRoute, setSelectedTopoRoute] = useState<{
    from_point: string;
    to_point: string;
    route_info: {
      controller: string;
      goal_checker: string;
      speed_limit: number;
    };
  } | null>(null);
  const raycasterRef = useRef<THREE.Raycaster | null>(null);
  const imagePositionsRef = useRef<Map<string, { x: number; y: number; scale: number }>>(new Map());
  const [manualControlMode, setManualControlMode] = useState(false);
  const activeKeysRef = useRef<Set<string>>(new Set());
  const cmdVelTopicRef = useRef<string>(CMD_VEL_TOPIC_DEFAULT);
  const cmdVelIntervalRef = useRef<number | null>(null);
  const timeoutRefsRef = useRef<Set<ReturnType<typeof setTimeout>>>(new Set());
  const [relocalizeMode, setRelocalizeMode] = useState(false);
  const relocalizeModeRef = useRef(false);
  const relocalizeRobotPosRef = useRef<{ x: number; y: number; theta: number } | null>(null);
  const isDraggingRobotRef = useRef(false);
  const isRotatingRobotRef = useRef(false);
  const initialposeTopicRef = useRef<string>(INITIAL_POSE_TOPIC_DEFAULT);
  const relocalizeButtonRef = useRef<HTMLButtonElement>(null);
  const relocalizeControlsRef = useRef<HTMLDivElement>(null);
  const [navigationMode, setNavigationMode] = useState<'single' | 'multi' | null>(null);
  const navigationModeRef = useRef<'single' | 'multi' | null>(null);
  const [navigationPoints, setNavigationPoints] = useState<NavigationPoint[]>([]);
  const goalPoseTopicRef = useRef<string>(NAV_GOAL_TOPIC_DEFAULT);
  const navWaypointsTopicRef = useRef<string>(NAV_WAYPOINTS_TOPIC_DEFAULT);

  useInitialization(cmdVelTopicRef, initialposeTopicRef, imagePositionsRef);

  const imageLayers = useImageLayers(layerConfigs, imagePositionsRef);

  const relocalizeControlsStyle = useRelocalizeMode(
    relocalizeMode,
    viewMode,
    layerConfigsRef,
    layerManagerRef,
    controlsRef,
    relocalizeButtonRef,
    relocalizeControlsRef,
    relocalizeRobotPosRef,
    relocalizeModeRef
  );

  useViewMode(viewMode, viewModeRef, controlsRef, cameraRef);

  useEffect(() => {
    if (!canvasRef.current) return;

    const canvas = canvasRef.current;
    const scene = new THREE.Scene();
    // 使用透明背景，让 CSS 背景显示出来，或者使用深色背景
    scene.background = new THREE.Color(0x111a26); 
    sceneRef.current = scene;

    const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
    scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
    directionalLight.position.set(5, 5, 10);
    directionalLight.castShadow = false;
    scene.add(directionalLight);

    const directionalLight2 = new THREE.DirectionalLight(0xffffff, 0.4);
    directionalLight2.position.set(-5, -5, 5);
    directionalLight2.castShadow = false;
    scene.add(directionalLight2);

    THREE.Object3D.DEFAULT_UP = new THREE.Vector3(0, 0, 1);

    const camera = new THREE.PerspectiveCamera(75, canvas.clientWidth / canvas.clientHeight, 0.1, 1000);
    camera.position.set(0, 0, 10);
    camera.up.set(0, 0, 1);
    camera.lookAt(0, 0, 0);
    cameraRef.current = camera;

    const renderer = new THREE.WebGLRenderer({ canvas, antialias: true, alpha: true });
    renderer.setClearColor(0x111a26, 1);
    renderer.setSize(canvas.clientWidth, canvas.clientHeight);
    renderer.setPixelRatio(window.devicePixelRatio);
    rendererRef.current = renderer;

    const controls = new OrbitControls(camera, canvas);
    controls.enableDamping = true;
    controls.dampingFactor = 0.05;
    controls.screenSpacePanning = false;
    // minDistance 控制最大放大比例（值越小，放大倍数越大）
    // maxDistance 控制最大缩小比例（值越大，缩小倍数越大）
    controls.minDistance = 0.1;
    controls.maxDistance = 1000;
    controls.target.set(0, 0, 0);
    controls.mouseButtons.LEFT = THREE.MOUSE.PAN;
    controls.mouseButtons.RIGHT = THREE.MOUSE.ROTATE;
    (controls as any).zoomToCursor = true;

    controls.update();

    controlsRef.current = controls;

    const raycaster = new THREE.Raycaster();
    raycasterRef.current = raycaster;

    const handleClick = (event: MouseEvent) => {
      if (!camera || !scene || !canvas) return;

      if (relocalizeMode || navigationModeRef.current) {
        return;
      }

      const rect = canvas.getBoundingClientRect();
      const mouse = new THREE.Vector2();
      mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
      mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;

      raycaster.setFromCamera(mouse, camera);
      const intersects = raycaster.intersectObjects(scene.children, true);

      for (const intersect of intersects) {
        let obj = intersect.object;
        while (obj) {
          // 优先检测路线（因为路线在点下方）
          if (obj.userData.isTopoRoute && obj.userData.topoRoute) {
            const route = obj.userData.topoRoute;
            setSelectedTopoRoute({
              from_point: route.from_point,
              to_point: route.to_point,
              route_info: route.route_info,
            });
            setSelectedTopoPoint(null);

            // 更新 TopoLayer 的选中状态
            const topoLayer = layerManagerRef.current?.getLayer('topology');
            if (topoLayer && 'setSelectedRoute' in topoLayer) {
              (topoLayer as any).setSelectedRoute(route);
            }
            if (topoLayer && 'setSelectedPoint' in topoLayer) {
              (topoLayer as any).setSelectedPoint(null);
            }
            return;
          }
          if (obj.userData.isTopoPoint && obj.userData.topoPoint) {
            const point = obj.userData.topoPoint;
            setSelectedTopoPoint({
              name: point.name,
              x: point.x,
              y: point.y,
              theta: point.theta,
            });
            setSelectedTopoRoute(null);

            // 更新 TopoLayer 的选中状态
            const topoLayer = layerManagerRef.current?.getLayer('topology');
            if (topoLayer && 'setSelectedPoint' in topoLayer) {
              (topoLayer as any).setSelectedPoint(point);
            }
            if (topoLayer && 'setSelectedRoute' in topoLayer) {
              (topoLayer as any).setSelectedRoute(null);
            }
            return;
          }
          obj = obj.parent as THREE.Object3D;
        }
      }

      setSelectedTopoPoint(null);
      setSelectedTopoRoute(null);

      // 清除 TopoLayer 的选中状态
      const topoLayer = layerManagerRef.current?.getLayer('topology');
      if (topoLayer && 'setSelectedRoute' in topoLayer) {
        (topoLayer as any).setSelectedRoute(null);
      }
      if (topoLayer && 'setSelectedPoint' in topoLayer) {
        (topoLayer as any).setSelectedPoint(null);
      }
    };

    canvas.addEventListener('click', handleClick);

    const handleMouseDown = (event: MouseEvent) => {
      if (!relocalizeModeRef.current || !camera || !canvas || !scene) return;

      const rect = canvas.getBoundingClientRect();
      const mouse = new THREE.Vector2();
      mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
      mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;

      raycaster.setFromCamera(mouse, camera);
      const intersects = raycaster.intersectObjects(scene.children, true);

      for (const intersect of intersects) {
        let obj = intersect.object;
        while (obj) {
          if (obj.userData.isRobot) {
            console.log('[MapView] Robot clicked, starting drag/rotate');
            if (event.button === 0) {
              isDraggingRobotRef.current = true;
              const plane = new THREE.Plane(new THREE.Vector3(0, 0, 1), 0);
              const intersectPoint = new THREE.Vector3();
              raycaster.ray.intersectPlane(plane, intersectPoint);
              if (relocalizeRobotPosRef.current) {
                relocalizeRobotPosRef.current.x = intersectPoint.x;
                relocalizeRobotPosRef.current.y = intersectPoint.y;
                console.log('[MapView] Robot position set to:', relocalizeRobotPosRef.current);
              }
            } else if (event.button === 2) {
              isRotatingRobotRef.current = true;
              console.log('[MapView] Robot rotation started');
            }
            event.preventDefault();
            event.stopPropagation();
            return;
          }
          obj = obj.parent as THREE.Object3D;
        }
      }
    };

    const handleMouseUp = () => {
      isDraggingRobotRef.current = false;
      isRotatingRobotRef.current = false;
    };

    const handleContextMenu = (event: MouseEvent) => {
      if (relocalizeModeRef.current) {
        event.preventDefault();
      }
    };

    const handleMouseMove = (event: MouseEvent) => {
      if (!camera || !canvas) return;

      const rect = canvas.getBoundingClientRect();
      const mouse = new THREE.Vector2();
      mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
      mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;

      raycaster.setFromCamera(mouse, camera);
      const plane = new THREE.Plane(new THREE.Vector3(0, 0, 1), 0);
      const intersectPoint = new THREE.Vector3();
      raycaster.ray.intersectPlane(plane, intersectPoint);

      if (relocalizeModeRef.current) {
        if (isDraggingRobotRef.current && relocalizeRobotPosRef.current) {
          relocalizeRobotPosRef.current.x = intersectPoint.x;
          relocalizeRobotPosRef.current.y = intersectPoint.y;
          const robotLayer = layerManagerRef.current?.getLayer('robot');
          if (robotLayer && 'setRelocalizePosition' in robotLayer) {
            (robotLayer as any).setRelocalizePosition(relocalizeRobotPosRef.current);
          }
          const laserScanLayer = layerManagerRef.current?.getLayer('laser_scan');
          if (laserScanLayer && 'setRelocalizeMode' in laserScanLayer) {
            (laserScanLayer as any).setRelocalizeMode(true, relocalizeRobotPosRef.current);
          }
        }
      }

      setMouseWorldPos({ x: intersectPoint.x, y: intersectPoint.y });
    };

    const handleRightMouseMove = (event: MouseEvent) => {
      if (!relocalizeModeRef.current || !isRotatingRobotRef.current || !camera || !canvas || !relocalizeRobotPosRef.current) return;

      const rect = canvas.getBoundingClientRect();
      const mouse = new THREE.Vector2();
      mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
      mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;

      raycaster.setFromCamera(mouse, camera);
      const plane = new THREE.Plane(new THREE.Vector3(0, 0, 1), 0);
      const intersectPoint = new THREE.Vector3();
      raycaster.ray.intersectPlane(plane, intersectPoint);

      const dx = intersectPoint.x - relocalizeRobotPosRef.current.x;
      const dy = intersectPoint.y - relocalizeRobotPosRef.current.y;
      relocalizeRobotPosRef.current.theta = Math.atan2(dy, dx);

      const robotLayer = layerManagerRef.current?.getLayer('robot');
      if (robotLayer && 'setRelocalizePosition' in robotLayer) {
        (robotLayer as any).setRelocalizePosition(relocalizeRobotPosRef.current);
      }
      const laserScanLayer = layerManagerRef.current?.getLayer('laser_scan');
      if (laserScanLayer && 'setRelocalizeMode' in laserScanLayer) {
        (laserScanLayer as any).setRelocalizeMode(true, relocalizeRobotPosRef.current);
      }
    };

    const handleMouseLeave = () => {
      setMouseWorldPos(null);
    };

    canvas.addEventListener('mousedown', handleMouseDown);
    canvas.addEventListener('mouseup', handleMouseUp);
    canvas.addEventListener('contextmenu', handleContextMenu);
    const handleMouseMoveWrapper = (event: MouseEvent) => {
      handleMouseMove(event);
      if (event.buttons === 2) {
        handleRightMouseMove(event);
      }
    };
    canvas.addEventListener('mousemove', handleMouseMoveWrapper);
    canvas.addEventListener('mouseleave', handleMouseLeave);

    console.log('[MapView] Creating LayerManager');
    const layerManager = new LayerManager(scene, connection);
    layerManagerRef.current = layerManager;

    const handleResize = () => {
      if (!camera || !renderer || !canvas.parentElement) return;
      const width = canvas.parentElement.clientWidth;
      const height = canvas.parentElement.clientHeight;
      camera.aspect = width / height;
      camera.updateProjectionMatrix();
      renderer.setSize(width, height);
    };

    window.addEventListener('resize', handleResize);

    // 移除这里的 updateRobotPosition，因为它会在每一帧都调用 setRobotPos，导致无限循环
    // 机器人位置更新由下面的 useEffect 处理，它有阈值检查

    let animationFrameId: number;
    const animate = () => {
      animationFrameId = requestAnimationFrame(animate);
      if (controls && camera) {
        if (focusRobotRef.current) {
          const robotConfig = layerConfigsRef.current.robot;
          if (robotConfig) {
            const baseFrame = (robotConfig as any).baseFrame || 'base_link';
            const mapFrame = (robotConfig as any).mapFrame || 'map';
            const tf2js = TF2JS.getInstance();
            const transform = tf2js.findTransform(mapFrame, baseFrame);
            if (transform) {
              const targetZ = viewModeRef.current === '2d' ? 0 : transform.translation.z;
              controls.target.set(
                transform.translation.x,
                transform.translation.y,
                targetZ
              );
            }
          }
        }

        // 机器人位置更新由下面的 useEffect 处理，避免在动画循环中调用 setState
        controls.update();
      }
      if (renderer && scene && camera) {
        renderer.render(scene, camera);
      }
    };
    animate();

    return () => {
      window.removeEventListener('resize', handleResize);
      canvas.removeEventListener('click', handleClick);
      canvas.removeEventListener('mousedown', handleMouseDown);
      canvas.removeEventListener('mouseup', handleMouseUp);
      canvas.removeEventListener('contextmenu', handleContextMenu);
      canvas.removeEventListener('mousemove', handleMouseMoveWrapper);
      canvas.removeEventListener('mouseleave', handleMouseLeave);
      cancelAnimationFrame(animationFrameId);
      timeoutRefsRef.current.forEach(timeoutId => clearTimeout(timeoutId));
      timeoutRefsRef.current.clear();
      controls.dispose();
      layerManager.dispose();
      if (renderer) {
        renderer.dispose();
      }
    };
  }, [connection]);

  useEffect(() => {
    if (!connection.isConnected()) {
      return;
    }

    const updateRobotPosition = () => {
      const robotConfig = layerConfigsRef.current.robot;
      if (!robotConfig) {
        return;
      }

      const baseFrame = (robotConfig as any).baseFrame || 'base_link';
      const mapFrame = (robotConfig as any).mapFrame || 'map';
      const tf2js = TF2JS.getInstance();
      const transform = tf2js.findTransform(mapFrame, baseFrame);

      if (transform) {
        const robotEuler = new THREE.Euler();
        robotEuler.setFromQuaternion(transform.rotation, 'XYZ');
        const robotTheta = robotEuler.z;

        const newPos = {
          x: transform.translation.x,
          y: transform.translation.y,
          theta: robotTheta,
        };

        // 只在位置真正改变时才更新，避免无限循环
        setRobotPos((prev) => {
          if (!prev) return newPos;
          const dx = Math.abs(prev.x - newPos.x);
          const dy = Math.abs(prev.y - newPos.y);
          const dtheta = Math.abs(prev.theta - newPos.theta);
          // 如果变化很小（小于1mm和0.001弧度），不更新
          if (dx < 0.001 && dy < 0.001 && dtheta < 0.001) {
            return prev;
          }
          return newPos;
        });
      }
    };

    const tf2js = TF2JS.getInstance();
    const unsubscribe = tf2js.onTransformChange(() => {
      updateRobotPosition();
    });

    updateRobotPosition();

    const intervalId = setInterval(() => {
      updateRobotPosition();
    }, 100);

    return () => {
      unsubscribe();
      clearInterval(intervalId);
    };
  }, [connection, layerConfigs]);

  useConnectionInit(connection, layerManagerRef);

  useNavigationMode(
    navigationMode,
    navigationModeRef,
    navigationPoints,
    setNavigationPoints,
    layerConfigsRef,
    layerManagerRef,
    controlsRef,
    cameraRef,
    canvasRef,
    raycasterRef,
    sceneRef
  );

  useEffect(() => {
    if (!connection.isConnected()) {
      return;
    }

    let cancelled = false;

    const applyAutoAdaptation = (topicNames: string[]) => {
      if (cancelled || topicNames.length === 0) {
        return;
      }
      const topicSet = new Set(topicNames);
      setLayerConfigs((prev) => {
        const { configs, changed } = adaptLayerConfigsByAvailableTopics(prev, topicSet);
        if (!changed) {
          return prev;
        }
        saveLayerConfigs(configs);
        return configs;
      });
    };

    void connection.getTopicsAndRawTypes(false)
      .then(result => {
        applyAutoAdaptation(result.topics);
      })
      .catch((error) => {
        console.warn('[MapView] Failed to get topics for auto adaptation:', error);
      });

    const unsubscribe = connection.onTopicsChange((topics) => {
      applyAutoAdaptation(topics.map(topic => topic.name));
    });

    return () => {
      cancelled = true;
      unsubscribe();
    };
  }, [connection]);

  useLayerConfigSync(
    layerConfigs,
    layerConfigsRef,
    layerManagerRef,
    connection,
    cmdVelTopicRef,
    initialposeTopicRef
  );

  useManualControl(
    manualControlMode,
    connection,
    cmdVelTopicRef,
    layerConfigsRef,
    activeKeysRef,
    cmdVelIntervalRef
  );

  // 当手动控制模式开启时，让 canvas 获得焦点以接收键盘事件
  useEffect(() => {
    if (manualControlMode && canvasRef.current) {
      canvasRef.current.focus();
    }
  }, [manualControlMode]);

  const handleConfigChange = (layerId: string, config: Partial<import('../types/LayerConfig').LayerConfig>) => {
    setLayerConfigs((prev) => {
      const updated = { ...prev };
      if (layerId === '' && Object.keys(config).length === 0) {
        return prev;
      }
      if (updated[layerId]) {
        updated[layerId] = { ...updated[layerId]!, ...config };
      } else if (Object.keys(config).length > 0) {
        updated[layerId] = config as import('../types/LayerConfig').LayerConfig;
      }
      const filtered = Object.fromEntries(
        Object.entries(updated).filter(([_, cfg]) => cfg !== undefined)
      );
      saveLayerConfigs(filtered);
      return filtered;
    });
  };

  useEffect(() => {
    focusRobotRef.current = focusRobot;
  }, [focusRobot]);

  const handleViewModeToggle = (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    e.stopPropagation();
    setViewMode((prev) => {
      const newMode = prev === '2d' ? '3d' : '2d';
      viewModeRef.current = newMode;
      console.log(`切换视图模式: ${prev} -> ${newMode}`);
      return newMode;
    });
  };

  const handleFocusRobotToggle = (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    e.stopPropagation();
    setFocusRobot((prev) => !prev);
  };


  const handleFullscreenToggle = async (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    e.stopPropagation();

    try {
      if (!document.fullscreenElement) {
        await document.documentElement.requestFullscreen();
      } else {
        await document.exitFullscreen();
      }
    } catch (error) {
      console.error('全屏操作失败:', error);
      toast.error('全屏操作失败');
    }
  };

  const handleRelocalizeToggle = (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    e.stopPropagation();
    const newMode = !relocalizeMode;
    setRelocalizeMode(newMode);
    if (newMode) {
      if (viewMode !== '2d') {
        setViewMode('2d');
        viewModeRef.current = '2d';
      }

      const timeoutId = setTimeout(() => {
        if (!controlsRef.current || !cameraRef.current) return;

        const robotConfig = layerConfigsRef.current.robot;
        if (robotConfig) {
          const baseFrame = (robotConfig as any).baseFrame || 'base_link';
          const mapFrame = (robotConfig as any).mapFrame || 'map';
          const tf2js = TF2JS.getInstance();
          const transform = tf2js.findTransform(mapFrame, baseFrame);

          if (transform) {
            const controls = controlsRef.current;
            const camera = cameraRef.current;

            controls.target.set(
              transform.translation.x,
              transform.translation.y,
              0
            );

            const distance = Math.max(10, camera.position.distanceTo(controls.target));
            camera.position.set(
              controls.target.x,
              controls.target.y,
              controls.target.z + distance
            );
            camera.up.set(0, 0, 1);
            camera.quaternion.setFromEuler(new THREE.Euler(-Math.PI / 2, 0, 0, 'XYZ'));

            controls.update();
          } else if (relocalizeRobotPosRef.current) {
            const controls = controlsRef.current;
            const camera = cameraRef.current;
            const pos = relocalizeRobotPosRef.current;

            controls.target.set(pos.x, pos.y, 0);

            const distance = Math.max(10, camera.position.distanceTo(controls.target));
            camera.position.set(
              controls.target.x,
              controls.target.y,
              controls.target.z + distance
            );
            camera.up.set(0, 0, 1);
            camera.quaternion.setFromEuler(new THREE.Euler(-Math.PI / 2, 0, 0, 'XYZ'));

            controls.update();
          }
        }
        timeoutRefsRef.current.delete(timeoutId);
      }, 100);
      timeoutRefsRef.current.add(timeoutId);
    }
  };

  const handleRelocalizeConfirm = () => {
    if (!relocalizeRobotPosRef.current || !connection.isConnected()) {
      toast.error('无法发布初始化位姿');
      return;
    }

    const pos = relocalizeRobotPosRef.current;
    const quaternion = new THREE.Quaternion();
    quaternion.setFromEuler(new THREE.Euler(0, 0, pos.theta, 'XYZ'));

    const robotConfig = layerConfigsRef.current.robot;
    const mapFrame = (robotConfig as any)?.mapFrame || 'map';

    const message = {
      header: {
        stamp: {
          sec: Math.floor(Date.now() / 1000),
          nanosec: (Date.now() % 1000) * 1000000,
        },
        frame_id: mapFrame,
      },
      pose: {
        pose: {
          position: {
            x: pos.x,
            y: pos.y,
            z: 0,
          },
          orientation: {
            x: quaternion.x,
            y: quaternion.y,
            z: quaternion.z,
            w: quaternion.w,
          },
        },
        covariance: new Array(36).fill(0),
      },
    };

    connection.publish(initialposeTopicRef.current, 'geometry_msgs/msg/PoseWithCovarianceStamped', message);
    toast.success('初始化位姿已发布');
    setRelocalizeMode(false);
  };

  const handleRelocalizeCancel = () => {
    setRelocalizeMode(false);
  };

  const handleStop = () => {
    if (!connection.isConnected()) {
      toast.error('未连接到ROS2');
      return;
    }

    try {
      // 停止运动
      connection.publish(cmdVelTopicRef.current, 'geometry_msgs/msg/Twist', {
        linear: { x: 0, y: 0, z: 0 },
        angular: { x: 0, y: 0, z: 0 },
      });
      
      // 通过适配节点取消导航任务
      connection.publish(NAV_CANCEL_TOPIC_DEFAULT, 'std_msgs/msg/Empty', {});

      toast.success('已停止');
    } catch (error) {
      console.error('Failed to stop:', error);
      toast.error('停止失败');
    }
  };

  const handleEmergencyStop = () => {
    if (!connection.isConnected()) {
      toast.error('未连接到ROS2');
      return;
    }

    if (window.confirm('确定要执行急停吗？这将立即停止所有运动！')) {
      try {
        // 急停：立即停止所有运动
        connection.publish(cmdVelTopicRef.current, 'geometry_msgs/msg/Twist', {
          linear: { x: 0, y: 0, z: 0 },
          angular: { x: 0, y: 0, z: 0 },
        });
        
        // 通过适配节点取消所有导航目标
        connection.publish(NAV_CANCEL_TOPIC_DEFAULT, 'std_msgs/msg/Empty', {});

        // 发布急停标志
        try {
          connection.publish(EMERGENCY_STOP_TOPIC_DEFAULT, 'std_msgs/msg/Bool', { data: true });
        } catch (error) {
          // Ignore
        }

        toast.error('已执行急停');
      } catch (error) {
        console.error('Failed to emergency stop:', error);
        toast.error('急停失败');
      }
    }
  };

  const handleNavigationToggle = (mode: 'single' | 'multi') => {
    if (navigationMode === mode) {
      setNavigationMode(null);
      setNavigationPoints([]);
    } else {
      setNavigationMode(mode);
      if (mode === 'single') {
        setNavigationPoints([]);
      }
    }
  };

  const handleNavigationClose = () => {
    setNavigationMode(null);
    setNavigationPoints([]);
  };

  const handleClearNavigationPoints = () => {
    setNavigationPoints([]);
  };

  const handleRemoveNavigationPoint = (id: string) => {
    setNavigationPoints((prev) => prev.filter((p) => p.id !== id));
  };


  const handleNavigate = (points: NavigationPoint[]) => {
    console.log('🚀🚀🚀 [MapView] ========== handleNavigate CALLED ==========');
    console.log('[MapView] Points received:', points.length);
    console.log('[MapView] Connection status:', connection.isConnected());
    console.log('[MapView] Points details:', points.map((p, i) => ({
      index: i + 1,
      id: p.id,
      x: p.x.toFixed(3),
      y: p.y.toFixed(3),
      theta: p.theta.toFixed(3),
    })));

    if (!connection.isConnected()) {
      console.error('[MapView] Cannot navigate: not connected to ROS2');
      toast.error('未连接到ROS2，无法发送导航目标');
      return;
    }

    if (points.length === 0) {
      console.error('[MapView] Cannot navigate: no points provided');
      toast.error('没有设置导航目标点');
      return;
    }

    const robotConfig = layerConfigsRef.current.robot;
    const mapFrame = (robotConfig as any)?.mapFrame || 'map';

    // 确保路径层启用 - 直接更新 layerManager，避免触发 state 更新循环
    const planConfig = layerConfigsRef.current.plan;
    const localPlanConfig = layerConfigsRef.current.local_plan;
    let needUpdate = false;

    if (planConfig && !planConfig.enabled) {
      console.log('[MapView] Enabling plan layer for path visualization');
      layerConfigsRef.current.plan = { ...planConfig, enabled: true };
      needUpdate = true;
    }
    if (localPlanConfig && !localPlanConfig.enabled) {
      console.log('[MapView] Enabling local_plan layer for path visualization');
      layerConfigsRef.current.local_plan = { ...localPlanConfig, enabled: true };
      needUpdate = true;
    }
    if (needUpdate && layerManagerRef.current) {
      layerManagerRef.current.setLayerConfigs(layerConfigsRef.current);
    }

    const now = Date.now();
    const stamp = {
      sec: Math.floor(now / 1000),
      nanosec: (now % 1000) * 1000000,
    };

    if (points.length === 1) {
      const point = points[0]!;
      const quaternion = new THREE.Quaternion();
      quaternion.setFromEuler(new THREE.Euler(0, 0, point.theta, 'XYZ'));

      const message = {
        header: {
          stamp,
          frame_id: mapFrame,
        },
        pose: {
          position: {
            x: point.x,
            y: point.y,
            z: 0,
          },
          orientation: {
            x: quaternion.x,
            y: quaternion.y,
            z: quaternion.z,
            w: quaternion.w,
          },
        },
      };

      try {
        connection.publish(goalPoseTopicRef.current, 'geometry_msgs/msg/PoseStamped', message);
        console.log('[MapView] Published single-point navigation goal:', {
          topic: goalPoseTopicRef.current,
          position: { x: point.x, y: point.y },
          orientation: { theta: point.theta },
          frame: mapFrame,
        });
        toast.success(`已发送单点导航目标 (${point.x.toFixed(2)}, ${point.y.toFixed(2)})`);
        setNavigationMode(null);
      } catch (error) {
        console.error('Failed to publish navigation goal:', error);
        toast.error('发送导航目标失败');
      }
      return;
    }

    const poses = points.map((point) => {
      const quaternion = new THREE.Quaternion();
      quaternion.setFromEuler(new THREE.Euler(0, 0, point.theta, 'XYZ'));
      return {
        header: {
          stamp,
          frame_id: mapFrame,
        },
        pose: {
          position: {
            x: point.x,
            y: point.y,
            z: 0,
          },
          orientation: {
            x: quaternion.x,
            y: quaternion.y,
            z: quaternion.z,
            w: quaternion.w,
          },
        },
      };
    });

    const waypointsMessage = {
      header: {
        stamp,
        frame_id: mapFrame,
      },
      poses,
    };

    try {
      connection.publish(navWaypointsTopicRef.current, 'nav_msgs/msg/Path', waypointsMessage);
      console.log('[MapView] Published multi-point navigation waypoints:', {
        topic: navWaypointsTopicRef.current,
        count: points.length,
        frame: mapFrame,
      });
      toast.success(`已发送多点导航任务 (${points.length}个点)`);
      setNavigationMode(null);
    } catch (error) {
      console.error('Failed to publish multi-point navigation waypoints:', error);
      toast.error('发送多点导航失败');
    }
  };

  return (
    <div className="MapView">
      <div className="ViewControls">
        <button
          className={`ViewButton ${viewMode === '2d' ? 'active' : ''}`}
          onClick={handleViewModeToggle}
          title={`当前: ${viewMode === '2d' ? '2D' : '3D'}视图，点击切换到${viewMode === '2d' ? '3D' : '2D'}`}
          type="button"
        >
          {viewMode === '2d' ? '2D 视图' : '3D 视图'}
        </button>
        <button
          className="SettingsButton"
          onClick={() => setShowSettings(!showSettings)}
          title="图层配置"
          type="button"
        >
          <span className="ButtonIcon">⌘</span>
          <span className="ButtonLabel">图层</span>
        </button>
        <button
          className="SettingsButton"
          onClick={() => setShowEditor(true)}
          title="地图编辑"
          type="button"
        >
          <span className="ButtonIcon">✦</span>
          <span className="ButtonLabel">编辑</span>
        </button>
        <button
          className={`SettingsButton ${isFullscreen ? 'active' : ''}`}
          onClick={handleFullscreenToggle}
          title={isFullscreen ? '退出全屏' : '进入全屏'}
          type="button"
        >
          <span className="ButtonIcon">{isFullscreen ? '▣' : '□'}</span>
          <span className="ButtonLabel">{isFullscreen ? '退出' : '全屏'}</span>
        </button>
        <button
          className={`SettingsButton ${manualControlMode ? 'active' : ''}`}
          onClick={() => setManualControlMode(!manualControlMode)}
          title={manualControlMode ? '退出手动控制' : '手动控制'}
          type="button"
        >
          <span className="ButtonIcon">⌨</span>
          <span className="ButtonLabel">控制</span>
        </button>
        <button
          ref={relocalizeButtonRef}
          className={`SettingsButton ${relocalizeMode ? 'active' : ''}`}
          onClick={handleRelocalizeToggle}
          title={relocalizeMode ? '退出重定位' : '重定位'}
          type="button"
        >
          <span className="ButtonIcon">◎</span>
          <span className="ButtonLabel">重定位</span>
        </button>
        <button
          className={`SettingsButton ${navigationMode === 'single' ? 'active' : ''}`}
          onClick={() => handleNavigationToggle('single')}
          title={navigationMode === 'single' ? '退出单点导航' : '单点导航'}
          type="button"
        >
          <span className="ButtonIcon">📍</span>
          <span className="ButtonLabel">单点</span>
        </button>
        <button
          className={`SettingsButton ${navigationMode === 'multi' ? 'active' : ''}`}
          onClick={() => handleNavigationToggle('multi')}
          title={navigationMode === 'multi' ? '退出多点导航' : '多点导航'}
          type="button"
        >
          <span className="ButtonIcon">🗺</span>
          <span className="ButtonLabel">多点</span>
        </button>
      </div>
      {relocalizeMode && (
        <div ref={relocalizeControlsRef} className="RelocalizeControls" style={relocalizeControlsStyle}>
          <button
            className="RelocalizeButton ConfirmButton"
            onClick={handleRelocalizeConfirm}
            type="button"
          >
            确定
          </button>
          <button
            className="RelocalizeButton CancelButton"
            onClick={handleRelocalizeCancel}
            type="button"
          >
            取消
          </button>
        </div>
      )}
      {manualControlMode && (
        <ManualControlPanel
          layerConfigs={layerConfigs}
          cmdVelTopic={cmdVelTopicRef.current}
          connection={connection}
          activeKeysRef={activeKeysRef}
        />
      )}
      {navigationMode && (
        <NavigationPanel
          navigationMode={navigationMode}
          navigationPoints={navigationPoints}
          onClose={handleNavigationClose}
          onClearPoints={handleClearNavigationPoints}
          onRemovePoint={handleRemoveNavigationPoint}
          onNavigate={handleNavigate}
          connection={connection}
        />
      )}
      <DeviceInfoPanel
        connection={connection}
        manualControlMode={manualControlMode}
        currentGoal={
          navigationPoints.length > 0
            ? {
                x: navigationPoints[0]!.x,
                y: navigationPoints[0]!.y,
              }
            : null
        }
        onStartNavigation={() => {
          if (navigationPoints.length > 0) {
            handleNavigate(navigationPoints);
          } else {
            toast.info('请先设置导航目标');
          }
        }}
        onStop={handleStop}
        onEmergencyStop={handleEmergencyStop}
        onRelocalize={() => {
          setRelocalizeMode(true);
        }}
      />
      <TaskManagementPanel
        connection={connection}
        navigationPoints={navigationPoints}
        onRemoveTask={handleRemoveNavigationPoint}
        onReorderTasks={(fromIndex, toIndex) => {
          setNavigationPoints((prev) => {
            const updated = [...prev];
            const [moved] = updated.splice(fromIndex, 1);
            updated.splice(toIndex, 0, moved!);
            return updated;
          });
        }}
      />
      <SystemLogPanel connection={connection} />
      <div className="BottomControls">
        <button
          className={`FocusRobotButton ${focusRobot ? 'active' : ''}`}
          onClick={handleFocusRobotToggle}
          title={focusRobot ? '取消跟随机器人' : '跟随机器人'}
          type="button"
        >
          {focusRobot ? '跟随中' : '跟随机器人'}
        </button>
      </div>
      {showSettings && (
        <LayerSettingsPanel
          layerConfigs={layerConfigs}
          onConfigChange={handleConfigChange}
          onResetToDefaults={() => {
            setLayerConfigs(DEFAULT_LAYER_CONFIGS);
            saveLayerConfigs(DEFAULT_LAYER_CONFIGS);
          }}
          onClose={() => setShowSettings(false)}
          onDeleteLayer={(layerId) => {
            setLayerConfigs((prev) => {
              const updated = { ...prev };
              delete updated[layerId];
              saveLayerConfigs(updated);
              return updated;
            });
            imagePositionsRef.current.delete(layerId);
            const positionsMap: ImagePositionsMap = {};
            imagePositionsRef.current.forEach((pos, id) => {
              positionsMap[id] = pos;
            });
            saveImagePositions(positionsMap);
          }}
          onUrdfConfigChange={async () => {
            const robotLayer = layerManagerRef.current?.getLayer('robot');
            if (robotLayer && 'reloadUrdf' in robotLayer) {
              try {
                await (robotLayer as any).reloadUrdf();
              } catch (error) {
                console.error('[MapView] Failed to reload URDF:', error);
                toast.error('加载 URDF 模型失败: ' + (error instanceof Error ? error.message : '未知错误'));
              }
            }
          }}
        />
      )}
      {showEditor && (
        <MapEditor
          connection={connection}
          onClose={() => setShowEditor(false)}
        />
      )}
      <TopoPointInfoPanel
        selectedPoint={selectedTopoPoint}
        selectedRoute={selectedTopoRoute}
        onClose={() => {
          setSelectedTopoPoint(null);
          setSelectedTopoRoute(null);
          const topoLayer = layerManagerRef.current?.getLayer('topology');
          if (topoLayer && 'setSelectedPoint' in topoLayer) {
            (topoLayer as any).setSelectedPoint(null);
          }
          if (topoLayer && 'setSelectedRoute' in topoLayer) {
            (topoLayer as any).setSelectedRoute(null);
          }
        }}
        connection={connection}
      />
      <canvas 
        ref={canvasRef} 
        className="MapCanvas" 
        tabIndex={0}
        style={{ outline: 'none' }}
      />
      {Array.from(imageLayers.entries())
        .filter(([layerId]) => layerConfigs[layerId]?.enabled)
        .map(([layerId, imageData]) => {
          const config = layerConfigs[layerId];
          const position = imagePositionsRef.current.get(layerId) || { x: 100, y: 100, scale: 1 };
          return (
            <ImageDisplay
              key={layerId}
              imageData={imageData}
              name={config?.name || layerId}
              position={position}
              onPositionChange={(newPos) => {
                imagePositionsRef.current.set(layerId, newPos);
                const positionsMap: ImagePositionsMap = {};
                imagePositionsRef.current.forEach((pos, id) => {
                  positionsMap[id] = pos;
                });
                saveImagePositions(positionsMap);
              }}
            />
          );
        })}
    </div>
  );
}
