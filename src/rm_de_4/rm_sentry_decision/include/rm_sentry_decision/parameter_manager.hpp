#ifndef RM_SENTRY_DECISION_PARAMETER_MANAGER_HPP
#define RM_SENTRY_DECISION_PARAMETER_MANAGER_HPP

#include <rclcpp/rclcpp.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace rm_sentry_decision {

/**
 * @brief 参数管理器类，用于管理所有决策系统的参数
 * 
 * 这个类负责从YAML配置文件中读取参数，并提供统一的参数访问接口。
 * 所有硬编码的阈值都应该通过这个类来管理。
 */
class ParameterManager {
public:
    /**
     * @brief 构造函数
     * @param node ROS2节点指针
     * @param config_file 配置文件路径
     */
    ParameterManager(rclcpp::Node::SharedPtr node, const std::string& config_file = "");

    /**
     * @brief 获取血量阈值
     * @param threshold_name 阈值名称 (critical, low, medium, recovery, max)
     * @return 血量阈值
     */
    int getHpThreshold(const std::string& threshold_name) const;

    /**
     * @brief 获取弹药阈值
     * @param threshold_name 阈值名称 (critical, low, medium, recovery, max)
     * @return 弹药阈值
     */
    int getAmmoThreshold(const std::string& threshold_name) const;

    /**
     * @brief 获取时间阈值
     * @param threshold_name 阈值名称 (endgame, critical, supply_cooldown)
     * @return 时间阈值
     */
    int getTimeThreshold(const std::string& threshold_name) const;

    /**
     * @brief 获取基地血量阈值
     * @param threshold_name 阈值名称 (low, critical, rush_diff)
     * @return 基地血量阈值
     */
    int getBaseThreshold(const std::string& threshold_name) const;

    /**
     * @brief 获取敌我血量差阈值
     * @param threshold_name 阈值名称 (min, medium, max, enemy_dead)
     * @return 血量差阈值
     */
    int getHpDiffThreshold(const std::string& threshold_name) const;

    /**
     * @brief 获取机器人状态参数
     * @param param_name 参数名称 (min_hp, min_bullets, status_ok)
     * @return 参数值
     */
    template<typename T>
    T getRobotStatusParam(const std::string& param_name) const;

    /**
     * @brief 获取补给决策参数
     * @param param_name 参数名称
     * @return 参数值
     */
    template<typename T>
    T getSupplyDecisionParam(const std::string& param_name) const;

    /**
     * @brief 获取增益点守卫参数
     * @param param_name 参数名称 (duration, pose)
     * @return 参数值
     */
    template<typename T>
    T getBuffGuardParam(const std::string& param_name) const;

    /**
     * @brief 获取导航参数
     * @param param_name 参数名称 (server_timeout, action_name)
     * @return 参数值
     */
    template<typename T>
    T getNavigationParam(const std::string& param_name) const;

    /**
     * @brief 获取日志参数
     * @param param_name 参数名称 (info_interval, warn_interval, debug_interval)
     * @return 参数值
     */
    int getLoggingParam(const std::string& param_name) const;

    /**
     * @brief 获取决策参数
     * @param param_name 参数名称 (frequency, cycle_time)
     * @return 参数值
     */
    int getDecisionParam(const std::string& param_name) const;

    /**
     * @brief 重新加载配置文件
     * @param config_file 新的配置文件路径
     * @return 是否成功加载
     */
    bool reloadConfig(const std::string& config_file = "");

    /**
     * @brief 获取参数管理器实例（单例模式）
     * @param node ROS2节点指针
     * @return 参数管理器实例
     */
    static std::shared_ptr<ParameterManager> getInstance(rclcpp::Node::SharedPtr node = nullptr);

private:
    rclcpp::Node::SharedPtr node_;
    YAML::Node config_;
    std::string config_file_;
    static std::shared_ptr<ParameterManager> instance_;

    /**
     * @brief 加载配置文件
     * @param config_file 配置文件路径
     * @return 是否成功加载
     */
    bool loadConfig(const std::string& config_file);

    /**
     * @brief 验证配置文件的完整性
     * @return 是否验证通过
     */
    bool validateConfig() const;

    /**
     * @brief 获取嵌套的YAML节点值
     * @param node YAML节点
     * @param key 键名
     * @param default_value 默认值
     * @return 节点值
     */
    template<typename T>
    T getNestedValue(const YAML::Node& node, const std::string& key, const T& default_value) const;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION_PARAMETER_MANAGER_HPP 