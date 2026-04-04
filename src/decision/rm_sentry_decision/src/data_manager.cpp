#include "rm_sentry_decision/data_manager.hpp"
#include <chrono>

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
        RCLCPP_WARN(node->get_logger(), "DataManager already initialized");
        return;
    }
    
    if (!node) {
        throw std::runtime_error("DataManager: Node pointer is null");
    }
    
    node_ = node;
    setupSubscriptions();
    initialized_ = true;
    
    RCLCPP_INFO(node_->get_logger(), "DataManager initialized successfully");
}

void DataManager::setupSubscriptions() {
    if (!node_) {
        throw std::runtime_error("DataManager: Node not set");
    }
    
    // 订阅游戏状态
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
    
    RCLCPP_DEBUG(node_->get_logger(), "DataManager subscriptions setup completed");
}

void DataManager::gameStatusCallback(const rm_decision_interfaces::msg::GameStatus::SharedPtr msg) {
    // 更新游戏状态数据
    current_hp_.store(msg->current_hp);
    stage_remain_time_.store(msg->stage_remain_time);
    game_progress_.store(msg->game_progress);
    team_color_.store(msg->team_color);
    
    // 更新子弹余量（从GameStatus中获取）
    bullets_remaining_.store(msg->bullet_allowance_17mm);
    
    // 更新全局血量数据（如果GameStatus包含这些信息）
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
    
    // 更新时间戳
    last_update_time_.store(std::chrono::steady_clock::now());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "GameStatus Update - HP: %d, Time: %d, Team: %s", 
                msg->current_hp, msg->stage_remain_time, 
                msg->team_color ? "Blue" : "Red");
}

void DataManager::robotStatusCallback(const rm_decision_interfaces::msg::RobotStatus::SharedPtr msg) {
    // 更新机器人状态数据
    current_hp_.store(msg->current_hp);
    team_color_.store(msg->team_color);
    shooter_heat_.store(msg->shooter_heat);
    
    // 更新时间戳
    last_update_time_.store(std::chrono::steady_clock::now());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "RobotStatus Update - HP: %d, Team: %s, Heat: %d", 
                msg->current_hp, msg->team_color ? "Blue" : "Red", msg->shooter_heat);
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
    
    // 更新时间戳
    last_update_time_.store(std::chrono::steady_clock::now());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "AllRobotHP Update - R_Out: %d B_Out: %d R_Base: %d B_Base: %d", 
                msg->red_outpost_hp, msg->blue_outpost_hp, 
                msg->red_base_hp, msg->blue_base_hp);
}

void DataManager::bulletAllowanceCallback(const rm_decision_interfaces::msg::BulletAllowance::SharedPtr msg) {
    // 更新子弹余量数据
    bullets_remaining_.store(msg->bullet_allowance_17mm);
    
    // 更新时间戳
    last_update_time_.store(std::chrono::steady_clock::now());
    
    RCLCPP_DEBUG(node_->get_logger(), 
                "BulletAllowance Update - 17mm: %d", 
                msg->bullet_allowance_17mm);
}

} // namespace rm_sentry_decision 