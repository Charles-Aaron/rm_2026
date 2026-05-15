#!/usr/bin/env python3
"""Local web UI for editing and launching the first-attack patrol BT config."""

import argparse
import csv
import html
import json
import math
import os
import signal
import shlex
import subprocess
import threading
import time
import urllib.parse
import webbrowser
from collections import deque
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from xml.etree import ElementTree as ET


SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8765
BT_XML = "rmuc_2025_first_attack_patrol.xml"
PATROL_CSV = "first_attack_patrol.csv"
SIM_CONFIG_JSON = "first_attack_referee_sim.json"

process = None
sim_process = None
nav_process = None
econtrol_process = None
status_monitor = None
log_lines = deque(maxlen=300)
status_message = ""
auto_demo_thread = None
auto_demo_message = "未运行"


def package_root():
    script_path = Path(__file__).resolve()
    source_root = script_path.parents[1]
    if (source_root / "config" / BT_XML).exists():
        return source_root

    try:
        from ament_index_python.packages import get_package_share_directory

        return Path(get_package_share_directory("rm_sentry_decision"))
    except Exception:
        return source_root


def workspace_root():
    script_path = Path(__file__).resolve()
    for parent in script_path.parents:
        if (parent / "install" / "setup.bash").exists():
            return parent
        if (parent / "share" / "rm_sentry_decision").exists():
            if (parent / "setup.bash").exists() or (parent / "local_setup.bash").exists():
                return parent
    return None


def script_root():
    candidate = Path(__file__).resolve().parent
    if (candidate / "referee_simulator.py").exists():
        return candidate

    if PKG_ROOT.parent.name == "share":
        lib_candidate = PKG_ROOT.parent.parent / "lib" / "rm_sentry_decision"
        if (lib_candidate / "referee_simulator.py").exists():
            return lib_candidate
    return candidate


def workspace_setup_path():
    if WORKSPACE_DIR is None:
        return None

    candidates = [
        WORKSPACE_DIR / "install" / "setup.bash",
        WORKSPACE_DIR / "setup.bash",
        WORKSPACE_DIR / "local_setup.bash",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


PKG_ROOT = package_root()
WORKSPACE_DIR = workspace_root()
SCRIPT_ROOT = script_root()
WORKSPACE_SETUP = workspace_setup_path()
RUNTIME_ROOT = WORKSPACE_DIR if WORKSPACE_DIR is not None else PKG_ROOT
CONFIG_DIR = PKG_ROOT / "config"
XML_PATH = CONFIG_DIR / BT_XML
CSV_PATH = CONFIG_DIR / PATROL_CSV
SIM_CONFIG_PATH = CONFIG_DIR / SIM_CONFIG_JSON
SIM_SCRIPT = SCRIPT_ROOT / "referee_simulator.py"
NAV_SIM_SCRIPT = SCRIPT_ROOT / "nav_pose_simulator.py"
ECONTROL_SIM_SCRIPT = SCRIPT_ROOT / "electrical_feedback_simulator.py"

DEFAULT_SIM_CONFIG = {
    "game_progress": "4",
    "stage_remain_time": "300",
    "team_color": "red",
    "current_hp": "400",
    "ammo_17mm": "500",
    "red_base_hp": "3000",
    "blue_base_hp": "3000",
    "red_outpost_hp": "0",
    "blue_outpost_hp": "0",
    "red_1_hp": "400",
    "red_3_hp": "400",
    "red_4_hp": "400",
    "blue_1_hp": "400",
    "blue_3_hp": "400",
    "blue_4_hp": "400",
}


def parse_pose(value):
    parts = [p.strip() for p in value.split(";")]
    nums = []
    for part in parts:
        if part:
            nums.append(float(part))
    while len(nums) < 7:
        nums.append(0.0)
    if len(nums) == 7:
        return nums
    raise ValueError(f"pose must contain 7 values: {value}")


def fmt_num(value):
    text = f"{float(value):.6f}".rstrip("0").rstrip(".")
    return text if text else "0"


def fmt_pose(values):
    nums = list(values)
    if len(nums) < 7:
        nums += [0.0] * (7 - len(nums))
    if len(nums) != 7:
        raise ValueError("pose must contain 7 values")
    return "; ".join(fmt_num(v) for v in nums)


def pose_xyz(pose):
    nums = parse_pose(pose)
    return nums[0], nums[1], nums[2]


def set_pose_xyz(pose, x, y, z):
    nums = parse_pose(pose)
    nums[0] = float(x)
    nums[1] = float(y)
    nums[2] = float(z)
    return fmt_pose(nums)


def xml_tree():
    parser = ET.XMLParser(target=ET.TreeBuilder(insert_comments=True))
    return ET.parse(XML_PATH, parser=parser)


def find_action(root, action_id=None, name=None):
    for node in root.iter("Action"):
        if action_id and node.get("ID") != action_id:
            continue
        if name and node.get("name") != name:
            continue
        return node
    return None


def find_condition(root, condition_id=None, name=None):
    for node in root.iter("Condition"):
        if condition_id and node.get("ID") != condition_id:
            continue
        if name and node.get("name") != name:
            continue
        return node
    return None


def find_first_action(root, action_id, names):
    for name in names:
        node = find_action(root, action_id, name)
        if node is not None:
            return node
    return None


def find_first_condition(root, condition_id, names):
    for name in names:
        node = find_condition(root, condition_id, name)
        if node is not None:
            return node
    return None


def find_named_actions(root, action_id, names):
    nodes = []
    for name in names:
        node = find_action(root, action_id, name)
        if node is not None:
            nodes.append(node)
    return nodes


def find_named_conditions(root, condition_id, names):
    nodes = []
    for name in names:
        node = find_condition(root, condition_id, name)
        if node is not None:
            nodes.append(node)
    return nodes


def load_patrol_rows():
    rows = []
    if not CSV_PATH.exists():
        return rows
    with CSV_PATH.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(row)
    return rows


def load_config():
    tree = xml_tree()
    root = tree.getroot()
    low = find_action(root, "SendThroughGoalsAction", "开局经过低头提前触发点到低头等待点")
    up = find_first_action(
        root,
        "SendThroughGoalsAction",
        [
            "快速通过抬头触发点并发送抬头信号到第一进攻点",
            "不停顿经过抬头触发点并发送抬头信号到第一进攻点",
            "不停顿经过抬头触发点到第一进攻点",
            "经过抬头触发点后到第一进攻点",
        ])
    need = find_first_condition(
        root,
        "NeedSupply",
        ["判断是否低血需要撤退补给", "判断是否需要撤退补给", "开局判断是否需要先补给"])
    recovered = find_first_condition(
        root, "RecoveredEnough", ["确认补给后血量弹量足够", "开局确认补给后血量弹量足够"])
    supply = find_first_action(
        root,
        "SendThroughGoalsAction",
        ["反向经过低头触发点并发送抬头信号到补给点"])
    if supply is None:
        supply = find_first_action(root, "SendGoalAction", ["导航到补给点", "开局导航到补给点"])
    patrol_speed = find_first_action(
        root, "SetNavSpeedAction", ["巡逻速度设为3.5米每秒", "巡逻速度设为2.0米每秒"])
    wait_reached = find_action(root, "WaitUntilReached", "等待机器人进入目标0.6米范围")

    config = {
        "xml_path": str(XML_PATH),
        "csv_path": str(CSV_PATH),
        "low_trigger": low.get("through_pose", "") if low is not None else "",
        "low_wait": low.get("final_pose", "") if low is not None else "",
        "head_up_trigger": up.get("through_pose", "") if up is not None else "",
        "first_attack": up.get("final_pose", "") if up is not None else "",
        "supply_pose": (
            supply.get("goal_pose") or supply.get("final_pose", "")
            if supply is not None else ""
        ),
        "enter_hp": need.get("enter_hp", "110") if need is not None else "110",
        "enter_ammo": need.get("enter_ammo", "40") if need is not None else "40",
        "recover_hp": recovered.get("min_hp", "400") if recovered is not None else "400",
        "recover_ammo": recovered.get("min_ammo", "50") if recovered is not None else "50",
        "patrol_speed": patrol_speed.get("speed_mps", "2.0") if patrol_speed is not None else "2.0",
        "patrol_tolerance": wait_reached.get("tolerance", "0.6") if wait_reached is not None else "0.6",
        "patrol_rows": load_patrol_rows(),
    }
    return config


def load_sim_config():
    config = dict(DEFAULT_SIM_CONFIG)
    if SIM_CONFIG_PATH.exists():
        try:
            with SIM_CONFIG_PATH.open(encoding="utf-8") as handle:
                saved = json.load(handle)
            for key in config:
                if key in saved:
                    config[key] = str(saved[key])
        except Exception:
            pass
    return config


def form_value(fields, name, default=""):
    value = fields.get(name, [default])[0]
    return value.strip()


def save_sim_config(fields):
    current = load_sim_config()
    config = {}
    for key, default in current.items():
        value = form_value(fields, f"sim_{key}", default)
        if key == "team_color":
            config[key] = "blue" if value == "blue" else "red"
        else:
            config[key] = str(int(float(value)))
    with SIM_CONFIG_PATH.open("w", encoding="utf-8") as handle:
        json.dump(config, handle, ensure_ascii=False, indent=2)
        handle.write("\n")
    return config


def update_config(fields):
    tree = xml_tree()
    root = tree.getroot()

    low = find_action(root, "SendThroughGoalsAction", "开局经过低头提前触发点到低头等待点")
    up = find_first_action(
        root,
        "SendThroughGoalsAction",
        [
            "快速通过抬头触发点并发送抬头信号到第一进攻点",
            "不停顿经过抬头触发点并发送抬头信号到第一进攻点",
            "不停顿经过抬头触发点到第一进攻点",
            "经过抬头触发点后到第一进攻点",
        ])
    need_nodes = find_named_conditions(
        root,
        "NeedSupply",
        ["开局判断是否需要先补给", "判断是否需要撤退补给", "判断是否低血需要撤退补给"])
    recovered_nodes = find_named_conditions(
        root, "RecoveredEnough", ["开局确认补给后血量弹量足够", "确认补给后血量弹量足够"])
    supply_nodes = find_named_actions(root, "SendGoalAction", ["开局导航到补给点", "导航到补给点"])
    supply_through_nodes = find_named_actions(
        root, "SendThroughGoalsAction", ["反向经过低头触发点并发送抬头信号到补给点"])
    return_first_nodes = find_named_actions(
        root, "SendGoalAction", ["开局补给后返回第一进攻点", "补给后返回第一进攻点"])
    patrol_speed = find_first_action(
        root, "SetNavSpeedAction", ["巡逻速度设为3.5米每秒", "巡逻速度设为2.0米每秒"])
    wait_reached = find_action(root, "WaitUntilReached", "等待机器人进入目标0.6米范围")

    low_trigger = set_pose_xyz(low.get("through_pose"), form_value(fields, "low_trigger_x"), form_value(fields, "low_trigger_y"), 0)
    low_wait = set_pose_xyz(low.get("final_pose"), form_value(fields, "low_wait_x"), form_value(fields, "low_wait_y"), 0)
    head_up = set_pose_xyz(up.get("through_pose"), form_value(fields, "head_up_x"), form_value(fields, "head_up_y"), 0)
    first_attack = set_pose_xyz(up.get("final_pose"), form_value(fields, "first_attack_x"), form_value(fields, "first_attack_y"), 0)
    if supply_nodes:
        supply_base = supply_nodes[0].get("goal_pose")
    elif supply_through_nodes:
        supply_base = supply_through_nodes[0].get("final_pose")
    else:
        supply_base = "0; 0; 0; 0; 0; 0; 1"
    supply_pose = set_pose_xyz(supply_base, form_value(fields, "supply_x"), form_value(fields, "supply_y"), 0)

    low.set("through_pose", low_trigger)
    low.set("final_pose", low_wait)
    up.set("through_pose", head_up)
    up.set("final_pose", first_attack)
    for supply in supply_nodes:
        supply.set("goal_pose", supply_pose)
    for supply in supply_through_nodes:
        supply.set("final_pose", supply_pose)
    for return_first in return_first_nodes:
        return_first.set("goal_pose", first_attack)

    enter_hp = str(int(float(form_value(fields, "enter_hp"))))
    enter_ammo = str(int(float(form_value(fields, "enter_ammo"))))
    recover_hp = str(int(float(form_value(fields, "recover_hp"))))
    recover_ammo = str(int(float(form_value(fields, "recover_ammo"))))
    for need in need_nodes:
        need.set("enter_hp", enter_hp)
        need.set("enter_ammo", enter_ammo)
        need.set("exit_hp", recover_hp)
        need.set("exit_ammo", recover_ammo)
    for recovered in recovered_nodes:
        recovered.set("min_hp", recover_hp)
        recovered.set("min_ammo", recover_ammo)
    patrol_speed.set("speed_mps", fmt_num(form_value(fields, "patrol_speed")))
    patrol_speed.set("name", f"巡逻速度设为{fmt_num(form_value(fields, 'patrol_speed'))}米每秒")
    wait_reached.set("tolerance", fmt_num(form_value(fields, "patrol_tolerance")))

    ET.indent(tree, space="  ")
    tree.write(XML_PATH, encoding="UTF-8", xml_declaration=True)

    rows = []
    for idx in range(2):
        x = form_value(fields, f"patrol_{idx}_x")
        y = form_value(fields, f"patrol_{idx}_y")
        wait = form_value(fields, f"patrol_{idx}_wait")
        rows.append({
            "id": str(idx),
            "pose_x": fmt_num(x),
            "pose_y": fmt_num(y),
            "pose_z": "0",
            "rot_x": "0",
            "rot_y": "0",
            "rot_z": "0",
            "rot_w": "1",
            "command": "",
            "wait_sec": fmt_num(wait),
        })

    with CSV_PATH.open("w", newline="", encoding="utf-8") as handle:
        fieldnames = ["id", "pose_x", "pose_y", "pose_z", "rot_x", "rot_y", "rot_z", "rot_w", "command", "wait_sec"]
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def process_running():
    return process is not None and process.poll() is None


def sim_running():
    return sim_process is not None and sim_process.poll() is None


def nav_running():
    return nav_process is not None and nav_process.poll() is None


def econtrol_running():
    return econtrol_process is not None and econtrol_process.poll() is None


def auto_demo_running():
    return auto_demo_thread is not None and auto_demo_thread.is_alive()


def pose_type_text(value):
    mapping = {
        0: "移动姿态",
        1: "进攻姿态",
        2: "防御姿态",
    }
    return mapping.get(value, "未知姿态")


def gimbal_mode_text(value):
    mapping = {
        1: "抬头到位",
        5: "低头到位",
    }
    return mapping.get(value, "未知")


def game_progress_text(value):
    mapping = {
        0: "未开始",
        1: "准备阶段",
        2: "自检阶段",
        3: "5秒倒计时",
        4: "比赛进行",
        5: "比赛结束",
    }
    return mapping.get(value, "未知阶段")


def data_age_text(age_sec):
    if age_sec is None:
        return "无数据"
    if age_sec < 1.0:
        return f"{age_sec * 1000:.0f} ms"
    return f"{age_sec:.1f} s"


def configured_points(cfg):
    points = []

    def add(name, pose_text):
        x, y, _ = pose_xyz(pose_text)
        points.append({"name": name, "x": x, "y": y})

    add("低头提前触发点", cfg["low_trigger"])
    add("低头等待点", cfg["low_wait"])
    add("抬头触发点", cfg["head_up_trigger"])
    add("第一进攻点", cfg["first_attack"])
    add("补给点", cfg["supply_pose"])
    for idx, row in enumerate(cfg.get("patrol_rows", [])):
        try:
            points.append({
                "name": f"巡逻点 {idx}",
                "x": float(row.get("pose_x", 0.0)),
                "y": float(row.get("pose_y", 0.0)),
            })
        except (TypeError, ValueError):
            continue
    return points


def nearest_configured_point(cfg, position):
    if position is None:
        return None
    px = position["x"]
    py = position["y"]
    nearest = None
    for point in configured_points(cfg):
        distance = math.hypot(px - point["x"], py - point["y"])
        if nearest is None or distance < nearest["distance"]:
            nearest = {
                "name": point["name"],
                "x": point["x"],
                "y": point["y"],
                "distance": distance,
            }
    return nearest


class RosStatusMonitor:
    def __init__(self):
        self.lock = threading.Lock()
        self.ready = False
        self.error = ""
        self.context = None
        self.node = None
        self.executor = None
        self.thread = None
        self.tf_buffer = None
        self.rclpy = None
        self.data = {
            "game_progress": None,
            "stage_remain_time": None,
            "team_color": None,
            "hp": None,
            "ammo": None,
            "game_stamp": None,
            "self_stamp": None,
            "pose_command": None,
            "pose_command_stamp": None,
            "pose_result": None,
            "pose_success": None,
            "pose_result_stamp": None,
            "gimbal_mode": None,
            "gimbal_stamp": None,
            "navigation_done": None,
            "navigation_done_stamp": None,
        }

    def start(self):
        if self.ready:
            return
        try:
            import rclpy
            from rclpy.executors import SingleThreadedExecutor
            from rm_decision_interfaces.msg import (
                BulletAllowance,
                GameStatus,
                RobotStatus,
                SentryPoseCommand,
                SentryPoseResult,
            )
            from rm_msgs.msg import GimbalStatus
            from std_msgs.msg import Bool
            import tf2_ros

            self.rclpy = rclpy
            self.context = rclpy.context.Context()
            rclpy.init(context=self.context)
            self.node = rclpy.create_node("bt_config_ui_status_monitor", context=self.context)
            self.executor = SingleThreadedExecutor(context=self.context)
            self.executor.add_node(self.node)
            qos = 10
            self.node.create_subscription(GameStatus, "/game_detection_status", self.game_cb, qos)
            self.node.create_subscription(GameStatus, "/game_status", self.game_cb, qos)
            self.node.create_subscription(RobotStatus, "/robot_status", self.robot_cb, qos)
            self.node.create_subscription(BulletAllowance, "/bullet_allowance", self.bullet_cb, qos)
            self.node.create_subscription(SentryPoseCommand, "/sentry/pose_command", self.pose_cmd_cb, qos)
            self.node.create_subscription(SentryPoseResult, "/sentry/pose_result", self.pose_result_cb, qos)
            self.node.create_subscription(GimbalStatus, "/gimbal_controller/gimbal_status", self.gimbal_cb, qos)
            self.node.create_subscription(Bool, "/navigation_done", self.nav_done_cb, qos)
            self.tf_buffer = tf2_ros.Buffer()
            self.tf_listener = tf2_ros.TransformListener(self.tf_buffer, self.node)
            self.thread = threading.Thread(target=self.executor.spin, daemon=True)
            self.thread.start()
            self.ready = True
            log_lines.append("状态监控：ROS 话题监控已启动")
        except Exception as exc:
            self.error = str(exc)
            log_lines.append(f"状态监控：启动失败，原因：{exc}")

    def stop(self):
        try:
            if self.executor is not None:
                self.executor.shutdown()
            if self.node is not None:
                self.node.destroy_node()
            if self.context is not None:
                self.rclpy.shutdown(context=self.context)
        except Exception:
            pass

    def update(self, **kwargs):
        now = time.monotonic()
        with self.lock:
            for key, value in kwargs.items():
                self.data[key] = value
            return now

    def game_cb(self, msg):
        now = time.monotonic()
        with self.lock:
            self.data["game_progress"] = int(msg.game_progress)
            self.data["stage_remain_time"] = int(msg.stage_remain_time)
            self.data["team_color"] = bool(msg.team_color)
            self.data["hp"] = int(msg.current_hp)
            self.data["ammo"] = int(msg.bullet_allowance_17mm)
            self.data["game_stamp"] = now
            self.data["self_stamp"] = now

    def robot_cb(self, msg):
        now = time.monotonic()
        with self.lock:
            self.data["team_color"] = bool(msg.team_color)
            self.data["hp"] = int(msg.current_hp)
            self.data["ammo"] = int(msg.bullet_allowance_17mm)
            self.data["self_stamp"] = now

    def bullet_cb(self, msg):
        now = time.monotonic()
        with self.lock:
            self.data["ammo"] = int(msg.bullet_allowance_17mm)
            self.data["self_stamp"] = now

    def pose_cmd_cb(self, msg):
        with self.lock:
            self.data["pose_command"] = int(msg.pose_type)
            self.data["pose_command_stamp"] = time.monotonic()

    def pose_result_cb(self, msg):
        with self.lock:
            self.data["pose_result"] = int(msg.pose_type)
            self.data["pose_success"] = bool(msg.pose_success)
            self.data["pose_result_stamp"] = time.monotonic()

    def gimbal_cb(self, msg):
        with self.lock:
            self.data["gimbal_mode"] = int(msg.gimbal_mode)
            self.data["gimbal_stamp"] = time.monotonic()

    def nav_done_cb(self, msg):
        with self.lock:
            self.data["navigation_done"] = bool(msg.data)
            self.data["navigation_done_stamp"] = time.monotonic()

    def tf_position(self):
        if self.tf_buffer is None or self.rclpy is None:
            return None
        try:
            tf_msg = self.tf_buffer.lookup_transform("map", "gimbal_yaw_fake", self.rclpy.time.Time())
            return {
                "x": float(tf_msg.transform.translation.x),
                "y": float(tf_msg.transform.translation.y),
                "z": float(tf_msg.transform.translation.z),
                "stamp": time.monotonic(),
            }
        except Exception:
            return None

    def snapshot(self):
        now = time.monotonic()
        with self.lock:
            data = dict(self.data)
        data["position"] = self.tf_position()
        data["monitor_ready"] = self.ready
        data["monitor_error"] = self.error
        data["game_age_sec"] = None if data["game_stamp"] is None else max(0.0, now - data["game_stamp"])
        data["self_age_sec"] = None if data["self_stamp"] is None else max(0.0, now - data["self_stamp"])
        data["pose_command_age_sec"] = (
            None if data["pose_command_stamp"] is None else max(0.0, now - data["pose_command_stamp"])
        )
        data["pose_result_age_sec"] = (
            None if data["pose_result_stamp"] is None else max(0.0, now - data["pose_result_stamp"])
        )
        data["gimbal_age_sec"] = None if data["gimbal_stamp"] is None else max(0.0, now - data["gimbal_stamp"])
        data["navigation_done_age_sec"] = (
            None if data["navigation_done_stamp"] is None else max(0.0, now - data["navigation_done_stamp"])
        )
        return data


def monitor_snapshot():
    cfg = load_config()
    raw = status_monitor.snapshot() if status_monitor is not None else {
        "monitor_ready": False,
        "monitor_error": "状态监控未启动",
        "position": None,
    }
    hp = raw.get("hp")
    ammo = raw.get("ammo")
    game_progress = raw.get("game_progress")
    enter_hp = int(float(cfg["enter_hp"]))
    enter_ammo = int(float(cfg["enter_ammo"]))
    recover_hp = int(float(cfg["recover_hp"]))
    recover_ammo = int(float(cfg["recover_ammo"]))
    tolerance = float(cfg.get("patrol_tolerance", 0.6))
    need_supply = hp is not None and ammo is not None and (hp <= enter_hp or ammo <= enter_ammo)
    recovered = hp is not None and ammo is not None and hp >= recover_hp and ammo >= recover_ammo
    nearest = nearest_configured_point(cfg, raw.get("position"))
    at_point = nearest is not None and nearest["distance"] <= tolerance

    if game_progress is None:
        phase = "等待裁判数据"
    elif game_progress != 4:
        phase = "等待比赛开始"
    elif need_supply:
        phase = "补给条件已触发"
    elif at_point:
        phase = f"位于{nearest['name']}"
    else:
        phase = "运行中/移动中"

    return {
        "monitor": {
            "ready": raw.get("monitor_ready", False),
            "error": raw.get("monitor_error", ""),
        },
        "processes": {
            "decision": process_running(),
            "referee_sim": sim_running(),
            "nav_sim": nav_running(),
            "econtrol_sim": econtrol_running(),
        },
        "auto_demo": {
            "running": auto_demo_running(),
            "message": auto_demo_message,
        },
        "phase": phase,
        "game": {
            "progress": game_progress,
            "progress_text": game_progress_text(game_progress) if game_progress is not None else "无数据",
            "stage_remain_time": raw.get("stage_remain_time"),
            "team": "蓝方" if raw.get("team_color") else "红方",
            "age": data_age_text(raw.get("game_age_sec")),
            "fresh": raw.get("game_age_sec") is not None and raw.get("game_age_sec") <= 1.0,
        },
        "self": {
            "hp": hp,
            "ammo": ammo,
            "age": data_age_text(raw.get("self_age_sec")),
            "fresh": raw.get("self_age_sec") is not None and raw.get("self_age_sec") <= 1.0,
        },
        "trigger": {
            "need_supply": need_supply,
            "recovered": recovered,
            "hp_text": f"{hp} <= {enter_hp}" if hp is not None else f"无数据 <= {enter_hp}",
            "ammo_text": f"{ammo} <= {enter_ammo}" if ammo is not None else f"无数据 <= {enter_ammo}",
            "recover_text": (
                f"HP {hp} >= {recover_hp} 且弹量 {ammo} >= {recover_ammo}"
                if hp is not None and ammo is not None else
                f"HP/弹量无数据，恢复阈值 {recover_hp}/{recover_ammo}"
            ),
        },
        "position": {
            "current": raw.get("position"),
            "nearest": nearest,
            "at_point": at_point,
            "tolerance": tolerance,
        },
        "pose": {
            "command": raw.get("pose_command"),
            "command_text": pose_type_text(raw.get("pose_command")) if raw.get("pose_command") is not None else "无命令",
            "command_age": data_age_text(raw.get("pose_command_age_sec")),
            "result": raw.get("pose_result"),
            "result_text": pose_type_text(raw.get("pose_result")) if raw.get("pose_result") is not None else "无回执",
            "success": raw.get("pose_success"),
            "result_age": data_age_text(raw.get("pose_result_age_sec")),
            "gimbal_mode": raw.get("gimbal_mode"),
            "gimbal_text": (
                gimbal_mode_text(raw.get("gimbal_mode")) if raw.get("gimbal_mode") is not None else "无反馈"
            ),
            "gimbal_age": data_age_text(raw.get("gimbal_age_sec")),
        },
        "navigation": {
            "done": raw.get("navigation_done"),
            "done_text": (
                "已到达/完成" if raw.get("navigation_done") is True else
                "执行中/未完成" if raw.get("navigation_done") is False else
                "无数据"
            ),
            "age": data_age_text(raw.get("navigation_done_age_sec")),
        },
        "config": {
            "points": configured_points(cfg),
            "xml_path": str(XML_PATH),
            "csv_path": str(CSV_PATH),
        },
    }


def ros_shell_command(command):
    workspace_setup = ""
    if WORKSPACE_SETUP is not None:
        workspace_setup = f"source {shlex.quote(str(WORKSPACE_SETUP))}; "

    setup = (
        "for setup in /opt/ros/*/setup.bash; do "
        'if [ -f "$setup" ]; then source "$setup"; break; fi; '
        "done; "
        f"{workspace_setup}"
        f"cd {shlex.quote(str(RUNTIME_ROOT))}; "
        f"exec {shlex.join(command)}"
    )
    return ["bash", "-lc", setup]


def read_process_output(proc):
    assert proc.stdout is not None
    for line in proc.stdout:
        text = line.rstrip()
        log_lines.append(text)
        print(text, flush=True)


def cleanup_existing_decision_nodes():
    for pattern in (
        "ros2 run rm_sentry_decision decision_node",
        "/rm_sentry_decision/decision_node",
    ):
        subprocess.run(
            ["pkill", "-f", pattern],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )


def start_decision(use_sim_time):
    global process
    if process_running():
        return "decision_node 已经在运行"
    cleanup_existing_decision_nodes()

    cmd = [
        "ros2",
        "run",
        "rm_sentry_decision",
        "decision_node",
        "--ros-args",
        "-p",
        f"xml_path:={XML_PATH}",
        "-p",
        f"use_sim_time:={use_sim_time}",
    ]
    start_info = [
        "正在启动 ROS 节点：rm_sentry_decision/decision_node",
        f"节点名：decision_node",
        f"行为树 XML：{XML_PATH}",
        f"use_sim_time：{use_sim_time}",
        "$ " + " ".join(cmd),
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)
    process = subprocess.Popen(
        ros_shell_command(cmd),
        cwd=str(RUNTIME_ROOT),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        preexec_fn=os.setsid,
    )
    thread = threading.Thread(target=read_process_output, args=(process,), daemon=True)
    thread.start()
    message = f"已启动 decision_node，PID={process.pid}"
    print(message, flush=True)
    log_lines.append(message)
    return message


def restart_decision(use_sim_time):
    stop_decision()
    return start_decision(use_sim_time)


def stop_decision():
    global process
    if not process_running():
        cleanup_existing_decision_nodes()
        process = None
        return "decision_node 当前没有运行"
    print(f"正在停止 ROS 节点：decision_node，PID={process.pid}", flush=True)
    os.killpg(os.getpgid(process.pid), signal.SIGTERM)
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(process.pid), signal.SIGKILL)
        process.wait(timeout=2)
    process = None
    message = "已停止 decision_node"
    print(message, flush=True)
    log_lines.append(message)
    return message


def cleanup_existing_simulators():
    for pattern in (str(SIM_SCRIPT), "referee_simulator.py"):
        subprocess.run(
            ["pkill", "-f", pattern],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )


def start_simulator(fields):
    global sim_process
    config = save_sim_config(fields)
    if sim_running():
        stop_simulator()
    else:
        cleanup_existing_simulators()

    cmd = [
        "python3",
        str(SIM_SCRIPT),
        "--game-progress",
        config["game_progress"],
        "--stage-remain-time",
        config["stage_remain_time"],
        "--team-color",
        config["team_color"],
        "--current-hp",
        config["current_hp"],
        "--ammo-17mm",
        config["ammo_17mm"],
        "--red-base-hp",
        config["red_base_hp"],
        "--blue-base-hp",
        config["blue_base_hp"],
        "--red-outpost-hp",
        config["red_outpost_hp"],
        "--blue-outpost-hp",
        config["blue_outpost_hp"],
        "--red-1-hp",
        config["red_1_hp"],
        "--red-3-hp",
        config["red_3_hp"],
        "--red-4-hp",
        config["red_4_hp"],
        "--blue-1-hp",
        config["blue_1_hp"],
        "--blue-3-hp",
        config["blue_3_hp"],
        "--blue-4-hp",
        config["blue_4_hp"],
    ]
    start_info = [
        "正在启动模拟裁判系统：sentry_referee_simulator",
        "发布话题：/game_detection_status /game_status /robot_status /bullet_allowance /all_robot_hp",
        f"比赛阶段：{config['game_progress']}，剩余时间：{config['stage_remain_time']}，HP：{config['current_hp']}，弹量：{config['ammo_17mm']}",
        "$ " + " ".join(cmd),
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)
    sim_process = subprocess.Popen(
        ros_shell_command(cmd),
        cwd=str(RUNTIME_ROOT),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        preexec_fn=os.setsid,
    )
    thread = threading.Thread(target=read_process_output, args=(sim_process,), daemon=True)
    thread.start()
    message = f"已启动模拟裁判，PID={sim_process.pid}"
    print(message, flush=True)
    log_lines.append(message)
    return message


def stop_simulator():
    global sim_process
    if not sim_running():
        cleanup_existing_simulators()
        sim_process = None
        return "模拟裁判当前没有运行"
    print(f"正在停止模拟裁判，PID={sim_process.pid}", flush=True)
    os.killpg(os.getpgid(sim_process.pid), signal.SIGTERM)
    try:
        sim_process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(sim_process.pid), signal.SIGKILL)
        sim_process.wait(timeout=2)
    sim_process = None
    message = "已停止模拟裁判"
    print(message, flush=True)
    log_lines.append(message)
    return message


def cleanup_existing_nav_simulators():
    for pattern in (str(NAV_SIM_SCRIPT), "nav_pose_simulator.py"):
        subprocess.run(
            ["pkill", "-f", pattern],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )


def start_nav_simulator():
    global nav_process
    if nav_running():
        return "导航/位置模拟器已经在运行"
    cleanup_existing_nav_simulators()

    cmd = ["python3", str(NAV_SIM_SCRIPT)]
    start_info = [
        "正在启动导航/位置模拟器：sentry_nav_pose_simulator",
        "提供 action：/navigate_to_pose /navigate_through_poses",
        "发布 TF：map -> gimbal_yaw_fake / base_link",
        "$ " + " ".join(cmd),
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)
    nav_process = subprocess.Popen(
        ros_shell_command(cmd),
        cwd=str(RUNTIME_ROOT),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        preexec_fn=os.setsid,
    )
    thread = threading.Thread(target=read_process_output, args=(nav_process,), daemon=True)
    thread.start()
    message = f"已启动导航/位置模拟器，PID={nav_process.pid}"
    print(message, flush=True)
    log_lines.append(message)
    return message


def stop_nav_simulator():
    global nav_process
    if not nav_running():
        cleanup_existing_nav_simulators()
        nav_process = None
        return "导航/位置模拟器当前没有运行"
    print(f"正在停止导航/位置模拟器，PID={nav_process.pid}", flush=True)
    os.killpg(os.getpgid(nav_process.pid), signal.SIGTERM)
    try:
        nav_process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(nav_process.pid), signal.SIGKILL)
        nav_process.wait(timeout=2)
    nav_process = None
    message = "已停止导航/位置模拟器"
    print(message, flush=True)
    log_lines.append(message)
    return message


def cleanup_existing_econtrol_simulators():
    for pattern in (str(ECONTROL_SIM_SCRIPT), "electrical_feedback_simulator.py"):
        subprocess.run(
            ["pkill", "-f", pattern],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )


def start_econtrol_simulator():
    global econtrol_process
    if econtrol_running():
        return "电控反馈模拟器已经在运行"
    cleanup_existing_econtrol_simulators()

    cmd = ["python3", str(ECONTROL_SIM_SCRIPT)]
    start_info = [
        "正在启动电控反馈模拟器：sentry_electrical_feedback_simulator",
        "应答话题：/sentry/pose_command -> /sentry/pose_result",
        "云台反馈：/sentry/gimbal_lower_command -> /gimbal_controller/gimbal_status",
        "$ " + " ".join(cmd),
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)
    econtrol_process = subprocess.Popen(
        ros_shell_command(cmd),
        cwd=str(RUNTIME_ROOT),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        preexec_fn=os.setsid,
    )
    thread = threading.Thread(target=read_process_output, args=(econtrol_process,), daemon=True)
    thread.start()
    message = f"已启动电控反馈模拟器，PID={econtrol_process.pid}"
    print(message, flush=True)
    log_lines.append(message)
    return message


def stop_econtrol_simulator():
    global econtrol_process
    if not econtrol_running():
        cleanup_existing_econtrol_simulators()
        econtrol_process = None
        return "电控反馈模拟器当前没有运行"
    print(f"正在停止电控反馈模拟器，PID={econtrol_process.pid}", flush=True)
    os.killpg(os.getpgid(econtrol_process.pid), signal.SIGTERM)
    try:
        econtrol_process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(econtrol_process.pid), signal.SIGKILL)
        econtrol_process.wait(timeout=2)
    econtrol_process = None
    message = "已停止电控反馈模拟器"
    print(message, flush=True)
    log_lines.append(message)
    return message


def demo_sim_fields(current_hp, ammo_17mm=400):
    config = load_sim_config()
    config.update({
        "game_progress": "4",
        "stage_remain_time": "700",
        "current_hp": str(current_hp),
        "ammo_17mm": str(ammo_17mm),
    })
    return {f"sim_{key}": [str(value)] for key, value in config.items()}


def set_auto_demo_message(message):
    global auto_demo_message, status_message
    auto_demo_message = message
    status_message = message
    log_line = f"自动全流程模拟：{message}"
    log_lines.append(log_line)
    print(log_line, flush=True)


def run_auto_demo():
    global auto_demo_message
    try:
        set_auto_demo_message("1/5 启动导航、电控、裁判，并重启行为树")
        start_nav_simulator()
        start_econtrol_simulator()
        start_simulator(demo_sim_fields(current_hp=400, ammo_17mm=400))
        time.sleep(1.0)
        restart_decision("False")

        set_auto_demo_message("2/5 正常开局并进入第一进攻点巡逻，观察低头/抬头/巡逻点切换")
        time.sleep(14.0)

        set_auto_demo_message("3/5 将本车 HP 降到 150，触发低血回补给")
        start_simulator(demo_sim_fields(current_hp=150, ammo_17mm=400))
        time.sleep(9.0)

        set_auto_demo_message("4/5 将本车 HP 恢复到 400，模拟补给完成后返回第一进攻点")
        start_simulator(demo_sim_fields(current_hp=400, ammo_17mm=400))
        time.sleep(12.0)

        set_auto_demo_message("5/5 演示完成：HP 已恢复，并回到第一进攻点巡逻逻辑，可继续手动测试")
    except Exception as exc:
        set_auto_demo_message(f"演示失败：{exc}")
    finally:
        auto_demo_message = f"{auto_demo_message}（已结束）"


def start_auto_demo():
    global auto_demo_thread, status_message
    if auto_demo_running():
        return "自动全流程模拟正在运行"
    auto_demo_thread = threading.Thread(target=run_auto_demo, daemon=True)
    auto_demo_thread.start()
    status_message = "自动全流程模拟已启动，约 35 秒完成"
    return status_message


def publish_robot_pose(x, y, label):
    start_info = [
        f"正在模拟机器人到达：{label}",
        f"坐标：x={x}, y={y}",
        "优先发送到导航/位置模拟器控制话题：/sentry/nav_pose_simulator/set_pose",
        "同时短时发布 TF：map -> gimbal_yaw_fake / base_link",
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)

    node = None
    context = None
    try:
        import rclpy
        from geometry_msgs.msg import PoseStamped, TransformStamped
        from tf2_msgs.msg import TFMessage

        context = rclpy.context.Context()
        rclpy.init(context=context)
        node = rclpy.create_node("bt_config_ui_pose_pub", context=context)
        pub = node.create_publisher(TFMessage, "/tf", 10)
        pose_pub = node.create_publisher(PoseStamped, "/sentry/nav_pose_simulator/set_pose", 10)
        pose_msg = PoseStamped()
        pose_msg.header.frame_id = "map"
        pose_msg.pose.position.x = float(x)
        pose_msg.pose.position.y = float(y)
        pose_msg.pose.position.z = 0.0
        pose_msg.pose.orientation.w = 1.0
        time.sleep(0.2)
        for _ in range(80):
            now = node.get_clock().now().to_msg()
            pose_msg.header.stamp = now
            pose_pub.publish(pose_msg)
            msg = TFMessage()
            for child in ("gimbal_yaw_fake", "base_link"):
                tf = TransformStamped()
                tf.header.stamp = now
                tf.header.frame_id = "map"
                tf.child_frame_id = child
                tf.transform.translation.x = float(x)
                tf.transform.translation.y = float(y)
                tf.transform.translation.z = 0.0
                tf.transform.rotation.w = 1.0
                msg.transforms.append(tf)
            pub.publish(msg)
            time.sleep(0.05)
    except Exception as exc:
        message = f"模拟到达失败：{label}，原因：{exc}"
        print(message, flush=True)
        log_lines.append(message)
        return message
    finally:
        try:
            if node is not None:
                node.destroy_node()
            if context is not None:
                rclpy.shutdown(context=context)
        except Exception:
            pass

    message = f"已模拟机器人到达：{label}（x={x}, y={y}）"
    print(message, flush=True)
    log_lines.append(message)
    return message


def publish_config_pose(config_key, label):
    cfg = load_config()
    x, y, _ = pose_xyz(cfg[config_key])
    return publish_robot_pose(x, y, label)


def publish_gimbal_state(lower):
    mode = 5 if lower else 1
    label = "低头成功" if lower else "抬头成功"
    start_info = [
        f"正在发布云台电控反馈：{label}",
        "话题：/gimbal_controller/gimbal_status",
        "同时发送模拟电控命令：/sentry/gimbal_lower_command",
        "消息类型：rm_msgs/msg/GimbalStatus",
        f"内容：gimbal_mode={mode}",
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)

    node = None
    context = None
    try:
        import rclpy
        from rm_decision_interfaces.msg import SentryPoseCommand
        from rm_msgs.msg import GimbalStatus

        context = rclpy.context.Context()
        rclpy.init(context=context)
        node = rclpy.create_node("bt_config_ui_gimbal_state_pub", context=context)
        pub = node.create_publisher(GimbalStatus, "/gimbal_controller/gimbal_status", 10)
        command_pub = node.create_publisher(
            SentryPoseCommand, "/sentry/gimbal_lower_command", 10)

        msg = GimbalStatus()
        msg.gimbal_mode = int(mode)
        command = SentryPoseCommand()
        command.pose_type = 0
        command.fold_type = 1 if lower else 0

        # 发布多次，给 DDS 发现订阅者留一点时间，避免一次性发布被错过。
        time.sleep(0.2)
        for _ in range(60):
            command_pub.publish(command)
            pub.publish(msg)
            time.sleep(0.05)
    except Exception as exc:
        message = f"云台电控反馈发布失败：{label}，原因：{exc}"
        print(message, flush=True)
        log_lines.append(message)
        return message
    finally:
        try:
            if node is not None:
                node.destroy_node()
            if context is not None:
                rclpy.shutdown(context=context)
        except Exception:
            pass

    message = f"已发布云台电控反馈：{label}（gimbal_mode={mode}）"
    print(message, flush=True)
    log_lines.append(message)
    return message


def input_box(name, label, value, step="0.01"):
    return (
        f'<label><span>{html.escape(label)}</span>'
        f'<input name="{html.escape(name)}" type="number" step="{step}" '
        f'value="{html.escape(str(value))}"></label>'
    )


def select_box(name, label, value, options):
    choices = []
    for option_value, option_label in options:
        selected = " selected" if option_value == value else ""
        choices.append(
            f'<option value="{html.escape(option_value)}"{selected}>{html.escape(option_label)}</option>')
    return (
        f'<label><span>{html.escape(label)}</span>'
        f'<select name="{html.escape(name)}">{"".join(choices)}</select></label>'
    )


def render_monitor_page():
    return """<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>行为树状态仪表盘</title>
  <style>
    :root { --bg:#f5f7fb; --panel:#fff; --ink:#182230; --muted:#64748b; --line:#d8e0ea; --good:#0f8a4b; --warn:#b7791f; --bad:#c2410c; --accent:#2563eb; }
    * { box-sizing:border-box; }
    body { margin:0; background:var(--bg); color:var(--ink); font-family:system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif; }
    header { height:56px; display:flex; align-items:center; justify-content:space-between; padding:0 16px; border-bottom:1px solid var(--line); background:#fff; }
    h1 { margin:0; font-size:18px; }
    a { color:var(--accent); text-decoration:none; }
    main { padding:14px; display:grid; gap:12px; grid-template-columns:repeat(auto-fit, minmax(260px, 1fr)); }
    section { background:var(--panel); border:1px solid var(--line); border-radius:8px; padding:14px; min-height:132px; }
    h2 { margin:0 0 10px; font-size:15px; }
    .hero { grid-column:1 / -1; display:grid; gap:8px; min-height:auto; }
    .phase { font-size:28px; font-weight:700; }
    .row { display:flex; justify-content:space-between; gap:16px; border-top:1px solid #edf2f7; padding-top:8px; margin-top:8px; font-size:14px; }
    .row:first-of-type { border-top:0; padding-top:0; margin-top:0; }
    .label { color:var(--muted); }
    .value { text-align:right; font-weight:600; }
    .pill { display:inline-flex; align-items:center; height:24px; border-radius:999px; padding:0 9px; font-size:12px; font-weight:700; background:#e8eef8; color:#243b64; }
    .good { background:#dff7ea; color:var(--good); }
    .warn { background:#fff4d6; color:var(--warn); }
    .bad { background:#ffe8de; color:var(--bad); }
    .muted { color:var(--muted); }
    .mono { font-family:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,monospace; }
  </style>
</head>
<body>
  <header>
    <h1>行为树状态仪表盘</h1>
    <a href="/">返回配置页</a>
  </header>
  <main>
    <section class="hero">
      <div class="muted">当前阶段</div>
      <div id="phase" class="phase">读取中...</div>
      <div id="processes"></div>
    </section>
    <section>
      <h2>比赛/裁判</h2>
      <div class="row"><span class="label">比赛阶段</span><span id="game" class="value"></span></div>
      <div class="row"><span class="label">剩余时间</span><span id="remain" class="value"></span></div>
      <div class="row"><span class="label">数据年龄</span><span id="game_age" class="value"></span></div>
    </section>
    <section>
      <h2>触发条件</h2>
      <div class="row"><span class="label">HP 回补给</span><span id="hp_trigger" class="value"></span></div>
      <div class="row"><span class="label">弹量回补给</span><span id="ammo_trigger" class="value"></span></div>
      <div class="row"><span class="label">补给完成</span><span id="recover" class="value"></span></div>
    </section>
    <section>
      <h2>位置/点位</h2>
      <div class="row"><span class="label">当前坐标</span><span id="position" class="value mono"></span></div>
      <div class="row"><span class="label">最近点</span><span id="nearest" class="value"></span></div>
      <div class="row"><span class="label">到点判定</span><span id="at_point" class="value"></span></div>
    </section>
    <section>
      <h2>姿态/云台</h2>
      <div class="row"><span class="label">姿态命令</span><span id="pose_cmd" class="value"></span></div>
      <div class="row"><span class="label">姿态回执</span><span id="pose_result" class="value"></span></div>
      <div class="row"><span class="label">云台状态</span><span id="gimbal" class="value"></span></div>
    </section>
    <section>
      <h2>导航</h2>
      <div class="row"><span class="label">导航完成</span><span id="nav_done" class="value"></span></div>
      <div class="row"><span class="label">导航模拟器</span><span id="nav_sim" class="value"></span></div>
      <div class="row"><span class="label">电控模拟器</span><span id="econtrol_sim" class="value"></span></div>
    </section>
  </main>
  <script>
    function pill(text, cls) { return `<span class="pill ${cls || ''}">${text}</span>`; }
    function yesNo(value) { return value ? pill('运行中', 'good') : pill('未运行', 'bad'); }
    function set(id, html) { document.getElementById(id).innerHTML = html; }
    function fmtPos(pos) {
      if (!pos) return '无 TF';
      return `${pos.x.toFixed(2)}, ${pos.y.toFixed(2)}`;
    }
    async function refresh() {
      try {
        const s = await fetch('/status', {cache:'no-store'}).then(r => r.json());
        set('phase', s.phase);
        set('processes', [
          `决策 ${yesNo(s.processes.decision)}`,
          `裁判 ${yesNo(s.processes.referee_sim)}`,
          `导航 ${yesNo(s.processes.nav_sim)}`,
          `电控 ${yesNo(s.processes.econtrol_sim)}`
        ].join(' '));
        set('game', `${s.game.progress_text} (${s.game.progress ?? '无'})`);
        set('remain', s.game.stage_remain_time ?? '无数据');
        set('game_age', s.game.fresh ? pill(s.game.age, 'good') : pill(s.game.age, 'warn'));
        set('hp_trigger', `${s.self.hp ?? '无'} / ${s.trigger.hp_text} ${s.trigger.need_supply ? pill('触发', 'bad') : pill('未触发', 'good')}`);
        set('ammo_trigger', `${s.self.ammo ?? '无'} / ${s.trigger.ammo_text} ${s.trigger.need_supply ? pill('触发', 'bad') : pill('未触发', 'good')}`);
        set('recover', `${s.trigger.recover_text} ${s.trigger.recovered ? pill('满足', 'good') : pill('未满足', 'warn')}`);
        set('position', fmtPos(s.position.current));
        if (s.position.nearest) {
          set('nearest', `${s.position.nearest.name}，${s.position.nearest.distance.toFixed(2)} m`);
        } else {
          set('nearest', '无数据');
        }
        set('at_point', s.position.at_point ? pill(`在点内 <= ${s.position.tolerance} m`, 'good') : pill(`不在点内 > ${s.position.tolerance} m`, 'warn'));
        set('pose_cmd', `${s.pose.command_text}，${s.pose.command_age}`);
        const resultCls = s.pose.success === true ? 'good' : (s.pose.success === false ? 'bad' : 'warn');
        set('pose_result', `${s.pose.result_text} ${pill(s.pose.success === true ? '成功' : s.pose.success === false ? '失败' : '无回执', resultCls)} ${s.pose.result_age}`);
        set('gimbal', `${s.pose.gimbal_text}，${s.pose.gimbal_age}`);
        set('nav_done', `${s.navigation.done_text}，${s.navigation.age}`);
        set('nav_sim', yesNo(s.processes.nav_sim));
        set('econtrol_sim', yesNo(s.processes.econtrol_sim));
      } catch (err) {
        set('phase', `状态读取失败：${err}`);
      }
    }
    refresh();
    setInterval(refresh, 1000);
  </script>
</body>
</html>"""


def render_inline_status_panel():
    return """<section class="live-status">
      <div class="live-head">
        <h2>当前状态</h2>
        <span id="inline_updated" class="status">读取中...</span>
      </div>
      <div id="inline_phase" class="phase-box">读取中...</div>
      <div class="status-grid">
        <div class="status-card">
          <span>进程</span>
          <strong id="inline_processes">读取中...</strong>
        </div>
        <div class="status-card">
          <span>当前位置</span>
          <strong id="inline_position">读取中...</strong>
        </div>
        <div class="status-card">
          <span>最近点</span>
          <strong id="inline_nearest">读取中...</strong>
        </div>
        <div class="status-card">
          <span>补给触发</span>
          <strong id="inline_supply">读取中...</strong>
        </div>
        <div class="status-card">
          <span>姿态/云台</span>
          <strong id="inline_pose">读取中...</strong>
        </div>
        <div class="status-card">
          <span>导航</span>
          <strong id="inline_nav">读取中...</strong>
        </div>
        <div class="status-card">
          <span>自动流程</span>
          <strong id="inline_auto_demo">未运行</strong>
        </div>
      </div>
    </section>
    <script>
      function inlineSet(id, text) {
        const el = document.getElementById(id);
        if (el) el.textContent = text;
      }
      function inlineYes(value) {
        return value ? '运行' : '停止';
      }
      function inlinePos(pos) {
        if (!pos || pos.x === undefined || pos.y === undefined) return '无 TF';
        return `(${pos.x.toFixed(2)}, ${pos.y.toFixed(2)})`;
      }
      async function refreshInlineStatus() {
        try {
          const s = await fetch('/status', {cache: 'no-store'}).then(r => r.json());
          inlineSet('inline_phase', s.phase || '无状态');
          inlineSet('inline_processes',
            `决策${inlineYes(s.processes.decision)} / 裁判${inlineYes(s.processes.referee_sim)} / 导航${inlineYes(s.processes.nav_sim)} / 电控${inlineYes(s.processes.econtrol_sim)}`);
          inlineSet('inline_position', inlinePos(s.position.current));
          inlineSet('inline_nearest', s.position.nearest
            ? `${s.position.nearest.name}，${s.position.nearest.distance.toFixed(2)} m`
            : '无最近点');
          inlineSet('inline_supply',
            `HP ${s.self.hp ?? '无'}，弹 ${s.self.ammo ?? '无'}，${s.trigger.need_supply ? '已触发回补给' : '未触发'}`);
          inlineSet('inline_pose',
            `${s.pose.command_text} / ${s.pose.gimbal_text}`);
          inlineSet('inline_nav',
            `${s.navigation.done_text}，${s.navigation.age}`);
          inlineSet('inline_auto_demo',
            `${s.auto_demo.running ? '运行中' : '停止'}：${s.auto_demo.message}`);
          inlineSet('inline_updated', `已更新 ${new Date().toLocaleTimeString()}`);
        } catch (err) {
          inlineSet('inline_phase', `状态读取失败：${err}`);
          inlineSet('inline_updated', '读取失败');
        }
      }
      refreshInlineStatus();
      setInterval(refreshInlineStatus, 500);
    </script>"""


def render_page():
    cfg = load_config()
    sim_cfg = load_sim_config()
    low_x, low_y, _ = pose_xyz(cfg["low_trigger"])
    low_wait_x, low_wait_y, _ = pose_xyz(cfg["low_wait"])
    up_x, up_y, _ = pose_xyz(cfg["head_up_trigger"])
    first_x, first_y, _ = pose_xyz(cfg["first_attack"])
    supply_x, supply_y, _ = pose_xyz(cfg["supply_pose"])
    rows = cfg["patrol_rows"]
    while len(rows) < 2:
        rows.append({"pose_x": "0", "pose_y": "0", "wait_sec": "1.0"})

    running = process_running()
    run_text = "运行中" if running else "未运行"
    sim_text = "运行中" if sim_running() else "未运行"
    nav_text = "运行中" if nav_running() else "未运行"
    econtrol_text = "运行中" if econtrol_running() else "未运行"
    logs = html.escape("\n".join(log_lines))

    return f"""<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>哨兵行为树配置</title>
  <style>
    :root {{ color-scheme: light; --bg:#f7f8fa; --panel:#fff; --ink:#17202a; --muted:#607080; --line:#d8dee6; --accent:#1769aa; --danger:#b3261e; }}
    body {{ margin:0; font-family: system-ui, -apple-system, "Segoe UI", sans-serif; background:var(--bg); color:var(--ink); }}
    header {{ padding:18px 24px; border-bottom:1px solid var(--line); background:#fff; display:flex; gap:16px; align-items:center; justify-content:space-between; }}
    h1 {{ font-size:20px; margin:0; }}
    main {{ max-width:1120px; margin:0 auto; padding:22px; display:grid; gap:16px; }}
    section {{ background:var(--panel); border:1px solid var(--line); border-radius:8px; padding:16px; }}
    h2 {{ margin:0 0 12px; font-size:16px; }}
    .grid {{ display:grid; grid-template-columns:repeat(auto-fit, minmax(210px, 1fr)); gap:12px; }}
    label {{ display:grid; gap:6px; font-size:13px; color:var(--muted); }}
    input, select {{ height:36px; border:1px solid var(--line); border-radius:6px; padding:0 10px; font-size:14px; color:var(--ink); background:#fff; }}
    button {{ height:38px; border:1px solid var(--accent); border-radius:6px; padding:0 14px; background:var(--accent); color:#fff; font-size:14px; cursor:pointer; }}
    button.secondary {{ background:#fff; color:var(--accent); }}
    button.danger {{ border-color:var(--danger); background:var(--danger); }}
    .actions {{ display:flex; gap:10px; flex-wrap:wrap; align-items:center; }}
    .status {{ color:var(--muted); font-size:14px; }}
    .paths {{ font-size:12px; color:var(--muted); line-height:1.7; word-break:break-all; }}
    pre {{ margin:0; max-height:260px; overflow:auto; background:#101820; color:#d6e6ff; padding:12px; border-radius:6px; font-size:12px; line-height:1.5; }}
    .live-status {{ display:grid; gap:12px; }}
    .live-head {{ display:flex; justify-content:space-between; align-items:center; gap:12px; }}
    .live-head h2 {{ margin:0; }}
    .phase-box {{ min-height:42px; display:flex; align-items:center; border:1px solid #cfe0f5; border-radius:6px; background:#eef6ff; color:#124d86; padding:0 12px; font-size:22px; font-weight:700; }}
    .status-grid {{ display:grid; grid-template-columns:repeat(auto-fit, minmax(230px, 1fr)); gap:10px; }}
    .status-card {{ min-height:68px; display:grid; gap:6px; border:1px solid var(--line); border-radius:6px; padding:10px; background:#fbfcfe; }}
    .status-card span {{ color:var(--muted); font-size:12px; }}
    .status-card strong {{ font-size:14px; line-height:1.35; }}
  </style>
</head>
<body>
  <header>
    <div>
      <h1>第一进攻点巡逻补给行为树</h1>
      <div class="status">状态：{run_text}　{html.escape(status_message)}　<a href="/monitor" target="_blank">打开状态仪表盘</a></div>
    </div>
    <form method="post" action="/start" class="actions">
      <select name="use_sim_time">
        <option value="True">use_sim_time=True</option>
        <option value="False">use_sim_time=False</option>
      </select>
      <button type="submit">启动</button>
      <button formaction="/stop" class="danger" type="submit">停止</button>
    </form>
  </header>
  <main>
    {render_inline_status_panel()}
    <section>
      <h2>自动化整套模拟</h2>
      <div class="status">一键启动导航模拟、电控反馈模拟、裁判模拟和行为树；随后自动演示开局巡逻、HP=150 回补给、HP=400 补给完成返回巡逻，全程约 35 秒。</div>
      <form method="post" action="/auto_demo_full" class="actions" style="margin-top:12px">
        <button type="submit">一键跑完整流程</button>
      </form>
    </section>
    <form method="post" action="/save">
      <section>
        <h2>开局路径</h2>
        <div class="grid">
          {input_box("low_trigger_x", "低头提前触发点 X", low_x)}
          {input_box("low_trigger_y", "低头提前触发点 Y", low_y)}
          {input_box("low_wait_x", "低头等待点 X", low_wait_x)}
          {input_box("low_wait_y", "低头等待点 Y", low_wait_y)}
          {input_box("head_up_x", "抬头触发点 X", up_x)}
          {input_box("head_up_y", "抬头触发点 Y", up_y)}
          {input_box("first_attack_x", "第一进攻点 X", first_x)}
          {input_box("first_attack_y", "第一进攻点 Y", first_y)}
        </div>
      </section>
      <section>
        <h2>补给逻辑</h2>
        <div class="grid">
          {input_box("supply_x", "补给点 X", supply_x)}
          {input_box("supply_y", "补给点 Y", supply_y)}
          {input_box("enter_hp", "回补给 HP 阈值", cfg["enter_hp"], "1")}
          {input_box("enter_ammo", "回补给弹量阈值", cfg["enter_ammo"], "1")}
          {input_box("recover_hp", "补给完成 HP", cfg["recover_hp"], "1")}
          {input_box("recover_ammo", "补给完成弹量", cfg["recover_ammo"], "1")}
        </div>
      </section>
      <section>
        <h2>巡逻</h2>
        <div class="grid">
          {input_box("patrol_speed", "巡逻速度 m/s", cfg["patrol_speed"])}
          {input_box("patrol_tolerance", "到点判定半径 m", cfg["patrol_tolerance"])}
          {input_box("patrol_0_x", "巡逻点 0 X", rows[0].get("pose_x", "0"))}
          {input_box("patrol_0_y", "巡逻点 0 Y", rows[0].get("pose_y", "0"))}
          {input_box("patrol_0_wait", "巡逻点 0 停留 s", rows[0].get("wait_sec", "1.0"))}
          {input_box("patrol_1_x", "巡逻点 1 X", rows[1].get("pose_x", "0"))}
          {input_box("patrol_1_y", "巡逻点 1 Y", rows[1].get("pose_y", "0"))}
          {input_box("patrol_1_wait", "巡逻点 1 停留 s", rows[1].get("wait_sec", "1.0"))}
        </div>
      </section>
      <section class="actions">
        <button type="submit">保存到 XML/CSV</button>
        <button formaction="/save_restart" type="submit" class="secondary">保存配置并重启行为树</button>
        <span class="status">保存后永久写入配置文件；已运行的节点需要重启才会读取新配置。</span>
      </section>
    </form>
    <form method="post" action="/sim_start">
      <section>
        <h2>模拟裁判系统</h2>
        <div class="status">状态：{sim_text}。用于发布比赛阶段、血量、弹量等裁判数据。</div>
        <div class="grid" style="margin-top:12px">
          {input_box("sim_game_progress", "比赛阶段 game_progress", sim_cfg["game_progress"], "1")}
          {input_box("sim_stage_remain_time", "剩余时间 s", sim_cfg["stage_remain_time"], "1")}
          {select_box("sim_team_color", "我方颜色", sim_cfg["team_color"], [("red", "红方"), ("blue", "蓝方")])}
          {input_box("sim_current_hp", "本车 HP", sim_cfg["current_hp"], "1")}
          {input_box("sim_ammo_17mm", "17mm 弹量", sim_cfg["ammo_17mm"], "1")}
          {input_box("sim_red_base_hp", "红方基地 HP", sim_cfg["red_base_hp"], "1")}
          {input_box("sim_blue_base_hp", "蓝方基地 HP", sim_cfg["blue_base_hp"], "1")}
          {input_box("sim_red_outpost_hp", "红方前哨站 HP", sim_cfg["red_outpost_hp"], "1")}
          {input_box("sim_blue_outpost_hp", "蓝方前哨站 HP", sim_cfg["blue_outpost_hp"], "1")}
          {input_box("sim_red_1_hp", "红方 1 号 HP", sim_cfg["red_1_hp"], "1")}
          {input_box("sim_red_3_hp", "红方 3 号 HP", sim_cfg["red_3_hp"], "1")}
          {input_box("sim_red_4_hp", "红方 4 号 HP", sim_cfg["red_4_hp"], "1")}
          {input_box("sim_blue_1_hp", "蓝方 1 号 HP", sim_cfg["blue_1_hp"], "1")}
          {input_box("sim_blue_3_hp", "蓝方 3 号 HP", sim_cfg["blue_3_hp"], "1")}
          {input_box("sim_blue_4_hp", "蓝方 4 号 HP", sim_cfg["blue_4_hp"], "1")}
        </div>
        <div class="actions" style="margin-top:12px">
          <button type="submit">应用并启动/重启模拟裁判</button>
          <button formaction="/sim_start_restart_decision" type="submit" class="secondary">应用裁判并重启整套验证</button>
          <button formaction="/sim_save" type="submit" class="secondary">仅保存参数</button>
          <button formaction="/sim_stop" type="submit" class="danger">停止模拟裁判</button>
        </div>
      </section>
    </form>
    <section>
      <h2>电控反馈自动模拟</h2>
      <div class="status">状态：{econtrol_text}。无电控时用于自动应答姿态切换，并根据低头/抬头命令持续发布云台状态。</div>
      <form method="post" action="/econtrol_start" class="actions" style="margin-top:12px">
        <button type="submit">启动电控反馈模拟器</button>
        <button formaction="/econtrol_stop" type="submit" class="danger">停止电控反馈模拟器</button>
      </form>
    </section>
    <section>
      <h2>云台电控反馈模拟</h2>
      <div class="status">用于手动模拟电控在 /gimbal_controller/gimbal_status 上反馈云台模式。1 视为抬起到位，5 视为折叠到位。</div>
      <form method="post" action="/gimbal_lower_ok" class="actions" style="margin-top:12px">
        <button type="submit">发布低头成功 gimbal_mode=5</button>
        <button formaction="/gimbal_raise_ok" type="submit" class="secondary">发布抬头成功 gimbal_mode=1</button>
      </form>
    </section>
    <section>
      <h2>导航/位置模拟</h2>
      <div class="status">状态：{nav_text}。启动后会模拟 Nav2 action server，并持续发布机器人 TF 位置。</div>
      <form method="post" action="/nav_start" class="actions" style="margin-top:12px">
        <button type="submit">启动导航/位置模拟器</button>
        <button formaction="/nav_stop" type="submit" class="danger">停止导航/位置模拟器</button>
      </form>
      <form method="post" action="/pose_low_trigger" class="actions" style="margin-top:12px">
        <button type="submit">到低头提前触发点</button>
        <button formaction="/pose_low_wait" type="submit" class="secondary">到低头等待点</button>
        <button formaction="/pose_head_up" type="submit" class="secondary">到抬头触发点</button>
        <button formaction="/pose_first_attack" type="submit" class="secondary">到第一进攻点</button>
        <button formaction="/pose_supply" type="submit" class="secondary">到补给点</button>
      </form>
    </section>
    <section>
      <h2>配置文件</h2>
      <div class="paths">XML：{html.escape(cfg["xml_path"])}<br>CSV：{html.escape(cfg["csv_path"])}<br>模拟裁判参数：{html.escape(str(SIM_CONFIG_PATH))}</div>
    </section>
    <section>
      <h2>运行日志</h2>
      <pre>{logs}</pre>
    </section>
  </main>
</body>
</html>"""


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        path = urllib.parse.urlparse(self.path).path
        if path == "/":
            self.respond(render_page())
        elif path == "/monitor":
            self.respond(render_monitor_page())
        elif path == "/status":
            self.respond_json(monitor_snapshot())
        else:
            self.send_error(404)

    def do_POST(self):
        global status_message
        length = int(self.headers.get("Content-Length", "0"))
        body = self.rfile.read(length).decode("utf-8")
        fields = urllib.parse.parse_qs(body)
        try:
            if self.path == "/save":
                update_config(fields)
                status_message = "配置已保存"
            elif self.path == "/save_restart":
                update_config(fields)
                status_message = restart_decision(form_value(fields, "use_sim_time", "True"))
            elif self.path == "/start":
                status_message = start_decision(form_value(fields, "use_sim_time", "True"))
            elif self.path == "/stop":
                status_message = stop_decision()
            elif self.path == "/sim_start":
                status_message = start_simulator(fields)
            elif self.path == "/sim_start_restart_decision":
                nav_message = start_nav_simulator()
                econtrol_message = start_econtrol_simulator()
                sim_message = start_simulator(fields)
                decision_message = restart_decision("False")
                status_message = f"{nav_message}；{econtrol_message}；{sim_message}；{decision_message}"
            elif self.path == "/auto_demo_full":
                status_message = start_auto_demo()
            elif self.path == "/sim_save":
                save_sim_config(fields)
                status_message = "模拟裁判参数已保存"
            elif self.path == "/sim_stop":
                status_message = stop_simulator()
            elif self.path == "/econtrol_start":
                status_message = start_econtrol_simulator()
            elif self.path == "/econtrol_stop":
                status_message = stop_econtrol_simulator()
            elif self.path == "/gimbal_lower_ok":
                status_message = publish_gimbal_state(True)
            elif self.path == "/gimbal_raise_ok":
                status_message = publish_gimbal_state(False)
            elif self.path == "/nav_start":
                status_message = start_nav_simulator()
            elif self.path == "/nav_stop":
                status_message = stop_nav_simulator()
            elif self.path == "/pose_low_trigger":
                status_message = publish_config_pose("low_trigger", "低头提前触发点")
            elif self.path == "/pose_low_wait":
                status_message = publish_config_pose("low_wait", "低头等待点")
            elif self.path == "/pose_head_up":
                status_message = publish_config_pose("head_up_trigger", "抬头触发点")
            elif self.path == "/pose_first_attack":
                status_message = publish_config_pose("first_attack", "第一进攻点")
            elif self.path == "/pose_supply":
                status_message = publish_config_pose("supply_pose", "补给点")
            else:
                self.send_error(404)
                return
        except Exception as exc:
            status_message = f"操作失败：{exc}"
        self.send_response(303)
        self.send_header("Location", "/")
        self.end_headers()

    def respond(self, text):
        data = text.encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def respond_json(self, value):
        data = json.dumps(value, ensure_ascii=False).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def log_message(self, fmt, *args):
        return


def validate_files():
    xml_tree()
    rows = load_patrol_rows()
    if len(rows) < 2:
        raise RuntimeError("patrol csv needs at least 2 rows")
    return True


def bind_server(host, preferred_port, attempts=20):
    last_error = None
    for offset in range(attempts):
        port = preferred_port + offset
        try:
            return ThreadingHTTPServer((host, port), Handler), port
        except OSError as exc:
            last_error = exc
            continue
    raise RuntimeError(
        f"failed to bind UI server near port {preferred_port}: {last_error}")


def main():
    global status_monitor
    parser = argparse.ArgumentParser(description="Local UI for rm_sentry_decision BT config")
    parser.add_argument("--host", default=SERVER_HOST)
    parser.add_argument("--port", default=SERVER_PORT, type=int)
    parser.add_argument("--open-browser", action="store_true", help="open the UI in the default browser")
    parser.add_argument("--check", action="store_true", help="validate XML/CSV and exit")
    args = parser.parse_args()

    validate_files()
    if args.check:
        print(f"检查通过：{XML_PATH}")
        print(f"检查通过：{CSV_PATH}")
        return

    server, actual_port = bind_server(args.host, args.port)
    url = f"http://{args.host}:{actual_port}"
    if actual_port != args.port:
        print(f"端口 {args.port} 已被占用，改用 {actual_port}。", flush=True)
    print("正在运行：哨兵行为树配置 UI", flush=True)
    print(f"UI 地址：{url}", flush=True)
    print("可启动 ROS 节点：rm_sentry_decision/decision_node", flush=True)
    print(f"当前行为树 XML：{XML_PATH}", flush=True)
    print(f"当前巡逻 CSV：{CSV_PATH}", flush=True)

    status_monitor = RosStatusMonitor()
    status_monitor.start()

    if args.open_browser:
        threading.Timer(0.7, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        stop_simulator()
        stop_nav_simulator()
        stop_econtrol_simulator()
        stop_decision()
        if status_monitor is not None:
            status_monitor.stop()
        print("\n已停止 UI")


if __name__ == "__main__":
    main()
