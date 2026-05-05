# 快速迁移指南

## 概述

本文档提供标准化的节点迁移步骤，帮助快速完成剩余硬编码参数的迁移工作。

## 标准迁移步骤

### 步骤1：更新源文件头文件包含

```cpp
// 在 .cpp 文件顶部添加
#include "rm_sentry_decision/parameter_manager.hpp"
```

### 步骤2：更新头文件

```cpp
// 在 .hpp 文件中添加
#include <memory>

namespace rm_sentry_decision {

// 前向声明
class ParameterManager;

class YourNode : public BT::SomeNode {
private:
    // 添加参数管理器成员
    std::shared_ptr<ParameterManager> param_manager_;
    
    // 其他现有成员...
};
```

### 步骤3：初始化参数管理器

```cpp
// 在构造函数中添加
YourNode::YourNode(const std::string &name, const BT::NodeConfiguration &config)
    : BT::SomeNode(name, config) {
    
    node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }

    // 初始化参数管理器
    param_manager_ = ParameterManager::getInstance(node_);
    
    // 其他初始化代码...
}
```

### 步骤4：替换硬编码参数

```cpp
// 原代码
if (current_hp_.load() < 200) { ... }

// 迁移后
int low_hp = param_manager_->getHpThreshold("medium");
if (current_hp_.load() < low_hp) { ... }
```

### 步骤5：添加调试日志

```cpp
// 添加调试日志
RCLCPP_DEBUG(node_->get_logger(), "YourNode: condition met - HP=%d<%d", 
             current_hp_.load(), low_hp);
```

## 参数映射表

### 常用参数映射

| 硬编码值 | 参数路径 | 说明 |
|---------|---------|------|
| 100 | `supply_decision.hp_critical` | 补给决策危险血量 |
| 130 | `supply_decision.hp_low` | 补给决策低血量 |
| 180 | `supply_decision.hp_medium` | 补给决策中等血量 |
| 200 | `hp_thresholds.medium` | 中等血量阈值 |
| 300 | `hp_thresholds.recovery` | 恢复血量阈值 |
| 50 | `ammo_thresholds.critical` | 危险弹药阈值 |
| 150 | `hp_thresholds.low` | 低血量阈值 |
| 1500 | `base_thresholds.low` | 基地低血量阈值 |
| 2500 | `base_thresholds.critical` | 基地危险血量阈值 |
| 500 | `base_thresholds.rush_diff` | 冲击基地血量差阈值 |
| 100000 | `hp_diff_thresholds.max` | 最大血量差阈值 |
| -10000 | `hp_diff_thresholds.enemy_dead` | 敌人阵亡时的差值 |

### 参数获取方法

```cpp
// 血量相关
int critical_hp = param_manager_->getHpThreshold("critical");
int low_hp = param_manager_->getHpThreshold("low");
int medium_hp = param_manager_->getHpThreshold("medium");
int recovery_hp = param_manager_->getHpThreshold("recovery");
int max_hp = param_manager_->getHpThreshold("max");

// 弹药相关
int critical_ammo = param_manager_->getAmmoThreshold("critical");
int low_ammo = param_manager_->getAmmoThreshold("low");
int medium_ammo = param_manager_->getAmmoThreshold("medium");
int recovery_ammo = param_manager_->getAmmoThreshold("recovery");

// 时间相关
int endgame_time = param_manager_->getTimeThreshold("endgame");
int critical_time = param_manager_->getTimeThreshold("critical");
int cooldown_time = param_manager_->getTimeThreshold("supply_cooldown");

// 基地相关
int base_low = param_manager_->getBaseThreshold("low");
int base_critical = param_manager_->getBaseThreshold("critical");
int rush_diff = param_manager_->getBaseThreshold("rush_diff");

// 血量差相关
int diff_min = param_manager_->getHpDiffThreshold("min");
int diff_medium = param_manager_->getHpDiffThreshold("medium");
int diff_max = param_manager_->getHpDiffThreshold("max");
int enemy_dead = param_manager_->getHpDiffThreshold("enemy_dead");

// 补给决策相关
int supply_hp_critical = param_manager_->getSupplyDecisionParam<int>("hp_critical");
int supply_hp_low = param_manager_->getSupplyDecisionParam<int>("hp_low");
int supply_hp_medium = param_manager_->getSupplyDecisionParam<int>("hp_medium");
int supply_diff_small = param_manager_->getSupplyDecisionParam<int>("diff_small");
int supply_diff_medium = param_manager_->getSupplyDecisionParam<int>("diff_medium");
int supply_diff_large = param_manager_->getSupplyDecisionParam<int>("diff_large");
```

## 剩余节点迁移清单

### 高优先级 (立即完成)

#### CompareHpDiff
```cpp
// 需要替换的硬编码参数
if(selfHP >= 200) { return BT::NodeStatus::FAILURE; }
bool upper_ok = (max_diff < 0) ? true : (diff <= max_diff);

// 迁移后
int max_self_hp = param_manager_->getHpThreshold("max");
int max_diff_threshold = param_manager_->getHpDiffThreshold("max");

if(selfHP >= max_self_hp) { return BT::NodeStatus::FAILURE; }
bool upper_ok = (max_diff_threshold < 0) ? true : (diff <= max_diff_threshold);
```

### 中优先级 (本周完成)

#### RandomPatrolPose
```cpp
// 需要替换的硬编码参数
if (own_base_hp_.load() < 1500) { ... }

// 迁移后
int base_low = param_manager_->getBaseThreshold("low");
if (own_base_hp_.load() < base_low) { ... }
```

#### HeroProtectAttack
```cpp
// 需要替换的硬编码参数
if (home_base_hp_.load() < 2500) return BT::NodeStatus::FAILURE;

// 迁移后
int base_critical = param_manager_->getBaseThreshold("critical");
if (home_base_hp_.load() < base_critical) return BT::NodeStatus::FAILURE;
```

### 低优先级 (下周完成)

#### SelfHpBelow
```cpp
// 这个节点已经使用输入参数，无需迁移
// 但可以添加参数验证
int threshold = 100;
getInput("threshold", threshold);

// 可以添加范围验证
int max_hp = param_manager_->getHpThreshold("max");
if (threshold > max_hp) {
    RCLCPP_WARN(node_->get_logger(), "Threshold %d exceeds max HP %d", threshold, max_hp);
    threshold = max_hp;
}
```

## 测试检查清单

### 编译测试
- [ ] 添加必要的头文件包含
- [ ] 添加前向声明
- [ ] 添加参数管理器成员变量
- [ ] 在构造函数中初始化参数管理器
- [ ] 替换所有硬编码参数
- [ ] 编译无错误

### 功能测试
- [ ] 参数读取正确
- [ ] 决策逻辑不变
- [ ] 调试日志输出正常
- [ ] 边界条件处理正确
- [ ] 错误处理正常

### 集成测试
- [ ] 与其他节点配合正常
- [ ] 参数管理器单例模式正常
- [ ] 配置文件加载正常
- [ ] 性能无显著影响

## 常见问题解决

### 问题1：编译错误 - 找不到ParameterManager
**解决方案**: 确保添加了正确的头文件包含和前向声明

### 问题2：运行时错误 - 参数管理器为空
**解决方案**: 确保在构造函数中正确初始化参数管理器

### 问题3：参数读取错误 - 参数不存在
**解决方案**: 检查参数名称是否正确，确保配置文件包含该参数

### 问题4：性能问题 - 频繁读取参数
**解决方案**: 考虑在构造函数中一次性读取所有需要的参数

## 质量检查

### 代码质量
- [ ] 遵循现有代码风格
- [ ] 变量命名有意义
- [ ] 添加适当注释
- [ ] 添加调试日志
- [ ] 错误处理完善

### 功能完整性
- [ ] 所有硬编码参数已替换
- [ ] 决策逻辑保持不变
- [ ] 参数范围合理
- [ ] 边界条件处理正确

### 文档更新
- [ ] 更新头文件注释
- [ ] 更新实现文件注释
- [ ] 更新迁移进度文档
- [ ] 记录使用的参数

## 完成标准

一个节点的迁移被认为完成，需要满足以下条件：

1. **编译通过**: 无编译错误和警告
2. **功能正常**: 基本功能测试通过
3. **参数化完成**: 所有硬编码参数已替换
4. **文档更新**: 相关文档已更新
5. **测试通过**: 单元测试和集成测试通过

## 总结

使用这个标准化指南，可以快速完成剩余节点的迁移工作。每个节点的迁移大约需要30-60分钟，包括代码修改、测试和文档更新。

建议按照优先级顺序进行迁移，确保高优先级节点先完成，然后再处理中低优先级节点。 