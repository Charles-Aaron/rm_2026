#ifndef RM_SENTRY_DECISION__DATA_MANAGER_HPP_
#define RM_SENTRY_DECISION__DATA_MANAGER_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/all_robot_hp.hpp"
#include "rm_decision_interfaces/msg/bullet_allowance.hpp"
#include "rm_decision_interfaces/msg/game_status.hpp"
#include "rm_decision_interfaces/msg/robot_status.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>

namespace rm_sentry_decision {

/**
 * @brief 共享数据管理器 - 单例模式
 * 
 * 集中管理所有ROS话题的订阅和数据缓存，避免每个节点重复创建订阅
 * 提供线程安全的数据访问接口
 */
class DataManager {
public:
    /**
     * @brief 获取单例实例
     * @param node ROS节点指针
     * @return DataManager实例的引用
     */
    static DataManager& getInstance(rclcpp::Node::SharedPtr node = nullptr);

    /**
     * @brief 初始化数据管理器
     * @param node ROS节点指针
     */
    void initialize(rclcpp::Node::SharedPtr node);

    /**
     * @brief 检查是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const { return initialized_; }

    // ========== 游戏状态数据 ==========
    std::atomic<uint16_t> current_hp_{0};
    std::atomic<uint16_t> stage_remain_time_{9999};
    std::atomic<uint8_t> game_progress_{0};
    std::atomic<bool> team_color_{false};  // true=blue, false=red

    // ========== 机器人状态数据 ==========
    std::atomic<uint16_t> bullets_remaining_{0};  // 17mm弹量
    std::atomic<uint16_t> shooter_heat_{0};

    // ========== 全局血量数据 ==========
    // 基地血量
    std::atomic<uint16_t> red_base_hp_{0};
    std::atomic<uint16_t> blue_base_hp_{0};
    
    // 前哨站血量
    std::atomic<uint16_t> red_outpost_hp_{0};
    std::atomic<uint16_t> blue_outpost_hp_{0};
    
    // 机器人血量
    std::atomic<uint16_t> red_1_robot_hp_{0};
    std::atomic<uint16_t> red_3_robot_hp_{0};
    std::atomic<uint16_t> red_4_robot_hp_{0};
    std::atomic<uint16_t> blue_1_robot_hp_{0};
    std::atomic<uint16_t> blue_3_robot_hp_{0};
    std::atomic<uint16_t> blue_4_robot_hp_{0};

    // ========== timestamps ==========
    int64_t lastUpdateAgeMs() const;
    int64_t lastDetectionAgeMs() const;
    int64_t lastGameStatusAgeMs() const;
    int64_t lastSelfStatusAgeMs() const;
    bool hasPrimaryStatus(uint32_t max_age_ms = 500) const;
    bool isGameStatusFresh(uint32_t max_age_ms = 1000) const;
    bool isSelfStatusFresh(uint32_t max_age_ms = 1000) const;
    bool isDataFresh(uint32_t max_age_ms = 1000) const;

    std::chrono::steady_clock::time_point getLastUpdateTime() const {
        return steadyTimeFromMs(last_update_ms_.load());
    }

    // ========== 便捷访问方法 ==========
    /**
     * @brief 获取己方基地血量
     * @param is_blue 是否为蓝队
     * @return 己方基地血量
     */
    uint16_t getOwnBaseHp(bool is_blue) const {
        return is_blue ? blue_base_hp_.load() : red_base_hp_.load();
    }

    /**
     * @brief 获取敌方基地血量
     * @param is_blue 是否为蓝队
     * @return 敌方基地血量
     */
    uint16_t getEnemyBaseHp(bool is_blue) const {
        return is_blue ? red_base_hp_.load() : blue_base_hp_.load();
    }

    /**
     * @brief 获取己方前哨站血量
     * @param is_blue 是否为蓝队
     * @return 己方前哨站血量
     */
    uint16_t getOwnOutpostHp(bool is_blue) const {
        return is_blue ? blue_outpost_hp_.load() : red_outpost_hp_.load();
    }

    /**
     * @brief 获取敌方前哨站血量
     * @param is_blue 是否为蓝队
     * @return 敌方前哨站血量
     */
    uint16_t getEnemyOutpostHp(bool is_blue) const {
        return is_blue ? red_outpost_hp_.load() : blue_outpost_hp_.load();
    }

    /**
     * @brief 获取己方机器人血量
     * @param is_blue 是否为蓝队
     * @param robot_id 机器人ID (1, 3, 4)
     * @return 机器人血量
     */
    uint16_t getOwnRobotHp(bool is_blue, int robot_id) const {
        if (is_blue) {
            switch (robot_id) {
                case 1: return blue_1_robot_hp_.load();
                case 3: return blue_3_robot_hp_.load();
                case 4: return blue_4_robot_hp_.load();
                default: return 0;
            }
        } else {
            switch (robot_id) {
                case 1: return red_1_robot_hp_.load();
                case 3: return red_3_robot_hp_.load();
                case 4: return red_4_robot_hp_.load();
                default: return 0;
            }
        }
    }

    /**
     * @brief 获取敌方机器人血量
     * @param is_blue 是否为蓝队
     * @param robot_id 机器人ID (1, 3, 4)
     * @return 机器人血量
     */
    uint16_t getEnemyRobotHp(bool is_blue, int robot_id) const {
        return getOwnRobotHp(!is_blue, robot_id);
    }

private:
    DataManager() = default;
    ~DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    // 初始化订阅
    void setupSubscriptions();

    // 订阅回调函数
    void gameDetectionStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg);
    void gameStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg);
    void robotStatusCallback(const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg);
    void allRobotHpCallback(const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg);
    void bulletAllowanceCallback(const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg);
    void updateFromGameStatus(const rm_decision_interfaces::msg::GameStatus &msg, bool update_self_status);
    static int64_t nowMs();
    static std::chrono::steady_clock::time_point steadyTimeFromMs(int64_t ms);

    // 成员变量
    rclcpp::Node::SharedPtr node_;
    bool initialized_{false};
    std::mutex init_mutex_;

    // 订阅器
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_detection_status_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::GameStatus>::SharedPtr game_status_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::RobotStatus>::SharedPtr robot_status_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::AllRobotHP>::SharedPtr all_robot_hp_sub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::BulletAllowance>::SharedPtr bullet_allowance_sub_;

    std::atomic<int64_t> last_update_ms_{0};
    std::atomic<int64_t> last_detection_status_ms_{0};
    std::atomic<int64_t> last_game_status_ms_{0};
    std::atomic<int64_t> last_hp_status_ms_{0};
    std::atomic<int64_t> last_ammo_status_ms_{0};
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__DATA_MANAGER_HPP_ 
