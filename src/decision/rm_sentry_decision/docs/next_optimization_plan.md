# 后续优化计划

## 概述

基于当前的参数管理系统，制定详细的后续优化计划，包括节点迁移、参数扩展、场景配置和自动调优。

## 第一阶段：逐步迁移其他节点

### 1.1 优先级排序

根据硬编码参数的数量和重要性，按以下优先级迁移：

#### 高优先级 (立即迁移)
1. **SelectSupplyPoint** - 补给点选择节点
   - 硬编码参数：100, 130, 180, 50, 60, 90, -10000
   - 影响：核心补给决策逻辑
   - 预计工作量：2小时

2. **RecoveredEnough** - 恢复足够判断节点
   - 硬编码参数：300, 300
   - 影响：补给完成判断
   - 预计工作量：1小时

3. **CompareHpDiff** - 血量差比较节点
   - 硬编码参数：200, 100000
   - 影响：敌我血量比较
   - 预计工作量：1小时

#### 中优先级 (第二周迁移)
4. **RandomPatrolPose** - 随机巡逻点选择
   - 硬编码参数：1500
   - 影响：巡逻策略
   - 预计工作量：1小时

5. **HeroProtectAttack** - 英雄保护攻击
   - 硬编码参数：2500
   - 影响：保护策略
   - 预计工作量：1小时

#### 低优先级 (第三周迁移)
6. **SelfHpBelow** - 自身血量判断
   - 硬编码参数：无(使用输入参数)
   - 影响：基础判断节点
   - 预计工作量：0.5小时

### 1.2 迁移实施步骤

#### 步骤1：SelectSupplyPoint 迁移
```cpp
// 当前硬编码
if (self_hp <= 100) { ... }
bool both_ge50 = (diff3 >= 50) && (diff4 >= 50);
bool any_between60_90 = (diff3 >= 60 && diff3 <= 90) || (diff4 >= 60 && diff4 <= 90);
bool both_ge90 = (diff3 >= 90) && (diff4 >= 90);
if (self_hp >= 130 && self_hp <= 180) { ... }
if (self_hp > 100 && self_hp < 130) { ... }

// 迁移后
int hp_critical = param_manager_->getSupplyDecisionParam<int>("hp_critical");
int hp_low = param_manager_->getSupplyDecisionParam<int>("hp_low");
int hp_medium = param_manager_->getSupplyDecisionParam<int>("hp_medium");
int diff_small = param_manager_->getSupplyDecisionParam<int>("diff_small");
int diff_medium = param_manager_->getSupplyDecisionParam<int>("diff_medium");
int diff_large = param_manager_->getSupplyDecisionParam<int>("diff_large");
int enemy_dead = param_manager_->getHpDiffThreshold("enemy_dead");

if (self_hp <= hp_critical) { ... }
bool both_ge_small = (diff3 >= diff_small) && (diff4 >= diff_small);
bool any_between_medium_large = (diff3 >= diff_medium && diff3 <= diff_large) || 
                                (diff4 >= diff_medium && diff4 <= diff_large);
bool both_ge_large = (diff3 >= diff_large) && (diff4 >= diff_large);
if (self_hp >= hp_low && self_hp <= hp_medium) { ... }
if (self_hp > hp_critical && self_hp < hp_low) { ... }
```

#### 步骤2：RecoveredEnough 迁移
```cpp
// 当前硬编码
if (current_hp_.load() >= 300 && remaining_ammo_.load() >= 300) { ... }

// 迁移后
int recovery_hp = param_manager_->getHpThreshold("recovery");
int recovery_ammo = param_manager_->getAmmoThreshold("recovery");

if (current_hp_.load() >= recovery_hp && remaining_ammo_.load() >= recovery_ammo) { ... }
```

#### 步骤3：CompareHpDiff 迁移
```cpp
// 当前硬编码
if(selfHP >= 200) { return BT::NodeStatus::FAILURE; }
bool upper_ok = (max_diff < 0) ? true : (diff <= max_diff);

// 迁移后
int max_self_hp = param_manager_->getHpThreshold("max");
int max_diff_threshold = param_manager_->getHpDiffThreshold("max");

if(selfHP >= max_self_hp) { return BT::NodeStatus::FAILURE; }
bool upper_ok = (max_diff_threshold < 0) ? true : (diff <= max_diff_threshold);
```

## 第二阶段：添加更多参数

### 2.1 扩展参数配置文件

#### 新增参数类别
```yaml
# 战术参数
tactics:
  attack_aggression: 0.7      # 攻击激进程度 (0.0-1.0)
  defense_conservatism: 0.6   # 防守保守程度 (0.0-1.0)
  risk_tolerance: 0.5         # 风险容忍度 (0.0-1.0)
  
# 位置参数
positions:
  outpost_attack: "10; 2; 0; 0; 0; 0; 1"
  base_defense: "-1; 2; 0; 0; 0; 0; 1"
  buff_guard: "1; -0.5; 0; 0; 0; 0; 1"
  patrol_points:
    - "2; 2; 0; 0; 0; 0; 1"
    - "3; 1; 0; 0; 0; 0; 1"
    - "1; 3; 0; 0; 0; 0; 1"
  
# 时间窗口参数
time_windows:
  early_game: 300     # 前期时间窗口(秒)
  mid_game: 180       # 中期时间窗口(秒)
  late_game: 60       # 后期时间窗口(秒)
  
# 性能参数
performance:
  max_decision_time: 10   # 最大决策时间(毫秒)
  max_navigation_time: 30 # 最大导航时间(秒)
  retry_attempts: 3       # 重试次数
  
# 通信参数
communication:
  topic_queue_size: 10    # 话题队列大小
  service_timeout: 5      # 服务超时时间(秒)
  action_timeout: 30      # 动作超时时间(秒)
```

### 2.2 新增参数管理器方法

```cpp
// 在 parameter_manager.hpp 中添加
int getTacticsParam(const std::string& param_name) const;
std::string getPositionParam(const std::string& param_name) const;
std::vector<std::string> getPatrolPoints() const;
int getTimeWindowParam(const std::string& param_name) const;
int getPerformanceParam(const std::string& param_name) const;
int getCommunicationParam(const std::string& param_name) const;
```

## 第三阶段：创建场景配置

### 3.1 场景配置文件结构

```
config/
├── parameters.yaml              # 默认配置
├── scenarios/
│   ├── aggressive.yaml         # 激进策略配置
│   ├── conservative.yaml       # 保守策略配置
│   ├── balanced.yaml           # 平衡策略配置
│   ├── endgame.yaml            # 终局策略配置
│   └── emergency.yaml          # 紧急情况配置
└── environments/
    ├── indoor.yaml             # 室内环境配置
    ├── outdoor.yaml            # 室外环境配置
    └── competition.yaml        # 比赛环境配置
```

### 3.2 激进策略配置示例

```yaml
# config/scenarios/aggressive.yaml
hp_thresholds:
  critical: 60       # 更低的危险阈值
  low: 100           # 更低的低血量阈值
  medium: 150        # 更低的中等血量阈值
  recovery: 200      # 更低的恢复阈值

ammo_thresholds:
  critical: 30       # 更低的危险弹药阈值
  low: 60            # 更低的低弹药阈值
  medium: 120        # 更低的中等弹药阈值
  recovery: 200      # 更低的恢复弹药阈值

tactics:
  attack_aggression: 0.9      # 高攻击激进程度
  defense_conservatism: 0.3   # 低防守保守程度
  risk_tolerance: 0.8         # 高风险容忍度

time_thresholds:
  endgame: 120       # 更早进入终局
  critical: 48       # 更早进入关键时间
  supply_cooldown: 6 # 更短的补给冷却
```

### 3.3 保守策略配置示例

```yaml
# config/scenarios/conservative.yaml
hp_thresholds:
  critical: 120      # 更高的危险阈值
  low: 180           # 更高的低血量阈值
  medium: 240        # 更高的中等血量阈值
  recovery: 360      # 更高的恢复阈值

ammo_thresholds:
  critical: 60       # 更高的危险弹药阈值
  low: 120           # 更高的低弹药阈值
  medium: 240        # 更高的中等弹药阈值
  recovery: 360      # 更高的恢复弹药阈值

tactics:
  attack_aggression: 0.4      # 低攻击激进程度
  defense_conservatism: 0.8   # 高防守保守程度
  risk_tolerance: 0.2         # 低风险容忍度

time_thresholds:
  endgame: 180       # 更晚进入终局
  critical: 72       # 更晚进入关键时间
  supply_cooldown: 12 # 更长的补给冷却
```

### 3.4 场景切换机制

```cpp
// 在 parameter_manager.hpp 中添加
class ParameterManager {
public:
    // 场景切换方法
    bool switchScenario(const std::string& scenario_name);
    bool switchEnvironment(const std::string& environment_name);
    
    // 获取当前场景信息
    std::string getCurrentScenario() const;
    std::string getCurrentEnvironment() const;
    
    // 场景列表
    std::vector<std::string> getAvailableScenarios() const;
    std::vector<std::string> getAvailableEnvironments() const;
    
private:
    std::string current_scenario_;
    std::string current_environment_;
    std::string scenarios_dir_;
    std::string environments_dir_;
};
```

## 第四阶段：参数自动调优

### 4.1 性能指标定义

```cpp
// 定义性能指标结构
struct PerformanceMetrics {
    double win_rate;           // 胜率
    double survival_time;      // 生存时间
    double damage_dealt;       // 造成伤害
    double damage_taken;       // 承受伤害
    double supply_efficiency;  // 补给效率
    double navigation_success; // 导航成功率
    double decision_speed;     // 决策速度
    double resource_usage;     // 资源使用率
};
```

### 4.2 自动调优算法

#### 遗传算法实现
```cpp
class ParameterOptimizer {
public:
    struct ParameterSet {
        std::map<std::string, double> parameters;
        double fitness;
    };
    
    // 初始化种群
    void initializePopulation(size_t population_size);
    
    // 评估适应度
    double evaluateFitness(const ParameterSet& params);
    
    // 选择操作
    std::vector<ParameterSet> selection(const std::vector<ParameterSet>& population);
    
    // 交叉操作
    ParameterSet crossover(const ParameterSet& parent1, const ParameterSet& parent2);
    
    // 变异操作
    void mutate(ParameterSet& params, double mutation_rate);
    
    // 主优化循环
    ParameterSet optimize(size_t generations, size_t population_size);
    
private:
    std::vector<ParameterSet> population_;
    std::shared_ptr<ParameterManager> param_manager_;
    std::function<double(const ParameterSet&)> fitness_function_;
};
```

### 4.3 在线学习机制

```cpp
class OnlineParameterLearner {
public:
    // 记录性能数据
    void recordPerformance(const PerformanceMetrics& metrics);
    
    // 更新参数
    void updateParameters(const std::vector<double>& gradients);
    
    // 学习率调整
    void adjustLearningRate(double performance_change);
    
    // 参数约束检查
    bool validateParameters(const std::map<std::string, double>& params);
    
private:
    std::deque<PerformanceMetrics> performance_history_;
    std::map<std::string, double> learning_rates_;
    double current_performance_;
    size_t update_frequency_;
};
```

## 第五阶段：监控和分析系统

### 5.1 参数使用监控

```cpp
class ParameterMonitor {
public:
    // 记录参数使用
    void recordParameterUsage(const std::string& param_name, double value);
    
    // 生成使用报告
    std::string generateUsageReport() const;
    
    // 检测异常参数
    std::vector<std::string> detectAnomalies() const;
    
    // 参数影响分析
    std::map<std::string, double> analyzeParameterImpact() const;
    
private:
    std::map<std::string, std::vector<double>> parameter_history_;
    std::map<std::string, std::vector<PerformanceMetrics>> performance_history_;
    std::chrono::steady_clock::time_point start_time_;
};
```

### 5.2 可视化界面

```cpp
// 创建参数监控ROS节点
class ParameterMonitorNode : public rclcpp::Node {
public:
    ParameterMonitorNode();
    
private:
    // 发布参数状态
    void publishParameterStatus();
    
    // 发布性能指标
    void publishPerformanceMetrics();
    
    // 参数调整服务
    void handleParameterAdjustment(const ParameterAdjustmentRequest& request,
                                  ParameterAdjustmentResponse& response);
    
    // 定时器回调
    void timerCallback();
    
    std::shared_ptr<ParameterManager> param_manager_;
    std::shared_ptr<ParameterMonitor> monitor_;
    rclcpp::Publisher<ParameterStatus>::SharedPtr status_pub_;
    rclcpp::Publisher<PerformanceMetrics>::SharedPtr metrics_pub_;
    rclcpp::Service<ParameterAdjustment>::SharedPtr adjustment_service_;
    rclcpp::TimerBase::SharedPtr timer_;
};
```

## 实施时间表

### 第一周：节点迁移
- [ ] Day 1-2: SelectSupplyPoint 迁移
- [ ] Day 3: RecoveredEnough 迁移
- [ ] Day 4: CompareHpDiff 迁移
- [ ] Day 5: 测试和调试

### 第二周：参数扩展
- [ ] Day 1-2: 扩展参数配置文件
- [ ] Day 3-4: 实现新的参数管理器方法
- [ ] Day 5: 更新现有节点使用新参数

### 第三周：场景配置
- [ ] Day 1-2: 创建场景配置文件
- [ ] Day 3-4: 实现场景切换机制
- [ ] Day 5: 测试场景切换功能

### 第四周：自动调优
- [ ] Day 1-2: 实现性能指标收集
- [ ] Day 3-4: 实现遗传算法优化
- [ ] Day 5: 测试自动调优功能

### 第五周：监控系统
- [ ] Day 1-2: 实现参数监控
- [ ] Day 3-4: 实现可视化界面
- [ ] Day 5: 系统集成测试

## 风险评估和缓解措施

### 风险1：参数调优过度拟合
- **缓解措施**: 使用交叉验证，限制参数范围
- **监控指标**: 泛化性能，过拟合检测

### 风险2：自动调优计算开销
- **缓解措施**: 异步优化，限制优化频率
- **监控指标**: CPU使用率，优化时间

### 风险3：场景切换影响稳定性
- **缓解措施**: 平滑过渡，参数验证
- **监控指标**: 系统稳定性，性能变化

## 成功标准

1. **功能完整性**: 所有硬编码参数迁移完成
2. **性能提升**: 系统性能提升10%以上
3. **可维护性**: 参数调整时间减少80%
4. **自动化程度**: 80%的参数可以自动调优
5. **监控覆盖**: 100%的关键参数有监控

## 总结

这个后续优化计划将机器人决策系统提升到一个新的水平：

1. **完全参数化**: 消除所有硬编码参数
2. **场景适应**: 支持不同比赛场景的快速切换
3. **智能优化**: 基于性能的自动参数调优
4. **全面监控**: 实时监控和性能分析

通过这些优化，系统将具备更强的适应性和竞争力，能够在不同比赛环境中快速调整策略，实现最佳性能。 