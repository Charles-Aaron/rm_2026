#include "rclcpp/rclcpp.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/loggers/bt_cout_logger.h"
#include "behaviortree_cpp_v3/loggers/bt_zmq_publisher.h"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <signal.h>

// 包含自定义节点头文件
#include "robot_status_sub.hpp"
#include "enemy.hpp"
#include "operator.hpp"
#include "game_status_sub.hpp"
// #include "base_attack.hpp"
#include "outpost_attack.hpp"
#include "send_goal.hpp"
#include "send_through_goals.hpp"
#include "need_supply.hpp"
#include "recovered_enough.hpp"
// #include "attack_strategy.hpp"
#include "guard_fortress.hpp"
#include "allies_down.hpp"
// #include "endgame_need_supply.hpp"
// #include "should_rush_base.hpp"
#include "hero_protect_attack.hpp"
// #include "compare_hp_diff.hpp"
// #include "late_game_decision.hpp"
// #include "wait_for_outpost_destroyed.hpp"
#include "self_hp_below.hpp"
#include "self_hp_at_least.hpp"
#include "is_outside_pose_radius.hpp"
#include "random_patrol_pose.hpp"
#include "select_supply_point.hpp"
// #include "rush_enemy_buff.hpp"
#include "pose_behavior.hpp"
#include "sim_visualization_move.hpp"
#include "calculate_attack_pose.hpp"
#include "set_nav_speed.hpp"

// 节点构造器包装类
template <typename T>
class NodeWrapper : public T {
public:
    NodeWrapper(const std::string& name, const BT::NodeConfiguration& config,
               rclcpp::Node::SharedPtr node)
        : T(name, config) {
        this->config().blackboard->set("node", node);
    }
};

// 全局变量用于优雅关闭
static volatile sig_atomic_t shutdown_requested = 0;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        shutdown_requested = 1;
    }
}

int main(int argc, char** argv) {
  try {
    std::cout << "[DEBUG] Starting decision_node initialization..." << std::endl;
    
    // 设置信号处理器
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    rclcpp::init(argc, argv);
    std::cout << "[DEBUG] ROS2 initialized successfully" << std::endl;
    
    //auto node = std::make_shared<rclcpp::Node>("sentry_decision");
    auto options = rclcpp::NodeOptions().allow_undeclared_parameters(true);
    auto node = std::make_shared<rclcpp::Node>("sentry_decision", options);
    std::cout << "[DEBUG] ROS2 node created successfully" << std::endl;
    
    BT::BehaviorTreeFactory factory;
    std::cout << "[DEBUG] BehaviorTreeFactory created successfully" << std::endl;
    
    // 注册自定义节点 - 逐个注册并添加错误处理
    std::cout << "[DEBUG] Starting node registration..." << std::endl;
    
    try {
      std::cout << "[DEBUG] Registering RobotStatusSub..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RobotStatusSub>("RobotStatusSub");
      std::cout << "[DEBUG] RobotStatusSub registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register RobotStatusSub: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register RobotStatusSub: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering GameStatusSub..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GameStatusSub>("GameStatusSub");
      std::cout << "[DEBUG] GameStatusSub registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register GameStatusSub: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register GameStatusSub: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering OutpostAttack..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::OutpostAttack>("OutpostAttack");
      std::cout << "[DEBUG] OutpostAttack registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register OutpostAttack: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register OutpostAttack: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering SendGoalAction..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SendGoalAction>("SendGoalAction");
      std::cout << "[DEBUG] SendGoalAction registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SendGoalAction: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SendGoalAction: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering SendThroughGoalsAction..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SendThroughGoalsAction>("SendThroughGoalsAction");
      std::cout << "[DEBUG] SendThroughGoalsAction registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SendThroughGoalsAction: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SendThroughGoalsAction: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering NeedSupply..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::NeedSupply>("NeedSupply");
      std::cout << "[DEBUG] NeedSupply registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register NeedSupply: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register NeedSupply: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering RecoveredEnough..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RecoveredEnough>("RecoveredEnough");
      std::cout << "[DEBUG] RecoveredEnough registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register RecoveredEnough: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register RecoveredEnough: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering GuardFortress..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GuardFortress>("GuardFortress");
      std::cout << "[DEBUG] GuardFortress registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register GuardFortress: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register GuardFortress: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering AlliesDown..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::AlliesDown>("AlliesDown");
      std::cout << "[DEBUG] AlliesDown registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register AlliesDown: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register AlliesDown: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering HeroProtectAttack..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::HeroProtectAttack>("HeroProtectAttack");
      std::cout << "[DEBUG] HeroProtectAttack registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register HeroProtectAttack: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register HeroProtectAttack: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering RandomPatrolPose..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RandomPatrolPose>("RandomPatrolPose");
      std::cout << "[DEBUG] RandomPatrolPose registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register RandomPatrolPose: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register RandomPatrolPose: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering SelectSupplyPoint..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelectSupplyPoint>("SelectSupplyPoint");
      std::cout << "[DEBUG] SelectSupplyPoint registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SelectSupplyPoint: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SelectSupplyPoint: %s", e.what());
    }

     try {
      std::cout << "[DEBUG] Registering Enemy..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::Enemy>("Enemy");
      std::cout << "[DEBUG] Enemy registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register Enemy: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register Enemy: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering Operator..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::Operator>("Operator");
      std::cout << "[DEBUG] Operator registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register Operator: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register Operator: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering SwitchPoseAction..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SwitchPoseAction>("SwitchPoseAction");
      std::cout << "[DEBUG] SwitchPoseAction registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SwitchPoseAction: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SwitchPoseAction: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering CanSwitchPoseCondition..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::CanSwitchPoseCondition>("CanSwitchPoseCondition");
      std::cout << "[DEBUG] CanSwitchPoseCondition registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register CanSwitchPoseCondition: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register CanSwitchPoseCondition: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering IsPoseEffectReducedCondition..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::IsPoseEffectReducedCondition>("IsPoseEffectReducedCondition");
      std::cout << "[DEBUG] IsPoseEffectReducedCondition registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register IsPoseEffectReducedCondition: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register IsPoseEffectReducedCondition: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering GetCurrentPoseCondition..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GetCurrentPoseCondition>("GetCurrentPoseCondition");
      std::cout << "[DEBUG] GetCurrentPoseCondition registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register GetCurrentPoseCondition: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register GetCurrentPoseCondition: %s", e.what());
    }
    
    try {
      std::cout << "[DEBUG] Registering SimVisualizationMove..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SimVisualizationMove>("SimVisualizationMove");
      std::cout << "[DEBUG] SimVisualizationMove registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SimVisualizationMove: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SimVisualizationMove: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering CalculateAttackPose..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::CalculateAttackPose>("CalculateAttackPose");
      std::cout << "[DEBUG] CalculateAttackPose registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register CalculateAttackPose: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register CalculateAttackPose: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering SelfHpBelow..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelfHpBelow>("SelfHpBelow");
      std::cout << "[DEBUG] SelfHpBelow registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SelfHpBelow: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SelfHpBelow: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering SelfHpAtLeast..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelfHpAtLeast>("SelfHpAtLeast");
      std::cout << "[DEBUG] SelfHpAtLeast registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SelfHpAtLeast: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SelfHpAtLeast: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering IsOutsidePoseRadius..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::IsOutsidePoseRadius>("IsOutsidePoseRadius");
      std::cout << "[DEBUG] IsOutsidePoseRadius registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register IsOutsidePoseRadius: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register IsOutsidePoseRadius: %s", e.what());
    }

    try {
      std::cout << "[DEBUG] Registering SetNavSpeedAction..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SetNavSpeedAction>("SetNavSpeedAction");
      std::cout << "[DEBUG] SetNavSpeedAction registered successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to register SetNavSpeedAction: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to register SetNavSpeedAction: %s", e.what());
    }

    std::cout << "[DEBUG] All nodes registered successfully" << std::endl;
    
    // 获取行为树XML路径（从参数或默认路径）
    std::string xml_path;
    try {
      std::cout << "[DEBUG] Getting XML path..." << std::endl;
      if (node->has_parameter("xml_path")) {
        xml_path = node->get_parameter("xml_path").as_string();
        std::cout << "[DEBUG] XML path from parameter: " << xml_path << std::endl;
      } else {
        xml_path = ament_index_cpp::get_package_share_directory("rm_sentry_decision") + "/config/rmuc_2025.xml";
        std::cout << "[DEBUG] XML path from default: " << xml_path << std::endl;
      }
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to get XML path: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to get XML path: %s", e.what());
      return 1;
    }
    
    // 创建行为树
    std::cout << "[DEBUG] Creating blackboard..." << std::endl;
    auto blackboard = BT::Blackboard::create();
    blackboard->set<rclcpp::Node::SharedPtr>("node", node);
    std::cout << "[DEBUG] Blackboard created successfully" << std::endl;
    
    BT::Tree tree;
    try {
      std::cout << "[DEBUG] Creating tree from file: " << xml_path << std::endl;
      tree = factory.createTreeFromFile(xml_path, blackboard);
      std::cout << "[DEBUG] Tree created successfully" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[ERROR] Failed to create tree: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Failed to create tree: %s", e.what());
      return 1;
    }
    
    std::cout << "[DEBUG] Creating logger..." << std::endl;
    // BT::StdCoutLogger logger_cout(tree);
    
    // 创建 ZMQ 发布器用于 Groot 监控
    BT::PublisherZMQ publisher_zmq(tree, 50, 1666, 1667);
    RCLCPP_INFO(node->get_logger(), "Groot ZMQ publisher started on ports 1666/1667");
    
    std::cout << "[DEBUG] Logger created successfully" << std::endl;
    
    RCLCPP_INFO(node->get_logger(), "Starting behavior tree at 50Hz");
    std::cout << "[DEBUG] Starting main loop..." << std::endl;
    
    rclcpp::Rate rate(50);
    while (rclcpp::ok() && !shutdown_requested) {
      try {
        tree.tickRoot();
        rclcpp::spin_some(node);
        rate.sleep();
      } catch (const std::exception& e) {
        std::cout << "[ERROR] Exception in main loop: " << e.what() << std::endl;
        RCLCPP_ERROR(node->get_logger(), "Exception in main loop: %s", e.what());
        break;
      }
    }
    
    std::cout << "[DEBUG] Shutting down gracefully..." << std::endl;
    
    // 优雅关闭：先停止行为树，再关闭ROS2
    try {
      // 给一些时间让所有回调完成
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      // 关闭ROS2
      rclcpp::shutdown();
      
      // 再给一些时间让资源析构
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
    } catch (const std::exception& e) {
      std::cout << "[WARNING] Exception during shutdown: " << e.what() << std::endl;
    }
    
    std::cout << "[DEBUG] Shutdown complete" << std::endl;
    return 0;
    
  } catch (const std::exception& e) {
    std::cout << "[FATAL ERROR] Unhandled exception in main: " << e.what() << std::endl;
    std::cout << "[FATAL ERROR] Exception type: " << typeid(e).name() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "[FATAL ERROR] Unknown exception in main" << std::endl;
    return 1;
  }
}
