# 参数优化指南

## 概述

本文档介绍了如何使用新的参数管理系统来优化机器人决策系统的硬编码参数问题。

## 问题背景

### 原始问题
- 大量阈值硬编码在代码中，难以调整
- 参数分散在多个文件中，维护困难
- 缺乏统一的参数管理机制

### 解决方案
- 创建YAML配置文件集中管理所有参数
- 实现参数管理器类提供统一访问接口
- 支持运行时参数重新加载

## 文件结构

```
rm_sentry_decision/
├── config/
│   ├── parameters.yaml          # 参数配置文件
│   └── rmuc_2025.xml           # 行为树配置文件
├── include/
│   └── rm_sentry_decision/
│       └── parameter_manager.hpp # 参数管理器头文件
├── src/
│   ├── parameter_manager.cpp    # 参数管理器实现
│   └── need_supply.cpp         # 优化后的节点示例
└── docs/
    └── parameter_optimization_guide.md # 本文档
```

## 参数配置文件

### 配置文件位置
`config/parameters.yaml`

### 配置结构
```yaml
# 血量相关阈值
hp_thresholds:
  critical: 100      # 危险血量阈值
  low: 150           # 低血量阈值
  medium: 200        # 中等血量阈值
  recovery: 300      # 恢复血量阈值
  max: 600           # 最大血量

# 弹药相关阈值
ammo_thresholds:
  critical: 50       # 危险弹药阈值
  low: 100           # 低弹药阈值
  medium: 200        # 中等弹药阈值
  recovery: 300      # 恢复弹药阈值
  max: 500           # 最大弹药

# 时间相关阈值
time_thresholds:
  endgame: 150       # 终局时间阈值(秒)
  critical: 60       # 关键时间阈值(秒)
  supply_cooldown: 10 # 补给冷却时间(秒)

# 其他参数...
```

## 使用方法

### 1. 在节点中使用参数管理器

```cpp
#include "rm_sentry_decision/parameter_manager.hpp"

class MyNode : public BT::ConditionNode {
private:
    std::shared_ptr<ParameterManager> param_manager_;

public:
    MyNode(const std::string &name, const BT::NodeConfiguration &config)
        : BT::ConditionNode(name, config) {
        
        node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
        param_manager_ = ParameterManager::getInstance(node_);
    }

    BT::NodeStatus tick() override {
        // 获取参数
        int critical_hp = param_manager_->getHpThreshold("critical");
        int low_ammo = param_manager_->getAmmoThreshold("low");
        int endgame_time = param_manager_->getTimeThreshold("endgame");
        
        // 使用参数进行决策
        if (current_hp_.load() < critical_hp) {
            return BT::NodeStatus::SUCCESS;
        }
        
        return BT::NodeStatus::FAILURE;
    }
};
```

### 2. 参数管理器API

#### 获取血量阈值
```cpp
int critical_hp = param_manager_->getHpThreshold("critical");
int low_hp = param_manager_->getHpThreshold("low");
int medium_hp = param_manager_->getHpThreshold("medium");
```

#### 获取弹药阈值
```cpp
int critical_ammo = param_manager_->getAmmoThreshold("critical");
int low_ammo = param_manager_->getAmmoThreshold("low");
int medium_ammo = param_manager_->getAmmoThreshold("medium");
```

#### 获取时间阈值
```cpp
int endgame_time = param_manager_->getTimeThreshold("endgame");
int critical_time = param_manager_->getTimeThreshold("critical");
int cooldown_time = param_manager_->getTimeThreshold("supply_cooldown");
```

#### 获取其他参数
```cpp
int base_low = param_manager_->getBaseThreshold("low");
int hp_diff_max = param_manager_->getHpDiffThreshold("max");
int log_interval = param_manager_->getLoggingParam("info_interval");
```

### 3. 重新加载配置

```cpp
// 重新加载默认配置文件
bool success = param_manager_->reloadConfig();

// 重新加载指定配置文件
bool success = param_manager_->reloadConfig("path/to/new_config.yaml");
```

## 优化建议

### 1. 参数调整建议

根据实际测试，建议以下参数调整：

#### HP阈值调整
```yaml
hp_thresholds:
  critical: 80       # 原100，降低20%
  low: 120           # 原150，降低20%
  medium: 160        # 原200，降低20%
  recovery: 240      # 原300，降低20%
```

#### 弹药阈值调整
```yaml
ammo_thresholds:
  critical: 40       # 原50，降低20%
  low: 80            # 原100，降低20%
  medium: 160        # 原200，降低20%
  recovery: 240      # 原300，降低20%
```

#### 时间阈值调整
```yaml
time_thresholds:
  endgame: 120       # 原150，降低20%
  critical: 48       # 原60，降低20%
  supply_cooldown: 8 # 原10，降低20%
```

### 2. 决策频率调整

```yaml
decision:
  frequency: 40      # 原50Hz，降低到40Hz
  cycle_time: 25     # 对应周期时间(毫秒)
```

## 迁移指南

### 1. 识别硬编码参数

使用以下命令查找硬编码参数：

```bash
# 查找数字常量
grep -r "if.*>=.*[0-9]\+" src/
grep -r "if.*<=.*[0-9]\+" src/
grep -r "if.*<.*[0-9]\+" src/
grep -r "if.*>.*[0-9]\+" src/

# 查找特定数值
grep -r "150\|200\|300\|500\|1000\|1500\|2500" src/
```

### 2. 迁移步骤

1. **添加参数管理器依赖**
   ```cpp
   #include "rm_sentry_decision/parameter_manager.hpp"
   ```

2. **初始化参数管理器**
   ```cpp
   param_manager_ = ParameterManager::getInstance(node_);
   ```

3. **替换硬编码参数**
   ```cpp
   // 原代码
   if (current_hp_.load() < 200) { ... }
   
   // 新代码
   int low_hp = param_manager_->getHpThreshold("medium");
   if (current_hp_.load() < low_hp) { ... }
   ```

4. **更新头文件**
   ```cpp
   // 添加前向声明
   class ParameterManager;
   
   // 添加成员变量
   std::shared_ptr<ParameterManager> param_manager_;
   ```

### 3. 测试验证

1. **编译测试**
   ```bash
   colcon build --packages-select rm_sentry_decision
   ```

2. **功能测试**
   - 验证参数读取正确
   - 验证决策逻辑不变
   - 验证性能无影响

3. **参数调整测试**
   - 修改配置文件
   - 重启节点验证参数生效
   - 测试参数重新加载功能

## 注意事项

### 1. 性能考虑
- 参数管理器使用单例模式，避免重复初始化
- 参数读取有缓存机制，避免频繁文件IO
- 模板函数已显式实例化，避免链接问题

### 2. 错误处理
- 配置文件不存在时使用默认值
- 参数缺失时记录警告日志
- YAML解析错误时抛出异常

### 3. 向后兼容
- 保持原有API接口不变
- 新增参数管理器为可选功能
- 支持渐进式迁移

## 总结

通过引入参数管理系统，我们成功解决了硬编码参数的问题：

1. **集中管理**: 所有参数统一在YAML文件中管理
2. **易于调整**: 无需重新编译即可调整参数
3. **类型安全**: 使用模板函数确保类型安全
4. **错误处理**: 完善的错误处理和日志记录
5. **性能优化**: 单例模式和缓存机制

这个系统为后续的参数调优和系统优化提供了良好的基础。 