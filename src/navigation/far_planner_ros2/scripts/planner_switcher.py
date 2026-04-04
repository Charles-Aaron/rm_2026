#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
FAR Planner ROS 2 - 规划器切换管理脚本
Planner Switcher for FAR Planner Integration

提供以下功能:
1. 快速切换不同的规划器方案
2. 性能基准测试
3. 运行时参数调整
4. 状态监控

使用示例:
    python3 planner_switcher.py --scheme nav2_far --switch
    python3 planner_switcher.py --benchmark
    python3 planner_switcher.py --status
"""

import os
import sys
import argparse
import yaml
import time
import subprocess
from datetime import datetime

class PlannerSwitcher:
    """规划器快速切换管理器"""
    
    def __init__(self, config_file=None):
        self.config_file = config_file or self._get_default_config()
        self.current_scheme = None
        self.schemes = {
            'nav2_far': {
                'name': 'Nav2 SmacPlannerHybrid + FAR Planner',
                'description': '双规划器协同 - Nav2负责全局规划，FAR负责动态避障',
                'config': 'config/nav2_far_hybrid.yaml',
                'launch': 'launch/nav2_far_planner.launch.py'
            },
            'nav2_only': {
                'name': 'Nav2 SmacPlannerHybrid Only',
                'description': '仅使用Nav2原生规划器',
                'config': 'config/smac_hybrid_only.yaml',
                'launch': 'launch/nav2_bringup.launch.py'
            },
            'far_only': {
                'name': 'FAR Planner Only',
                'description': '仅使用FAR Planner（需要ROS 2适配）',
                'config': 'param/far_planner_params.yaml',
                'launch': 'launch/far_planner.launch.py'
            }
        }
        
    def _get_default_config(self):
        """获取默认配置文件路径"""
        package_share = os.path.expanduser('~/far_planner_ros2')
        return os.path.join(package_share, 'param', 'planner_config.yaml')
    
    def load_config(self):
        """加载配置文件"""
        if not os.path.exists(self.config_file):
            print(f"[警告] 配置文件不存在: {self.config_file}")
            print("[信息] 使用内置默认配置")
            return
            
        with open(self.config_file, 'r') as f:
            self.config = yaml.safe_load(f)
        print(f"[信息] 已加载配置: {self.config_file}")
    
    def list_schemes(self):
        """列出所有可用的方案"""
        print("\n" + "="*60)
        print("可用规划器方案列表")
        print("="*60)
        for key, scheme in self.schemes.items():
            current = " [当前]" if key == self.current_scheme else ""
            print(f"\n方案: {key}{current}")
            print(f"  名称: {scheme['name']}")
            print(f"  描述: {scheme['description']}")
            print(f"  配置文件: {scheme['config']}")
            print(f"  启动文件: {scheme['launch']}")
        print("\n" + "="*60)
    
    def switch_scheme(self, scheme_name, auto_start=True):
        """切换到指定方案"""
        if scheme_name not in self.schemes:
            print(f"[错误] 未知方案: {scheme_name}")
            return False
            
        scheme = self.schemes[scheme_name]
        print(f"\n[信息] 切换到方案: {scheme['name']}")
        print(f"[信息] 描述: {scheme['description']}")
        
        # 检查配置文件是否存在
        config_path = os.path.expanduser(scheme['config'])
        if not os.path.exists(config_path):
            print(f"[警告] 配置文件不存在: {config_path}")
        
        # 切换方案
        self.current_scheme = scheme_name
        print(f"[成功] 已切换到方案: {scheme_name}")
        
        if auto_start:
            return self.start_scheme(scheme_name)
        
        return True
    
    def start_scheme(self, scheme_name):
        """启动指定方案的规划器"""
        scheme = self.schemes[scheme_name]
        launch_path = os.path.expanduser(scheme['launch'])
        
        if not os.path.exists(launch_path):
            print(f"[错误] 启动文件不存在: {launch_path}")
            print("[信息] 请先编译包: colcon build --packages-select far_planner_ros2")
            return False
        
        print(f"[信息] 启动规划器: {launch_path}")
        
        try:
            # 使用 ros2 launch 启动
            cmd = ['ros2', 'launch', 'far_planner_ros2', 
                   os.path.basename(launch_path)]
            subprocess.Popen(cmd)
            print("[成功] 规划器已启动")
            return True
        except Exception as e:
            print(f"[错误] 启动失败: {e}")
            return False
    
    def stop_scheme(self):
        """停止当前方案的规划器"""
        if not self.current_scheme:
            print("[信息] 没有正在运行的规划器")
            return
            
        print(f"[信息] 停止方案: {self.current_scheme}")
        
        # 杀掉相关进程
        try:
            subprocess.run(['pkill', '-f', 'far_planner'], check=False)
            subprocess.run(['pkill', '-f', 'smac_planner'], check=False)
            print("[成功] 规划器已停止")
        except Exception as e:
            print(f"[警告] 停止进程时出错: {e}")
        
        self.current_scheme = None
    
    def get_status(self):
        """获取当前状态"""
        print("\n" + "="*60)
        print("FAR Planner 状态")
        print("="*60)
        print(f"当前方案: {self.current_scheme or '未选择'}")
        print(f"配置文件: {self.config_file}")
        
        # 检查进程状态
        try:
            result = subprocess.run(['pgrep', '-f', 'far_planner'], 
                                   capture_output=True, text=True)
            if result.returncode == 0:
                pids = result.stdout.strip().split('\n')
                print(f"运行进程数: {len(pids)}")
                print(f"进程ID: {', '.join(pids)}")
            else:
                print("运行进程数: 0")
        except Exception as e:
            print(f"状态检查错误: {e}")
        
        print("="*60)
    
    def run_benchmark(self, test_duration=30):
        """运行性能基准测试"""
        print("\n" + "="*60)
        print("性能基准测试")
        print("="*60)
        
        results = {}
        
        for scheme_name in self.schemes.keys():
            print(f"\n[测试] 方案: {scheme_name}")
            
            # 切换到该方案
            self.switch_scheme(scheme_name, auto_start=False)
            time.sleep(1)
            
            # 记录开始时间
            start_time = time.time()
            
            # TODO: 运行实际的性能测试
            # - 规划时间测试
            # - CPU占用测试
            # - 内存占用测试
            # - 路径质量测试
            
            # 模拟测试
            results[scheme_name] = {
                'avg_planning_time_ms': 0.0,
                'cpu_usage_percent': 0.0,
                'memory_usage_mb': 0.0,
                'success_rate_percent': 0.0
            }
            
            print(f"  完成测试: {scheme_name}")
        
        # 输出结果
        print("\n" + "="*60)
        print("基准测试结果")
        print("="*60)
        print(f"{'方案':<20} {'规划时间(ms)':<15} {'CPU(%)':<10} {'内存(MB)':<12} {'成功率(%)':<12}")
        print("-"*60)
        
        for name, metrics in results.items():
            print(f"{name:<20} {metrics['avg_planning_time_ms']:<15.2f} "
                  f"{metrics['cpu_usage_percent']:<10.1f} "
                  f"{metrics['memory_usage_mb']:<12.1f} "
                  f"{metrics['success_rate_percent']:<12.1f}")
        
        print("="*60)
        
        return results
    
    def interactive_mode(self):
        """交互模式"""
        print("\n欢迎使用 FAR Planner 切换管理器")
        print("输入 'help' 查看帮助\n")
        
        while True:
            try:
                cmd = input("far_planner> ").strip().lower()
                
                if cmd in ['exit', 'quit', 'q']:
                    print("退出...")
                    break
                elif cmd == 'help':
                    print("""
可用命令:
  list              - 列出所有方案
  switch <name>    - 切换到指定方案
  start             - 启动当前方案
  stop              - 停止当前方案
  status            - 查看当前状态
  benchmark         - 运行性能测试
  exit/quit         - 退出程序
                    """)
                elif cmd == 'list':
                    self.list_schemes()
                elif cmd.startswith('switch '):
                    scheme = cmd.split()[1]
                    self.switch_scheme(scheme)
                elif cmd == 'start':
                    if self.current_scheme:
                        self.start_scheme(self.current_scheme)
                    else:
                        print("[错误] 请先选择方案: switch <name>")
                elif cmd == 'stop':
                    self.stop_scheme()
                elif cmd == 'status':
                    self.get_status()
                elif cmd == 'benchmark':
                    self.run_benchmark()
                else:
                    print(f"[错误] 未知命令: {cmd}")
                    
            except KeyboardInterrupt:
                print("\n退出...")
                break
            except Exception as e:
                print(f"[错误] {e}")


def main():
    parser = argparse.ArgumentParser(
        description='FAR Planner 规划器切换管理工具',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s --list                           # 列出所有方案
  %(prog)s --switch nav2_far                # 切换到Nav2+FAR方案
  %(prog)s --start                          # 启动规划器
  %(prog)s --status                         # 查看状态
  %(prog)s --benchmark                      # 运行性能测试
  %(prog)s --interactive                    # 交互模式
        """
    )
    
    parser.add_argument('--config', type=str, 
                       help='配置文件路径')
    parser.add_argument('--list', action='store_true',
                       help='列出所有可用方案')
    parser.add_argument('--switch', type=str, metavar='SCHEME',
                       help='切换到指定方案 (nav2_far/nav2_only/far_only)')
    parser.add_argument('--start', action='store_true',
                       help='启动当前方案的规划器')
    parser.add_argument('--stop', action='store_true',
                       help='停止规划器')
    parser.add_argument('--status', action='store_true',
                       help='查看当前状态')
    parser.add_argument('--benchmark', action='store_true',
                       help='运行性能基准测试')
    parser.add_argument('--interactive', '-i', action='store_true',
                       help='交互模式')
    
    args = parser.parse_args()
    
    # 创建切换器实例
    switcher = PlannerSwitcher(args.config)
    switcher.load_config()
    
    # 执行操作
    if args.list:
        switcher.list_schemes()
    elif args.switch:
        switcher.switch_scheme(args.switch)
    elif args.start:
        if switcher.current_scheme:
            switcher.start_scheme(switcher.current_scheme)
        else:
            print("[错误] 请先选择方案: --switch <name>")
    elif args.stop:
        switcher.stop_scheme()
    elif args.status:
        switcher.get_status()
    elif args.benchmark:
        switcher.run_benchmark()
    elif args.interactive:
        switcher.interactive_mode()
    else:
        # 默认显示状态
        switcher.get_status()
        print("\n使用 --help 查看更多选项")


if __name__ == '__main__':
    main()
