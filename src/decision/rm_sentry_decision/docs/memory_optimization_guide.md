# 内存管理优化指南

## 问题描述

在原有的实现中，每个行为树节点都创建了多个ROS订阅，导致内存使用过高：

```cpp
// 每个节点都有多个订阅
status_sub_ = node_->create_subscription<...>(...);
bullet_sub_ = node_->create_subscription<...>(...);
hp_sub_ = node_->create_subscription<...>(...);
```

## 解决方案

实现了一个共享的 `DataManager` 类，采用单例模式集中管理所有ROS话题的订阅和数据缓存。

### 核心特性

1. **单例模式**: 全局唯一的数据管理器实例
2. **线程安全**: 使用 `std::atomic` 保证数据访问的线程安全
3. **数据新鲜度检查**: 提供数据时效性验证
4. **便捷访问方法**: 提供简化的数据访问接口

## 使用方法

### 1. 在节点构造函数中获取DataManager

```cpp
// 旧方式
status_sub_ = node_->create_subscription<...>(...);
bullet_sub_ = node_->create_subscription<...>(...);

// 新方式
data_manager_ = &DataManager::getInstance(node_);
```

### 2. 在tick()方法中使用数据

```cpp
// 旧方式
const uint16_t current_hp = current_hp_.load();
const uint16_t bullets = bullets_remaining_.load();

// 新方式
const uint16_t current_hp = data_manager_->current_hp_.load();
const uint16_t bullets = data_manager_->bullets_remaining_.load();
```

### 3. 数据新鲜度检查

```cpp
// 检查数据是否在2秒内更新过
if (!data_manager_->isDataFresh(2000)) {
    RCLCPP_WARN(node_->get_logger(), "Data is stale");
    return BT::NodeStatus::FAILURE;
}
```

## 迁移步骤

### 步骤1: 修改头文件

```cpp
// 添加包含
#include "rm_sentry_decision/data_manager.hpp"

// 移除订阅器成员变量
// rclcpp::Subscription<...>::SharedPtr status_sub_;

// 添加数据管理器指针
DataManager* data_manager_{nullptr};
```

### 步骤2: 修改构造函数

```cpp
// 移除订阅创建代码
// status_sub_ = node_->create_subscription<...>(...);

// 添加数据管理器获取
data_manager_ = &DataManager::getInstance(node_);
```

### 步骤3: 修改tick()方法

```cpp
// 添加数据管理器检查
if (!data_manager_ || !data_manager_->isInitialized()) {
    RCLCPP_ERROR(node_->get_logger(), "DataManager not initialized");
    return BT::NodeStatus::FAILURE;
}

// 替换数据访问
// 旧: current_hp_.load()
// 新: data_manager_->current_hp_.load()
```

## 可用的数据字段

### 游戏状态数据
- `current_hp_`: 当前血量
- `stage_remain_time_`: 剩余时间
- `game_progress_`: 游戏进度
- `team_color_`: 队伍颜色

### 机器人状态数据
- `bullets_remaining_`: 17mm弹量
- `shooter_heat_`: 发射器热量

### 全局血量数据
- `red_base_hp_`, `blue_base_hp_`: 基地血量
- `red_outpost_hp_`, `blue_outpost_hp_`: 前哨站血量
- `red_1_robot_hp_`, `blue_1_robot_hp_`: 机器人1血量
- `red_3_robot_hp_`, `blue_3_robot_hp_`: 机器人3血量
- `red_4_robot_hp_`, `blue_4_robot_hp_`: 机器人4血量

## 便捷访问方法

```cpp
// 获取己方/敌方基地血量
uint16_t own_hp = data_manager_->getOwnBaseHp(is_blue);
uint16_t enemy_hp = data_manager_->getEnemyBaseHp(is_blue);

// 获取己方/敌方机器人血量
uint16_t robot3_hp = data_manager_->getOwnRobotHp(is_blue, 3);
uint16_t enemy3_hp = data_manager_->getEnemyRobotHp(is_blue, 3);
```

## 性能优化效果

### 内存使用减少
- **订阅数量**: 从 N×4 减少到 4 (N为节点数量)
- **内存占用**: 预计减少 60-80%

### 系统稳定性提升
- **数据一致性**: 所有节点使用相同的数据源
- **资源竞争**: 减少ROS话题的并发访问
- **错误处理**: 统一的数据新鲜度检查

## 注意事项

1. **初始化顺序**: 确保DataManager在使用前已初始化
2. **数据新鲜度**: 根据应用需求设置合适的数据新鲜度阈值
3. **错误处理**: 添加适当的错误处理逻辑
4. **向后兼容**: 可以逐步迁移，新旧实现并存

## 示例代码

参考以下优化后的节点实现：
- `robot_status_sub_optimized.hpp/cpp`
- `need_supply_optimized.hpp/cpp`

这些示例展示了完整的迁移过程和最佳实践。 