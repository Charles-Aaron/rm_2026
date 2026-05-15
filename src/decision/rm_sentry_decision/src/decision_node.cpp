#include "rclcpp/rclcpp.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/controls/sequence_star_node.h"
#include "behaviortree_cpp_v3/loggers/bt_cout_logger.h"
#include "behaviortree_cpp_v3/loggers/bt_zmq_publisher.h"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <signal.h>

// 包含自定义节点头文件
#include "robot_status_sub.hpp"
#include "enemy.hpp"
#include "operator.hpp"
#include "game_status_sub.hpp"
#include "base_attack.hpp"
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
#include "late_game_decision.hpp"
// #include "wait_for_outpost_destroyed.hpp"
#include "self_hp_below.hpp"
#include "self_hp_at_least.hpp"
#include "is_outside_pose_radius.hpp"
#include "random_patrol_pose.hpp"
#include "select_supply_point.hpp"
#include "rush_enemy_buff.hpp"
#include "pose_behavior.hpp"
#include "sim_visualization_move.hpp"
#include "calculate_attack_pose.hpp"
#include "set_nav_speed.hpp"
#include "gimbal_lower_action.hpp"
#include "topics_to_blackboard.hpp"
#include "log_info_action.hpp"
#include "patrol_waypoints.hpp"
#include "rm_sentry_decision/data_manager.hpp"

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
    std::cout << "[调试] 开始初始化 decision_node..." << std::endl;
    
    // 设置信号处理器
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    rclcpp::init(argc, argv);
    std::cout << "[调试] ROS2 初始化成功" << std::endl;
    
    //auto node = std::make_shared<rclcpp::Node>("sentry_decision");
    auto options = rclcpp::NodeOptions().allow_undeclared_parameters(true);
    auto node = std::make_shared<rclcpp::Node>("sentry_decision", options);
    std::cout << "[调试] ROS2 节点创建成功" << std::endl;
    
    BT::BehaviorTreeFactory factory;
    std::cout << "[调试] 行为树工厂创建成功" << std::endl;
    factory.registerBuilder<BT::SequenceStarNode>(
      "SequenceWithMemory",
      [](const std::string& name, const BT::NodeConfiguration&) {
        return std::make_unique<BT::SequenceStarNode>(name);
      });
    
    // 注册自定义节点 - 逐个注册并添加错误处理
    std::cout << "[调试] 开始注册行为树节点..." << std::endl;
    
    try {
      std::cout << "[调试] 正在注册 RobotStatusSub（机器人状态订阅）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RobotStatusSub>("RobotStatusSub");
      std::cout << "[调试] RobotStatusSub（机器人状态订阅）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] RobotStatusSub（机器人状态订阅）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "RobotStatusSub（机器人状态订阅）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 GameStatusSub（比赛状态订阅）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GameStatusSub>("GameStatusSub");
      std::cout << "[调试] GameStatusSub（比赛状态订阅）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] GameStatusSub（比赛状态订阅）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "GameStatusSub（比赛状态订阅）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 OutpostAttack（前哨站进攻判断）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::OutpostAttack>("OutpostAttack");
      std::cout << "[调试] OutpostAttack（前哨站进攻判断）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] OutpostAttack（前哨站进攻判断）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "OutpostAttack（前哨站进攻判断）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 SendGoalAction（单点导航）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SendGoalAction>("SendGoalAction");
      std::cout << "[调试] SendGoalAction（单点导航）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SendGoalAction（单点导航）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SendGoalAction（单点导航）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 SendThroughGoalsAction（途经点导航）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SendThroughGoalsAction>("SendThroughGoalsAction");
      std::cout << "[调试] SendThroughGoalsAction（途经点导航）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SendThroughGoalsAction（途经点导航）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SendThroughGoalsAction（途经点导航）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 NeedSupply（是否需要补给）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::NeedSupply>("NeedSupply");
      std::cout << "[调试] NeedSupply（是否需要补给）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] NeedSupply（是否需要补给）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "NeedSupply（是否需要补给）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 RecoveredEnough（补给完成判断）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RecoveredEnough>("RecoveredEnough");
      std::cout << "[调试] RecoveredEnough（补给完成判断）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] RecoveredEnough（补给完成判断）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "RecoveredEnough（补给完成判断）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 GuardFortress（堡垒守卫）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GuardFortress>("GuardFortress");
      std::cout << "[调试] GuardFortress（堡垒守卫）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] GuardFortress（堡垒守卫）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "GuardFortress（堡垒守卫）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 AlliesDown（队友阵亡判断）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::AlliesDown>("AlliesDown");
      std::cout << "[调试] AlliesDown（队友阵亡判断）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] AlliesDown（队友阵亡判断）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "AlliesDown（队友阵亡判断）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 HeroProtectAttack（英雄保护进攻）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::HeroProtectAttack>("HeroProtectAttack");
      std::cout << "[调试] HeroProtectAttack（英雄保护进攻）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] HeroProtectAttack（英雄保护进攻）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "HeroProtectAttack（英雄保护进攻）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 RandomPatrolPose（随机巡逻点）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RandomPatrolPose>("RandomPatrolPose");
      std::cout << "[调试] RandomPatrolPose（随机巡逻点）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] RandomPatrolPose（随机巡逻点）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "RandomPatrolPose（随机巡逻点）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 SelectSupplyPoint（选择补给点）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelectSupplyPoint>("SelectSupplyPoint");
      std::cout << "[调试] SelectSupplyPoint（选择补给点）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SelectSupplyPoint（选择补给点）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SelectSupplyPoint（选择补给点）注册失败: %s", e.what());
    }

     try {
      std::cout << "[调试] 正在注册 Enemy（敌方目标）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::Enemy>("Enemy");
      std::cout << "[调试] Enemy（敌方目标）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] Enemy（敌方目标）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Enemy（敌方目标）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 Operator（操作手目标）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::Operator>("Operator");
      std::cout << "[调试] Operator（操作手目标）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] Operator（操作手目标）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "Operator（操作手目标）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 SwitchPoseAction（切换姿态）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SwitchPoseAction>("SwitchPoseAction");
      std::cout << "[调试] SwitchPoseAction（切换姿态）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SwitchPoseAction（切换姿态）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SwitchPoseAction（切换姿态）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 CanSwitchPoseCondition（能否切换姿态）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::CanSwitchPoseCondition>("CanSwitchPoseCondition");
      std::cout << "[调试] CanSwitchPoseCondition（能否切换姿态）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] CanSwitchPoseCondition（能否切换姿态）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "CanSwitchPoseCondition（能否切换姿态）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 IsPoseEffectReducedCondition（姿态效果衰减判断）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::IsPoseEffectReducedCondition>("IsPoseEffectReducedCondition");
      std::cout << "[调试] IsPoseEffectReducedCondition（姿态效果衰减判断）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] IsPoseEffectReducedCondition（姿态效果衰减判断）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "IsPoseEffectReducedCondition（姿态效果衰减判断）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 GetCurrentPoseCondition（读取当前姿态）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GetCurrentPoseCondition>("GetCurrentPoseCondition");
      std::cout << "[调试] GetCurrentPoseCondition（读取当前姿态）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] GetCurrentPoseCondition（读取当前姿态）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "GetCurrentPoseCondition（读取当前姿态）注册失败: %s", e.what());
    }
    
    try {
      std::cout << "[调试] 正在注册 SimVisualizationMove（仿真可视化移动）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SimVisualizationMove>("SimVisualizationMove");
      std::cout << "[调试] SimVisualizationMove（仿真可视化移动）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SimVisualizationMove（仿真可视化移动）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SimVisualizationMove（仿真可视化移动）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 CalculateAttackPose（计算进攻点）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::CalculateAttackPose>("CalculateAttackPose");
      std::cout << "[调试] CalculateAttackPose（计算进攻点）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] CalculateAttackPose（计算进攻点）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "CalculateAttackPose（计算进攻点）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 SelfHpBelow（本车血量低于阈值）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelfHpBelow>("SelfHpBelow");
      std::cout << "[调试] SelfHpBelow（本车血量低于阈值）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SelfHpBelow（本车血量低于阈值）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SelfHpBelow（本车血量低于阈值）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 SelfHpAtLeast（本车血量达到阈值）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SelfHpAtLeast>("SelfHpAtLeast");
      std::cout << "[调试] SelfHpAtLeast（本车血量达到阈值）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SelfHpAtLeast（本车血量达到阈值）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SelfHpAtLeast（本车血量达到阈值）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 IsOutsidePoseRadius（是否离开点位半径）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::IsOutsidePoseRadius>("IsOutsidePoseRadius");
      std::cout << "[调试] IsOutsidePoseRadius（是否离开点位半径）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] IsOutsidePoseRadius（是否离开点位半径）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "IsOutsidePoseRadius（是否离开点位半径）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 SetNavSpeedAction（设置导航速度）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::SetNavSpeedAction>("SetNavSpeedAction");
      std::cout << "[调试] SetNavSpeedAction（设置导航速度）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] SetNavSpeedAction（设置导航速度）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "SetNavSpeedAction（设置导航速度）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 GimbalLowerAction（云台低头/抬头控制）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::GimbalLowerAction>("GimbalLowerAction");
      std::cout << "[调试] GimbalLowerAction（云台低头/抬头控制）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] GimbalLowerAction（云台低头/抬头控制）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "GimbalLowerAction（云台低头/抬头控制）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 LateGameDecision（后期决策）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::LateGameDecision>("LateGameDecision");
      std::cout << "[调试] LateGameDecision（后期决策）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] LateGameDecision（后期决策）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "LateGameDecision（后期决策）注册失败: %s", e.what());
    }

    try {
      std::cout << "[调试] 正在注册 RushEnemyBuff（抢敌方增益点）..." << std::endl;
      factory.registerNodeType<rm_sentry_decision::RushEnemyBuff>("RushEnemyBuff");
      std::cout << "[调试] RushEnemyBuff（抢敌方增益点）注册成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] RushEnemyBuff（抢敌方增益点）注册失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "RushEnemyBuff（抢敌方增益点）注册失败: %s", e.what());
    }

	    try {
	      std::cout << "[调试] 正在注册 BaseAttack（基地进攻）..." << std::endl;
	      factory.registerNodeType<rm_sentry_decision::BaseAttack>("BaseAttack");
	      std::cout << "[调试] BaseAttack（基地进攻）注册成功" << std::endl;
	    } catch (const std::exception& e) {
	      std::cout << "[错误] BaseAttack（基地进攻）注册失败: " << e.what() << std::endl;
	      RCLCPP_ERROR(node->get_logger(), "BaseAttack（基地进攻）注册失败: %s", e.what());
	    }

	    try {
	      std::cout << "[调试] 正在注册裁判数据黑板和巡逻节点..." << std::endl;
	      factory.registerNodeType<rm_sentry_decision::TopicsToBlackboard>("TopicsToBlackboard");
	      factory.registerNodeType<rm_sentry_decision::LogInfoAction>("LogInfoAction");
	      factory.registerNodeType<rm_sentry_decision::LoadWaypoints>("LoadWaypoints");
	      factory.registerNodeType<rm_sentry_decision::GetCurrentWaypoint>("GetCurrentWaypoint");
	      factory.registerNodeType<rm_sentry_decision::WaitUntilReached>("WaitUntilReached");
	      factory.registerNodeType<rm_sentry_decision::WaitDuration>("WaitDuration");
	      factory.registerNodeType<rm_sentry_decision::NextWaypoint>("NextWaypoint");
	      std::cout << "[调试] 裁判数据黑板和巡逻节点注册成功" << std::endl;
	    } catch (const std::exception& e) {
	      std::cout << "[错误] 裁判数据黑板和巡逻节点注册失败: " << e.what() << std::endl;
	      RCLCPP_ERROR(node->get_logger(), "裁判数据黑板和巡逻节点注册失败: %s", e.what());
	    }

    std::cout << "[调试] 所有行为树节点注册完成" << std::endl;
    
    // 获取行为树XML路径（从参数或默认路径）
    std::string xml_path;
    try {
      std::cout << "[调试] 正在读取行为树 XML 路径..." << std::endl;
      node->declare_parameter("xml_path", "");
      xml_path = node->get_parameter("xml_path").as_string();
      if (xml_path.empty()) {
        xml_path = ament_index_cpp::get_package_share_directory("rm_sentry_decision") + "/config/rmuc_2025_first_attack_patrol.xml";
        std::cout << "[调试] 使用默认行为树 XML: " << xml_path << std::endl;
      } else {
        std::cout << "[调试] 使用参数指定的行为树 XML: " << xml_path << std::endl;
      }
    } catch (const std::exception& e) {
      std::cout << "[错误] 读取行为树 XML 路径失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "读取行为树 XML 路径失败: %s", e.what());
      return 1;
    }
    
    // 创建行为树
    std::cout << "[调试] 正在创建行为树黑板..." << std::endl;
    auto blackboard = BT::Blackboard::create();
    blackboard->set<rclcpp::Node::SharedPtr>("node", node);
    rm_sentry_decision::DataManager::getInstance(node);
    std::cout << "[调试] 行为树黑板创建成功" << std::endl;
    
    BT::Tree tree;
    try {
      std::cout << "[调试] 正在从 XML 创建行为树: " << xml_path << std::endl;
      tree = factory.createTreeFromFile(xml_path, blackboard);
      std::cout << "[调试] 行为树创建成功" << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[错误] 行为树创建失败: " << e.what() << std::endl;
      RCLCPP_ERROR(node->get_logger(), "行为树创建失败: %s", e.what());
      return 1;
    }
    
    std::cout << "[调试] 正在创建行为树日志/监控器..." << std::endl;
    bool bt_stdout_log = true;
    try {
      node->declare_parameter("bt_stdout_log", true);
      bt_stdout_log = node->get_parameter("bt_stdout_log").as_bool();
    } catch (const std::exception& e) {
      RCLCPP_WARN(node->get_logger(), "读取 bt_stdout_log 参数失败，默认开启终端行为树状态日志: %s", e.what());
    }

    std::unique_ptr<BT::StdCoutLogger> logger_cout;
    if (bt_stdout_log) {
      logger_cout = std::make_unique<BT::StdCoutLogger>(tree);
      RCLCPP_INFO(node->get_logger(), "行为树终端状态日志已开启，可通过参数 bt_stdout_log:=false 关闭");
    } else {
      RCLCPP_INFO(node->get_logger(), "行为树终端状态日志未开启，可通过参数 bt_stdout_log:=true 开启");
    }
    
    // 创建 ZMQ 发布器用于 Groot 监控。端口被占用时不影响行为树运行。
    std::unique_ptr<BT::PublisherZMQ> publisher_zmq;
    try {
      publisher_zmq = std::make_unique<BT::PublisherZMQ>(tree, 50, 1666, 1667);
      RCLCPP_INFO(node->get_logger(), "Groot ZMQ 监控发布器已启动，端口 1666/1667");
    } catch (const std::exception& e) {
      RCLCPP_WARN(
        node->get_logger(),
        "Groot ZMQ 监控发布器未启动，端口 1666/1667 不可用: %s",
        e.what());
    }
    
    std::cout << "[调试] 行为树日志/监控器创建完成" << std::endl;
    
    RCLCPP_INFO(node->get_logger(), "行为树开始运行，频率 50Hz");
    std::cout << "[调试] 进入主循环..." << std::endl;
    
    rclcpp::Rate rate(50);
    while (rclcpp::ok() && !shutdown_requested) {
      try {
        rclcpp::spin_some(node);
        tree.tickRoot();
        rate.sleep();
      } catch (const std::exception& e) {
        std::cout << "[错误] 主循环出现异常: " << e.what() << std::endl;
        RCLCPP_ERROR(node->get_logger(), "主循环出现异常: %s", e.what());
        break;
      }
    }
    
    std::cout << "[调试] 正在优雅关闭..." << std::endl;
    
    // 优雅关闭：先停止行为树，再关闭ROS2
    try {
      // 给一些时间让所有回调完成
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      // 关闭ROS2
      rclcpp::shutdown();
      
      // 再给一些时间让资源析构
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
    } catch (const std::exception& e) {
      std::cout << "[警告] 关闭过程中出现异常: " << e.what() << std::endl;
    }
    
    std::cout << "[调试] 关闭完成" << std::endl;
    return 0;
    
  } catch (const std::exception& e) {
    std::cout << "[致命错误] main 中出现未处理异常: " << e.what() << std::endl;
    std::cout << "[致命错误] 异常类型: " << typeid(e).name() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "[致命错误] main 中出现未知异常" << std::endl;
    return 1;
  }
}
