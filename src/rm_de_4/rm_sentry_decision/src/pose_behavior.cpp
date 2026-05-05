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
                "姿态回执：类型=%u 成功=%d",
                static_cast<unsigned int>(msg->pose_type),
                msg->pose_success);
        });
    retry_timer_ = node_->create_wall_timer(
        std::chrono::milliseconds(RETRY_INTERVAL_MS),
        [this]() { retryPendingPoseCommand(); });
    
    RCLCPP_INFO(node_->get_logger(), "姿态管理器初始化完成，当前姿态=%s", poseToString(current_pose_).c_str());
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
        RCLCPP_DEBUG(node_->get_logger(), "姿态切换：冷却未结束，还剩 %ld 秒",
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
        
        RCLCPP_INFO(node_->get_logger(), "姿态切换：从 %s 切到 %s（命令码=%u）",
                   poseToString(old_pose).c_str(), poseToString(new_pose).c_str(),
                   static_cast<unsigned int>(poseToCommandCode(new_pose)));
    }
}

void PoseManager::publishPoseCommand(RobotPose pose) {
    auto msg = rm_decision_interfaces::msg::SentryPoseCommand();
    msg.pose_type = poseToCommandCode(pose);
    
    command_pub_->publish(msg);
    
    RCLCPP_INFO(node_->get_logger(), "姿态命令：已发布命令码=%u（%s）",
                static_cast<unsigned int>(msg.pose_type),
                poseToString(pose).c_str());
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
    
    RCLCPP_INFO(node_->get_logger(), "姿态累计时间已重置");
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
        RCLCPP_WARN(node_->get_logger(), "姿态效果衰减：%s 使用时间过长，累计=%d秒",
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
        throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("无法获取姿态管理器实例");
    }
    
    // 获取目标姿态参数
    std::string target_pose_str;
    if (!getInput("target_pose", target_pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "姿态切换：缺少 target_pose 输入");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    target_pose_ = pose_manager_->stringToPose(target_pose_str);
    pose_switched_ = false;
    
    RCLCPP_INFO(node_->get_logger(), "姿态切换：开始执行，目标姿态=%s",
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
            
            RCLCPP_INFO(node_->get_logger(), "姿态切换：执行完成");
            return BT::NodeStatus::SUCCESS;
        } else {
            // 仍然在冷却中，继续等待
            RCLCPP_DEBUG(node_->get_logger(), "姿态切换：仍在冷却中，继续等待");
            return BT::NodeStatus::RUNNING;
        }
    }
    
    return BT::NodeStatus::SUCCESS;
}

void SwitchPoseAction::onHalted() {
    RCLCPP_INFO(node_->get_logger(), "姿态切换：被中断");
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
        throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("无法获取姿态管理器实例");
    }
    
    // 获取目标姿态参数
    std::string target_pose_str;
    if (!getInput("target_pose", target_pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "姿态切换判断：缺少 target_pose 输入");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    RobotPose target_pose = pose_manager_->stringToPose(target_pose_str);
    
    // 检查是否可以切换
    bool can_switch = pose_manager_->canSwitchPose(target_pose);
    
    RCLCPP_DEBUG(node_->get_logger(), "姿态切换判断：当前=%s，目标=%s，结果=%s",
               pose_manager_->poseToString(pose_manager_->getCurrentPose()).c_str(),
               pose_manager_->poseToString(target_pose).c_str(),
               can_switch ? "成功" : "失败");
    
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
        throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("无法获取姿态管理器实例");
    }
    
    // 获取要检查的姿态参数
    std::string pose_str;
    if (!getInput("pose", pose_str)) {
        RCLCPP_ERROR(node_->get_logger(), "姿态衰减判断：缺少 pose 输入");
        return BT::NodeStatus::FAILURE;
    }
    
    // 转换为RobotPose枚举
    RobotPose pose = pose_manager_->stringToPose(pose_str);
    
    // 检查效果是否下降
    bool is_reduced = pose_manager_->isPoseEffectReduced(pose);
    
    RCLCPP_DEBUG(node_->get_logger(), "姿态衰减判断：姿态=%s，结果=%s，累计=%d秒",
               pose_manager_->poseToString(pose).c_str(),
               is_reduced ? "成功" : "失败",
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
        throw std::runtime_error("无法从行为树黑板获取 ROS 节点");
    }
    
    // 获取姿态管理器实例
    pose_manager_ = PoseManager::getInstance(node_);
    if (!pose_manager_) {
        throw std::runtime_error("无法获取姿态管理器实例");
    }
    
    // 获取当前姿态
    RobotPose current_pose = pose_manager_->getCurrentPose();
    std::string current_pose_str = pose_manager_->poseToString(current_pose);
    
    // 设置输出端口
    setOutput("current_pose", current_pose_str);
    
    RCLCPP_DEBUG(node_->get_logger(), "当前姿态读取：%s", current_pose_str.c_str());
    
    return BT::NodeStatus::SUCCESS;
}

} // namespace rm_sentry_decision
