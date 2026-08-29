#include "rm_sentry_decision/data_manager.hpp"

#include <algorithm>
#include <chrono>
#include <limits>

namespace rm_sentry_decision {

DataManager& DataManager::getInstance(rclcpp::Node::SharedPtr node) {
    static DataManager instance;
    
    // 如果提供了节点且未初始化，则初始化
    if (node && !instance.isInitialized()) {
        instance.initialize(node);
    }
    
    return instance;
}

void DataManager::initialize(rclcpp::Node::SharedPtr node) {
    std::lock_guard<std::mutex> lock(init_mutex_);
    
    if (initialized_) {
        RCLCPP_WARN(node->get_logger(), "裁判数据管理器已经初始化");
        return;
    }
    
    if (!node) {
        throw std::runtime_error("裁判数据管理器：ROS 节点为空");
    }
    
    node_ = node;
    setupSubscriptions();
    initialized_ = true;
    
    RCLCPP_INFO(node_->get_logger(), "裁判数据管理器初始化成功");
}

void DataManager::setupSubscriptions() {
    if (!node_) {
        throw std::runtime_error("裁判数据管理器：ROS 节点未设置");
    }
    
    // Primary fused judge/status topic used by the existing sentry BT nodes.
    game_detection_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_detection_status", 10,
        std::bind(&DataManager::gameDetectionStatusCallback, this, std::placeholders::_1));

    // Compatibility topic. It is applied only when /game_detection_status is stale.
    game_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::GameStatus>(
        "/game_status", 10,
        std::bind(&DataManager::gameStatusCallback, this, std::placeholders::_1));
    
    // 订阅机器人状态
    robot_status_sub_ = node_->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
        "/robot_status", 10,
        std::bind(&DataManager::robotStatusCallback, this, std::placeholders::_1));
    
    // 订阅全局血量
    all_robot_hp_sub_ = node_->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
        "/all_robot_hp", 10,
        std::bind(&DataManager::allRobotHpCallback, this, std::placeholders::_1));
    
    // 订阅子弹余量
    bullet_allowance_sub_ = node_->create_subscription<rm_decision_interfaces::msg::BulletAllowance>(
        "/bullet_allowance", 10,
        std::bind(&DataManager::bulletAllowanceCallback, this, std::placeholders::_1));
    
    RCLCPP_DEBUG(node_->get_logger(), "裁判数据管理器：话题订阅创建完成");
}

void DataManager::updateFromGameStatus(
    const rm_decision_interfaces::msg::GameStatus &msg,
    bool update_self_status) {
    const int64_t stamp = nowMs();
    if (update_self_status) {
        current_hp_.store(msg.current_hp);
        bullets_remaining_.store(msg.bullet_allowance_17mm);
        last_hp_status_ms_.store(stamp);
        last_ammo_status_ms_.store(stamp);
    }

    stage_remain_time_.store(msg.stage_remain_time);
    game_progress_.store(msg.game_progress);
    team_color_.store(msg.team_color);
    
    // 更新全局血量数据（如果GameStatus包含这些信息）
    red_outpost_hp_.store(msg.red_outpost_hp);
    blue_outpost_hp_.store(msg.blue_outpost_hp);
    red_base_hp_.store(msg.red_base_hp);
    blue_base_hp_.store(msg.blue_base_hp);
    
    // 更新机器人血量
    red_1_robot_hp_.store(msg.red_1_robot_hp);
    red_3_robot_hp_.store(msg.red_3_robot_hp);
    red_4_robot_hp_.store(msg.red_4_robot_hp);
    blue_1_robot_hp_.store(msg.blue_1_robot_hp);
    blue_3_robot_hp_.store(msg.blue_3_robot_hp);
    blue_4_robot_hp_.store(msg.blue_4_robot_hp);
    
    last_update_ms_.store(stamp);
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "比赛状态更新：HP=%d，剩余时间=%d，阵营=%s",
                msg.current_hp, msg.stage_remain_time,
                msg.team_color ? "蓝方" : "红方");
}

void DataManager::gameDetectionStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
    updateFromGameStatus(*msg, true);
    last_detection_status_ms_.store(nowMs());
}

void DataManager::gameStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
    last_game_status_ms_.store(nowMs());
    if (hasPrimaryStatus(500)) {
        return;
    }
    // /game_status 在部分测试和兼容场景中只发布比赛阶段，未必携带本车 HP/弹量。
    // 两个字段全为 0 时不覆盖本车状态，避免启动时把默认值误判为需要补给。
    const bool has_self_status = msg->current_hp > 0 || msg->bullet_allowance_17mm > 0;
    updateFromGameStatus(*msg, has_self_status);
}

void DataManager::robotStatusCallback(const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
    // RobotStatus does not carry game_progress, so use it only as a fallback
    // for self HP/ammo when the fused judge topic is not fresh.
    if (!hasPrimaryStatus(500)) {
        const int64_t stamp = nowMs();
        current_hp_.store(msg->current_hp);
        bullets_remaining_.store(msg->bullet_allowance_17mm);
        last_hp_status_ms_.store(stamp);
        last_ammo_status_ms_.store(stamp);
    }
    team_color_.store(msg->team_color);
    shooter_heat_.store(0);
    
    last_update_ms_.store(nowMs());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "机器人状态更新：HP=%d，阵营=%s，弹量=%d",
                msg->current_hp, msg->team_color ? "蓝方" : "红方", msg->bullet_allowance_17mm);
}

void DataManager::allRobotHpCallback(const rm_decision_interfaces::msg::AllRobotHP::SharedPtr msg) {
    // 更新全局血量数据
    red_outpost_hp_.store(msg->red_outpost_hp);
    blue_outpost_hp_.store(msg->blue_outpost_hp);
    red_base_hp_.store(msg->red_base_hp);
    blue_base_hp_.store(msg->blue_base_hp);
    
    // 更新机器人血量
    red_1_robot_hp_.store(msg->red_1_robot_hp);
    red_3_robot_hp_.store(msg->red_3_robot_hp);
    red_4_robot_hp_.store(msg->red_4_robot_hp);
    blue_1_robot_hp_.store(msg->blue_1_robot_hp);
    blue_3_robot_hp_.store(msg->blue_3_robot_hp);
    blue_4_robot_hp_.store(msg->blue_4_robot_hp);
    
    last_update_ms_.store(nowMs());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "全场血量更新：红前哨=%d 蓝前哨=%d 红基地=%d 蓝基地=%d",
                msg->red_outpost_hp, msg->blue_outpost_hp, 
                msg->red_base_hp, msg->blue_base_hp);
}

void DataManager::bulletAllowanceCallback(const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg) {
    // 更新子弹余量数据
    bullets_remaining_.store(msg->bullet_allowance_17mm);
    last_ammo_status_ms_.store(nowMs());
    
    last_update_ms_.store(nowMs());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "弹丸余量更新：17mm=%d",
                msg->bullet_allowance_17mm);
}

int64_t DataManager::nowMs() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

std::chrono::steady_clock::time_point DataManager::steadyTimeFromMs(int64_t ms) {
    return std::chrono::steady_clock::time_point(std::chrono::milliseconds(ms));
}

int64_t DataManager::lastUpdateAgeMs() const {
    const int64_t stamp = last_update_ms_.load();
    if (stamp <= 0) {
        return std::numeric_limits<int64_t>::max();
    }
    return std::max<int64_t>(0, nowMs() - stamp);
}

int64_t DataManager::lastDetectionAgeMs() const {
    const int64_t stamp = last_detection_status_ms_.load();
    if (stamp <= 0) {
        return std::numeric_limits<int64_t>::max();
    }
    return std::max<int64_t>(0, nowMs() - stamp);
}

int64_t DataManager::lastGameStatusAgeMs() const {
    const int64_t detection_stamp = last_detection_status_ms_.load();
    const int64_t game_stamp = last_game_status_ms_.load();
    const int64_t stamp = std::max(detection_stamp, game_stamp);
    if (stamp <= 0) {
        return std::numeric_limits<int64_t>::max();
    }
    return std::max<int64_t>(0, nowMs() - stamp);
}

int64_t DataManager::lastSelfStatusAgeMs() const {
    const int64_t hp_stamp = last_hp_status_ms_.load();
    const int64_t ammo_stamp = last_ammo_status_ms_.load();
    if (hp_stamp <= 0 || ammo_stamp <= 0) {
        return std::numeric_limits<int64_t>::max();
    }
    const int64_t oldest_stamp = std::min(hp_stamp, ammo_stamp);
    return std::max<int64_t>(0, nowMs() - oldest_stamp);
}

bool DataManager::hasPrimaryStatus(uint32_t max_age_ms) const {
    return lastDetectionAgeMs() <= static_cast<int64_t>(max_age_ms);
}

bool DataManager::isGameStatusFresh(uint32_t max_age_ms) const {
    return lastGameStatusAgeMs() <= static_cast<int64_t>(max_age_ms);
}

bool DataManager::isSelfStatusFresh(uint32_t max_age_ms) const {
    return lastSelfStatusAgeMs() <= static_cast<int64_t>(max_age_ms);
}

bool DataManager::isDataFresh(uint32_t max_age_ms) const {
    return lastUpdateAgeMs() <= static_cast<int64_t>(max_age_ms);
}

} // namespace rm_sentry_decision 
