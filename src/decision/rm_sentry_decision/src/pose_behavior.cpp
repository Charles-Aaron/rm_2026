#include "pose_behavior.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"

namespace rm_sentry_decision {


PoseManager::PoseManager(rclcpp::Node::SharedPtr node)
    : node_(node), current_pose_(RobotPose::MOVEMENT)
{
    last_switch_time_ = std::chrono::steady_clock::now();
    
    // 初始化累计时间和开始时间
    resetPoseAccumulatedTime();
    
    pose_start_time_[current_pose_] = std::chrono::steady_clock::now();
    
    // 初始化姿态通信（专用命令/回执消息）
    command_pub_ = node_->create_publisher<rm_decision_interfaces::msg::SentryPoseCommand>(
        "/sentry/pose_command", 10);
    result_sub_ = node_->create_subscription<rm_decision_interfaces::msg::SentryPoseResult>(
        "/sentry/pose_result", 10,
        [this](const rm_decision_interfaces::msg::SentryPoseResult::SharedPtr msg) {
            const uint8_t expected = poseToCommandCode(desired_pose_.load());
            if (msg->pose_type != expected) {
                return;
            }

            last_result_success_.store(msg->pose_success);
            result_received_.store(true);
            awaiting_success_ack_.store(!msg->pose_success);

            RCLCPP_DEBUG(node_->get_logger(),
                "Received pose result: type=%u success=%d",
                static_cast<unsigned int>(msg->pose_type),
                msg->pose_success);
        });
    retry_timer_ = node_->create_wall_timer(
        std::chrono::milliseconds(RETRY_INTERVAL_MS),
        [this]() { retryPendingPoseCommand(); });
    
    RCLCPP_INFO(node_->get_logger(), "PoseManager initialized, current pose: %s", poseToString(current_pose_).c_str());
}

bool PoseManager::canSwitchPose(RobotPose target_pose) {
    // 同一姿态不需要切换，直接返回true表示可以切换（实际不会执行切换操作）
    if (target_pose == getCurrentPose()) {
        return true;
    }
    
    // 检查冷却时间
    auto now = std::chrono::steady_clock::now();
    auto time_since_last_switch = std::chrono::duration_cast<std::chrono::seconds>(
        now - last_switch_time_);
    
    if (time_since_last_switch < std::chrono::seconds(SWITCH_COOLDOWN)) {
        RCLCPP_DEBUG(node_->get_logger(), "Cannot switch pose: cooldown not expired (%ld seconds remaining)",
                   static_cast<long>((std::chrono::seconds(SWITCH_COOLDOWN) - time_since_last_switch).count()));
        return false;
    }
    
    return true;
}

void PoseManager::switchPose(RobotPose new_pose) {
    if (canSwitchPose(new_pose)) {
        // 更新当前姿态的累计时间
        updateAccumulatedTime();
        
        RobotPose old_pose = current_pose_.load();
        current_pose_.store(new_pose);
        
        // 更新切换时间和新姿态的开始时间
        last_switch_time_ = std::chrono::steady_clock::now();
        pose_start_time_[new_pose] = last_switch_time_;
        desired_pose_.store(new_pose);
        result_received_.store(false);
        last_result_success_.store(false);
        awaiting_success_ack_.store(true);
        
        // 发布姿态命令
        publishPoseCommand(new_pose);
        
        RCLCPP_INFO(node_->get_logger(), "Pose switched from %s to %s (command: %s)", 
                   poseToString(old_pose).c_str(), poseToString(new_pose).c_str(),
                   poseToCommandString(new_pose).c_str());
    }
}

void PoseManager::publishPoseCommand(RobotPose pose) {
    auto msg = rm_decision_interfaces::msg::SentryPoseCommand();
    msg.pose_type = poseToCommandCode(pose);
    
    command_pub_->publish(msg);
    
    RCLCPP_INFO(node_->get_logger(), "Published pose command: code=%u (%s)",
                static_cast<unsigned int>(msg.pose_type),
                poseToCommandString(pose).c_str());
}

void PoseManager::retryPendingPoseCommand() {
    if (!awaiting_success_ack_.load()) {
        return;
    }
    publishPoseCommand(desired_pose_.load());
}

bool PoseManager::isPoseEffectReduced(RobotPose pose) {
    updateAccumulatedTime();
    return pose_accumulated_time_[pose] > std::chrono::seconds(EFFECT_REDUCE_TIME);
}

int PoseManager::getAccumulatedTime(RobotPose pose) const {
    auto it = pose_accumulated_time_.find(pose);
    if (it != pose_accumulated_time_.end()) {
        return static_cast<int>(it->second.count());
    }
    return 0;
}

std::string PoseManager::poseToString(RobotPose pose) const {
    switch (pose) {
        case RobotPose::MOVEMENT:
            return "移动姿态";
        case RobotPose::ATTACK:
            return "进攻姿态";
        case RobotPose::DEFENSE:
            return "防御姿态";
        default:
            return "未知姿态";
    }
}

RobotPose PoseManager::stringToPose(const std::string& pose_str) const {
    if (pose_str == "移动姿态" || pose_str == "MOVEMENT") {
        return RobotPose::MOVEMENT;
    } else if (pose_str == "进攻姿态" || pose_str == "ATTACK") {
        return RobotPose::ATTACK;
    } else if (pose_str == "防御姿态" || pose_str == "DEFENSE") {
        return RobotPose::DEFENSE;
    }
    return RobotPose::MOVEMENT;  // 默认返回移动姿态
}

void PoseManager::resetPoseAccumulatedTime() {
    // 重置所有姿态的累计时间为0秒
    pose_accumulated_time_[RobotPose::MOVEMENT] = std::chrono::seconds(0);
    pose_accumulated_time_[RobotPose::ATTACK] = std::chrono::seconds(0);
    pose_accumulated_time_[RobotPose::DEFENSE] = std::chrono::seconds(0);
    
    RCLCPP_INFO(node_->get_logger(), "Pose accumulated time reset");
}

void PoseManager::updateAccumulatedTime() {
    RobotPose current = current_pose_.load();
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        now - pose_start_time_[current]);
    
    pose_accumulated_time_[current] += duration;
    pose_start_time_[current] = now;
    
    // 检查效果是否下降（直接比较，避免递归调用）
    if (pose_accumulated_time_[current] > std::chrono::seconds(EFFECT_REDUCE_TIME)) {
        RCLCPP_WARN(node_->get_logger(), "Pose %s effect reduced due to long usage (accumulated: %ds)", 
                   poseToString(current).c_str(), getAccumulatedTime(current));
    }
}

// ====================================== SwitchPoseAction Implementation ======================================

SwitchPoseAction::SwitchPoseAction(const std::string& name,
                                 const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name, config) {
}

BT::NodeStatus SwitchPoseAction::onStart() {
    // 从黑板获取ROS节点
    node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("Failed to get PoseManager instance");
    }
    
    // 获取目标姿态参数
    std::string target_pose_str;
    if (!getInput("target_pose", target_pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "Failed to get target_pose input");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    target_pose_ = pose_manager_->stringToPose(target_pose_str);
    pose_switched_ = false;
    
    RCLCPP_INFO(node_->get_logger(), "SwitchPoseAction started, target pose: %s", 
               pose_manager_->poseToString(target_pose_).c_str());
    
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SwitchPoseAction::onRunning() {
    if (!pose_switched_) {
        // 检查是否可以切换姿态
        if (pose_manager_->canSwitchPose(target_pose_)) {
            // 执行姿态切换
            pose_manager_->switchPose(target_pose_);
            pose_switched_ = true;
            
            RCLCPP_INFO(node_->get_logger(), "SwitchPoseAction completed successfully");
            return BT::NodeStatus::SUCCESS;
        } else {
            // 仍然在冷却中，继续等待
            RCLCPP_DEBUG(node_->get_logger(), "SwitchPoseAction still in cooldown, waiting...");
            return BT::NodeStatus::RUNNING;
        }
    }
    
    return BT::NodeStatus::SUCCESS;
}

void SwitchPoseAction::onHalted() {
    RCLCPP_INFO(node_->get_logger(), "SwitchPoseAction halted");
}

// ====================================== CanSwitchPoseCondition Implementation ======================================

CanSwitchPoseCondition::CanSwitchPoseCondition(const std::string& name,
                                             const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
}

BT::NodeStatus CanSwitchPoseCondition::tick() {
    // 从黑板获取ROS节点
    node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("Failed to get PoseManager instance");
    }
    
    // 获取目标姿态参数
    std::string target_pose_str;
    if (!getInput("target_pose", target_pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "Failed to get target_pose input");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    RobotPose target_pose = pose_manager_->stringToPose(target_pose_str);
    
    // 检查是否可以切换
    bool can_switch = pose_manager_->canSwitchPose(target_pose);
    
    RCLCPP_DEBUG(node_->get_logger(), "CanSwitchPoseCondition: current=%s, target=%s, result=%s", 
               pose_manager_->poseToString(pose_manager_->getCurrentPose()).c_str(),
               pose_manager_->poseToString(target_pose).c_str(),
               can_switch ? "SUCCESS" : "FAILURE");
    
    return can_switch ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

// ====================================== IsPoseEffectReducedCondition Implementation ======================================

IsPoseEffectReducedCondition::IsPoseEffectReducedCondition(const std::string& name,
                                                       const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
}

BT::NodeStatus IsPoseEffectReducedCondition::tick() {
    // 从黑板获取ROS节点
    node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("Failed to get PoseManager instance");
    }
    
    // 获取要检查的姿态参数
    std::string pose_str;
    if (!getInput("pose", pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "Failed to get pose input");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    RobotPose pose = pose_manager_->stringToPose(pose_str);
    
    // 检查效果是否下降
    bool is_reduced = pose_manager_->isPoseEffectReduced(pose);
    
    RCLCPP_DEBUG(node_->get_logger(), "IsPoseEffectReducedCondition: pose=%s, result=%s (accumulated: %ds)", 
               pose_manager_->poseToString(pose).c_str(),
               is_reduced ? "SUCCESS" : "FAILURE",
               pose_manager_->getAccumulatedTime(pose));
    
    return is_reduced ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

// ====================================== GetCurrentPoseCondition Implementation ======================================

GetCurrentPoseCondition::GetCurrentPoseCondition(const std::string& name,
                                               const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config) {
}

BT::NodeStatus GetCurrentPoseCondition::tick() {
    // 从黑板获取ROS节点
    node_ = config().blackboard->get<rclcpp::Node::SharedPtr>("node");
    if (!node_) {
        throw std::runtime_error("Failed to get ROS node from blackboard");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("Failed to get PoseManager instance");
    }
    
    // 获取当前姿态
    RobotPose current_pose = pose_manager_->getCurrentPose();
    std::string current_pose_str = pose_manager_->poseToString(current_pose);
    
    // 设置输出端口
    setOutput("current_pose", current_pose_str);
    
    RCLCPP_DEBUG(node_->get_logger(), "GetCurrentPoseCondition: current pose=%s", current_pose_str.c_str());
    
    return BT::NodeStatus::SUCCESS;
}

} // namespace rm_sentry_decision
