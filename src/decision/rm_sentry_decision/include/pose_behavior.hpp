#ifndef RM_SENTRY_DECISION__POSE_BEHAVIOR_HPP_
#define RM_SENTRY_DECISION__POSE_BEHAVIOR_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "rm_decision_interfaces/msg/sentry_pose_command.hpp"
#include "rm_decision_interfaces/msg/sentry_pose_result.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>

// 姿态枚举（使用描述性名称）
enum class RobotPose {
    MOVEMENT,  // 移动姿态
    ATTACK,    // 进攻姿态
    DEFENSE    // 防御姿态
};

namespace rm_sentry_decision {

// 姿态管理类
class PoseManager {
public:
    static std::shared_ptr<PoseManager> getInstance(rclcpp::Node::SharedPtr node = nullptr) {
        static std::shared_ptr<PoseManager> instance;
        if (!instance && node) {
            instance = std::shared_ptr<PoseManager>(new PoseManager(node));
        }
        return instance;
    }
    
    // 获取当前姿态
    RobotPose getCurrentPose() const {
        return current_pose_.load();
    }
    
    // 检查是否可以切换姿态
    bool canSwitchPose(RobotPose target_pose);
    
    // 执行姿态切换（发送官方协议指令）
    void switchPose(RobotPose new_pose);
    
    // 检查姿态效果是否下降
    bool isPoseEffectReduced(RobotPose pose);
    
    // 获取姿态累计时间（秒）
    int getAccumulatedTime(RobotPose pose) const;
    
    // 姿态转字符串
    std::string poseToString(RobotPose pose) const;
    
    // 字符串转姿态
    RobotPose stringToPose(const std::string& pose_str) const;
    
    // 获取官方指令 ID
    std::string poseToCommandString(RobotPose pose) const {
        switch(pose) {
            case RobotPose::ATTACK: return "ATTACK";
            case RobotPose::DEFENSE: return "DEFENSE";
            case RobotPose::MOVEMENT: return "MOVEMENT";
            default: return "UNKNOWN";
        }
    }

    // 获取姿态指令编码
    uint8_t poseToCommandCode(RobotPose pose) const {
        switch (pose) {
            case RobotPose::MOVEMENT: return 0;
            case RobotPose::ATTACK:   return 1;
            case RobotPose::DEFENSE:  return 2;
            default:                  return 0;
        }
    }
    
private:
    explicit PoseManager(rclcpp::Node::SharedPtr node);
    
    // 更新当前姿态的累计时间
    void updateAccumulatedTime();
    
    // 重置姿态累计时间
    void resetPoseAccumulatedTime();
    
    // 发布姿态切换指令
    void publishPoseCommand(RobotPose pose);
    
    // 未收到成功回执时重发
    void retryPendingPoseCommand();
    
    rclcpp::Node::SharedPtr node_;
    std::atomic<RobotPose> current_pose_;  // 当前姿态
    
    // 最后一次姿态切换时间
    std::chrono::steady_clock::time_point last_switch_time_;
    
    // 各姿态开始时间
    std::unordered_map<RobotPose, std::chrono::steady_clock::time_point> pose_start_time_;
    
    // 各姿态累计时间
    std::unordered_map<RobotPose, std::chrono::seconds> pose_accumulated_time_;
    
    // 姿态切换冷却时间（秒）
    const int SWITCH_COOLDOWN = 5;
    
    // 姿态效果下降时间（秒）
    const int EFFECT_REDUCE_TIME = 180;  // 3 分钟
    
    // 姿态命令通信
    rclcpp::Publisher<rm_decision_interfaces::msg::SentryPoseCommand>::SharedPtr command_pub_;
    rclcpp::Subscription<rm_decision_interfaces::msg::SentryPoseResult>::SharedPtr result_sub_;
    rclcpp::TimerBase::SharedPtr retry_timer_;
    
    // 指令状态
    std::atomic<RobotPose> desired_pose_{RobotPose::MOVEMENT};
    std::atomic<bool> awaiting_success_ack_{false};
    std::atomic<bool> last_result_success_{false};
    std::atomic<bool> result_received_{false};
    const int RETRY_INTERVAL_MS = 500;
};

// 切换姿态动作节点
class SwitchPoseAction : public BT::StatefulActionNode {
public:
    SwitchPoseAction(const std::string& name, 
                   const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("target_pose", "移动姿态", "目标姿态（移动姿态/进攻姿态/防御姿态）")
        };
    }
    
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;
    
private:
    rclcpp::Node::SharedPtr node_;
    std::shared_ptr<PoseManager> pose_manager_;
    RobotPose target_pose_;
    bool pose_switched_ = false;
};

// 检查姿态是否可以切换的条件节点
class CanSwitchPoseCondition : public BT::ConditionNode {
public:
    CanSwitchPoseCondition(const std::string& name, 
                         const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("target_pose", "移动姿态", "目标姿态（移动姿态/进攻姿态/防御姿态）")
        };
    }
    
    BT::NodeStatus tick() override;
    
private:
    rclcpp::Node::SharedPtr node_;
    std::shared_ptr<PoseManager> pose_manager_;
};

// 检查姿态效果是否下降的条件节点
class IsPoseEffectReducedCondition : public BT::ConditionNode {
public:
    IsPoseEffectReducedCondition(const std::string& name, 
                               const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("pose", "移动姿态", "要检查的姿态（移动姿态/进攻姿态/防御姿态）")
        };
    }
    
    BT::NodeStatus tick() override;
    
private:
    rclcpp::Node::SharedPtr node_;
    std::shared_ptr<PoseManager> pose_manager_;
};

// 获取当前姿态的条件节点
class GetCurrentPoseCondition : public BT::ConditionNode {
public:
    GetCurrentPoseCondition(const std::string& name, 
                          const BT::NodeConfiguration& config);
    
    static BT::PortsList providedPorts() {
        return {
            BT::OutputPort<std::string>("current_pose", "当前姿态输出")
        };
    }
    
    BT::NodeStatus tick() override;
    
private:
    rclcpp::Node::SharedPtr node_;
    std::shared_ptr<PoseManager> pose_manager_;
};

} // namespace rm_sentry_decision

#endif // RM_SENTRY_DECISION__POSE_BEHAVIOR_HPP_
