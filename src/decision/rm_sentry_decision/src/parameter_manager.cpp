#include "rm_sentry_decision/parameter_manager.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <filesystem>
#include <iostream>
#include <set> // Added for set in getNestedValue

namespace rm_sentry_decision {

// 静态成员初始化
std::shared_ptr<ParameterManager> ParameterManager::instance_ = nullptr;

ParameterManager::ParameterManager(rclcpp::Node::SharedPtr node, const std::string& config_file)
    : node_(node), config_file_(config_file) {
    
    RCLCPP_INFO(node_->get_logger(), "ParameterManager constructor called");
    
    if (!node_) {
        RCLCPP_ERROR(rclcpp::get_logger("parameter_manager"), "Node pointer is null in ParameterManager constructor");
        throw std::runtime_error("Node pointer cannot be null");
    }
    
    try {
        RCLCPP_INFO(node_->get_logger(), "Loading config file: %s", config_file.c_str());
        
        if (!loadConfig(config_file)) {
            RCLCPP_ERROR(node_->get_logger(), "Failed to load parameter config file: %s", config_file.c_str());
            throw std::runtime_error("Parameter config file loading failed");
        }
        
        RCLCPP_INFO(node_->get_logger(), "Config loaded successfully, validating...");
        
        if (!validateConfig()) {
            RCLCPP_ERROR(node_->get_logger(), "Parameter config validation failed");
            throw std::runtime_error("Parameter config validation failed");
        }
        
        RCLCPP_INFO(node_->get_logger(), "Parameter manager initialized successfully");
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "Parameter manager initialization failed: %s", e.what());
        throw;
    }
}

bool ParameterManager::loadConfig(const std::string& config_file) {
    std::string file_path = config_file;
    
    // 如果没有指定配置文件，使用默认路径
    if (file_path.empty()) {
        file_path = ament_index_cpp::get_package_share_directory("rm_sentry_decision") + "/config/parameters.yaml";
    }
    
    RCLCPP_INFO(node_->get_logger(), "Attempting to load config file: %s", file_path.c_str());
    
    // 检查文件是否存在
    if (!std::filesystem::exists(file_path)) {
        RCLCPP_ERROR(node_->get_logger(), "Config file does not exist: %s", file_path.c_str());
        return false;
    }
    
    try {
        RCLCPP_INFO(node_->get_logger(), "Loading YAML file...");
        config_ = YAML::LoadFile(file_path);
        
        if (!config_) {
            RCLCPP_ERROR(node_->get_logger(), "YAML file loaded but config is null");
            return false;
        }
        
        config_file_ = file_path;
        RCLCPP_INFO(node_->get_logger(), "Successfully loaded config file: %s", file_path.c_str());
        
        // 打印加载的配置节点
        RCLCPP_INFO(node_->get_logger(), "Loaded config nodes:");
        try {
            if (!config_.IsMap()) {
                RCLCPP_ERROR(node_->get_logger(), "Config root is not a map");
                return false;
            }
            
            for (const auto& it : config_) {
                try {
                    RCLCPP_INFO(node_->get_logger(), "  - %s", it.first.as<std::string>().c_str());
                } catch (const std::exception& e) {
                    RCLCPP_WARN(node_->get_logger(), "Error printing config node: %s", e.what());
                }
            }
        } catch (const std::exception& e) {
            RCLCPP_WARN(node_->get_logger(), "Error iterating config nodes: %s", e.what());
        }
        
        return true;
        
    } catch (const YAML::Exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "YAML parsing error: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "Unexpected error loading config: %s", e.what());
        return false;
    }
}

bool ParameterManager::validateConfig() const {
    // 检查必需的配置节点
    std::vector<std::string> required_nodes = {
        "hp_thresholds", "ammo_thresholds", "time_thresholds", 
        "base_thresholds", "hp_diff_thresholds", "robot_status_sub",
        "need_supply", "rush_enemy_buff", "send_goal_action", 
        "logging", "decision"
    };
    
    RCLCPP_INFO(node_->get_logger(), "Validating config with %zu required nodes", required_nodes.size());
    
    if (!config_) {
        RCLCPP_ERROR(node_->get_logger(), "Config is null during validation");
        return false;
    }
    
    if (!config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "Config root is not a map during validation");
        return false;
    }
    
    for (const auto& node_name : required_nodes) {
        try {
            RCLCPP_DEBUG(node_->get_logger(), "Checking node: %s", node_name.c_str());
            
            // 使用更安全的方式检查节点
            if (!config_[node_name]) {
                RCLCPP_ERROR(node_->get_logger(), "Missing required config node: %s", node_name.c_str());
                return false;
            }
            
            const YAML::Node& node = config_[node_name];
            if (!node.IsMap()) {
                RCLCPP_ERROR(node_->get_logger(), "Config node %s is not a map", node_name.c_str());
                return false;
            }
            
            RCLCPP_DEBUG(node_->get_logger(), "Node %s is valid", node_name.c_str());
            
        } catch (const std::exception& e) {
            RCLCPP_ERROR(node_->get_logger(), "Error checking config node %s: %s", node_name.c_str(), e.what());
            return false;
        }
    }
    
    RCLCPP_INFO(node_->get_logger(), "Config validation successful");
    return true;
}

int ParameterManager::getHpThreshold(const std::string& threshold_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getHpThreshold: config invalid");
        return 600;
    }
    const YAML::Node& node = config_["hp_thresholds"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getHpThreshold: node invalid");
        return 600;
    }
    return getNestedValue(node, threshold_name, 600);
}

int ParameterManager::getAmmoThreshold(const std::string& threshold_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getAmmoThreshold: config invalid");
        return 500;
    }
    const YAML::Node& node = config_["ammo_thresholds"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getAmmoThreshold: node invalid");
        return 500;
    }
    return getNestedValue(node, threshold_name, 500);
}

int ParameterManager::getTimeThreshold(const std::string& threshold_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getTimeThreshold: config invalid");
        return 150;
    }
    const YAML::Node& node = config_["time_thresholds"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getTimeThreshold: node invalid");
        return 150;
    }
    return getNestedValue(node, threshold_name, 150);
}

int ParameterManager::getBaseThreshold(const std::string& threshold_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getBaseThreshold: config invalid");
        return 1500;
    }
    const YAML::Node& node = config_["base_thresholds"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getBaseThreshold: node invalid");
        return 1500;
    }
    return getNestedValue(node, threshold_name, 1500);
}

int ParameterManager::getHpDiffThreshold(const std::string& threshold_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getHpDiffThreshold: config invalid");
        return 50;
    }
    const YAML::Node& node = config_["hp_diff_thresholds"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getHpDiffThreshold: node invalid");
        return 50;
    }
    return getNestedValue(node, threshold_name, 50);
}

template<typename T>
T ParameterManager::getRobotStatusParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getRobotStatusParam: config invalid");
        return T{};
    }
    const YAML::Node& node = config_["robot_status_sub"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getRobotStatusParam: node invalid");
        return T{};
    }
    return getNestedValue(node, param_name, T{});
}

template<typename T>
T ParameterManager::getSupplyDecisionParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getSupplyDecisionParam: config invalid");
        return T{};
    }
    const YAML::Node& node = config_["need_supply"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getSupplyDecisionParam: node invalid");
        return T{};
    }
    return getNestedValue(node, param_name, T{});
}

template<typename T>
T ParameterManager::getBuffGuardParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getBuffGuardParam: config invalid");
        return T{};
    }
    const YAML::Node& node = config_["rush_enemy_buff"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getBuffGuardParam: node invalid");
        return T{};
    }
    return getNestedValue(node, param_name, T{});
}

template<typename T>
T ParameterManager::getNavigationParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getNavigationParam: config invalid");
        return T{};
    }
    const YAML::Node& node = config_["send_goal_action"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getNavigationParam: node invalid");
        return T{};
    }
    return getNestedValue(node, param_name, T{});
}

int ParameterManager::getLoggingParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getLoggingParam: config invalid");
        return 1000;
    }
    const YAML::Node& node = config_["logging"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getLoggingParam: node invalid");
        return 1000;
    }
    return getNestedValue(node, param_name, 1000);
}

int ParameterManager::getDecisionParam(const std::string& param_name) const {
    if (!config_ || !config_.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getDecisionParam: config invalid");
        return 40;
    }
    const YAML::Node& node = config_["decision"];
    if (!node || !node.IsMap()) {
        RCLCPP_ERROR(node_->get_logger(), "getDecisionParam: node invalid");
        return 40;
    }
    return getNestedValue(node, param_name, 40);
}

bool ParameterManager::reloadConfig(const std::string& config_file) {
    return loadConfig(config_file);
}

std::shared_ptr<ParameterManager> ParameterManager::getInstance(rclcpp::Node::SharedPtr node) {
    try {
        if (!instance_) {
            if (!node) {
                RCLCPP_ERROR(rclcpp::get_logger("parameter_manager"), "Node pointer is required for first instance creation");
                throw std::runtime_error("Node pointer is required for first instance creation");
            }
            
            RCLCPP_INFO(node->get_logger(), "Creating new ParameterManager instance");
            
            try {
                instance_ = std::make_shared<ParameterManager>(node);
                RCLCPP_INFO(node->get_logger(), "ParameterManager instance created successfully");
            } catch (const std::exception& e) {
                RCLCPP_ERROR(node->get_logger(), "Failed to create ParameterManager instance: %s", e.what());
                throw;
            }
        } else {
            RCLCPP_DEBUG(node->get_logger(), "Returning existing ParameterManager instance");
        }
        return instance_;
    } catch (const std::exception& e) {
        if (node) {
            RCLCPP_ERROR(node->get_logger(), "Failed to get ParameterManager instance: %s", e.what());
        } else {
            RCLCPP_ERROR(rclcpp::get_logger("parameter_manager"), "Failed to get ParameterManager instance: %s", e.what());
        }
        throw;
    }
}

template<typename T>
T ParameterManager::getNestedValue(const YAML::Node& node, const std::string& key, const T& default_value) const {
    try {
        // 只在第一次访问时输出调试信息，避免重复日志
        static std::set<std::string> logged_keys;
        if (logged_keys.find(key) == logged_keys.end()) {
            RCLCPP_DEBUG(node_->get_logger(), "[DEBUG] getNestedValue: key=%s, node.Type()=%d", key.c_str(), node.Type());
            logged_keys.insert(key);
        }
        
        if (!node.IsMap()) {
            RCLCPP_WARN(node_->get_logger(), "Node is not a map for key: %s", key.c_str());
            return default_value;
        }
        
        if (!node[key]) {
            RCLCPP_WARN(node_->get_logger(), "Key not found: %s, using default value", key.c_str());
            return default_value;
        }
        
        return node[key].as<T>();
    } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "Exception in getNestedValue for key %s: %s", key.c_str(), e.what());
        return default_value;
    }
}

// 显式实例化模板函数
template int ParameterManager::getRobotStatusParam<int>(const std::string&) const;
template bool ParameterManager::getRobotStatusParam<bool>(const std::string&) const;
template int ParameterManager::getSupplyDecisionParam<int>(const std::string&) const;
template std::string ParameterManager::getSupplyDecisionParam<std::string>(const std::string&) const;
template int ParameterManager::getBuffGuardParam<int>(const std::string&) const;
template std::string ParameterManager::getBuffGuardParam<std::string>(const std::string&) const;
template int ParameterManager::getNavigationParam<int>(const std::string&) const;
template std::string ParameterManager::getNavigationParam<std::string>(const std::string&) const;

} // namespace rm_sentry_decision 