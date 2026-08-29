#!/usr/bin/env python3
"""Local web UI for editing and launching the first-attack patrol BT config."""

import argparse
import csv
import html
import json
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
log_lines = deque(maxlen=300)
status_message = ""


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


PKG_ROOT = package_root()
WORKSPACE_DIR = PKG_ROOT.parent
CONFIG_DIR = PKG_ROOT / "config"
XML_PATH = CONFIG_DIR / BT_XML
CSV_PATH = CONFIG_DIR / PATROL_CSV
SIM_CONFIG_PATH = CONFIG_DIR / SIM_CONFIG_JSON
SIM_SCRIPT = PKG_ROOT / "scripts" / "referee_simulator.py"
NAV_SIM_SCRIPT = PKG_ROOT / "scripts" / "nav_pose_simulator.py"

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
    up = find_action(root, "SendThroughGoalsAction", "经过抬头触发点后到第一进攻点")
    need = find_condition(root, "NeedSupply", "判断是否需要撤退补给")
    recovered = find_condition(root, "RecoveredEnough", "确认补给后血量弹量足够")
    supply = find_action(root, "SendGoalAction", "导航到补给点")
    patrol_speed = find_action(root, "SetNavSpeedAction", "巡逻速度设为2.0米每秒")
    wait_reached = find_action(root, "WaitUntilReached", "等待机器人进入目标0.6米范围")

    config = {
        "xml_path": str(XML_PATH),
        "csv_path": str(CSV_PATH),
        "low_trigger": low.get("through_pose", "") if low is not None else "",
        "low_wait": low.get("final_pose", "") if low is not None else "",
        "head_up_trigger": up.get("through_pose", "") if up is not None else "",
        "first_attack": up.get("final_pose", "") if up is not None else "",
        "supply_pose": supply.get("goal_pose", "") if supply is not None else "",
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
    up = find_action(root, "SendThroughGoalsAction", "经过抬头触发点后到第一进攻点")
    need = find_condition(root, "NeedSupply", "判断是否需要撤退补给")
    recovered = find_condition(root, "RecoveredEnough", "确认补给后血量弹量足够")
    supply = find_action(root, "SendGoalAction", "导航到补给点")
    return_first = find_action(root, "SendGoalAction", "补给后返回第一进攻点")
    patrol_speed = find_action(root, "SetNavSpeedAction", "巡逻速度设为2.0米每秒")
    wait_reached = find_action(root, "WaitUntilReached", "等待机器人进入目标0.6米范围")

    low_trigger = set_pose_xyz(low.get("through_pose"), form_value(fields, "low_trigger_x"), form_value(fields, "low_trigger_y"), 0)
    low_wait = set_pose_xyz(low.get("final_pose"), form_value(fields, "low_wait_x"), form_value(fields, "low_wait_y"), 0)
    head_up = set_pose_xyz(up.get("through_pose"), form_value(fields, "head_up_x"), form_value(fields, "head_up_y"), 0)
    first_attack = set_pose_xyz(up.get("final_pose"), form_value(fields, "first_attack_x"), form_value(fields, "first_attack_y"), 0)
    supply_pose = set_pose_xyz(supply.get("goal_pose"), form_value(fields, "supply_x"), form_value(fields, "supply_y"), 0)

    low.set("through_pose", low_trigger)
    low.set("final_pose", low_wait)
    up.set("through_pose", head_up)
    up.set("final_pose", first_attack)
    supply.set("goal_pose", supply_pose)
    return_first.set("goal_pose", first_attack)

    need.set("enter_hp", str(int(float(form_value(fields, "enter_hp")))))
    need.set("enter_ammo", str(int(float(form_value(fields, "enter_ammo")))))
    need.set("exit_hp", str(int(float(form_value(fields, "recover_hp")))))
    need.set("exit_ammo", str(int(float(form_value(fields, "recover_ammo")))))
    recovered.set("min_hp", str(int(float(form_value(fields, "recover_hp")))))
    recovered.set("min_ammo", str(int(float(form_value(fields, "recover_ammo")))))
    patrol_speed.set("speed_mps", fmt_num(form_value(fields, "patrol_speed")))
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


def ros_shell_command(command):
    setup = (
        "for setup in /opt/ros/*/setup.bash; do "
        'if [ -f "$setup" ]; then source "$setup"; break; fi; '
        "done; "
        f"if [ -f {shlex.quote(str(WORKSPACE_DIR / 'install' / 'setup.bash'))} ]; then "
        f"source {shlex.quote(str(WORKSPACE_DIR / 'install' / 'setup.bash'))}; "
        "fi; "
        f"cd {shlex.quote(str(PKG_ROOT.parent))}; "
        f"exec {shlex.join(command)}"
    )
    return ["bash", "-lc", setup]


def read_process_output(proc):
    assert proc.stdout is not None
    for line in proc.stdout:
        text = line.rstrip()
        log_lines.append(text)
        print(text, flush=True)


def start_decision(use_sim_time):
    global process
    if process_running():
        return "decision_node 已经在运行"

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
        cwd=str(PKG_ROOT.parent),
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
    subprocess.run(
        ["pkill", "-f", str(SIM_SCRIPT)],
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
        cwd=str(PKG_ROOT.parent),
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
    subprocess.run(
        ["pkill", "-f", str(NAV_SIM_SCRIPT)],
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
        cwd=str(PKG_ROOT.parent),
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


def publish_robot_pose(x, y, label):
    start_info = [
        f"正在模拟机器人到达：{label}",
        f"坐标：x={x}, y={y}",
        "TF：map -> gimbal_yaw_fake / base_link",
    ]
    for line in start_info:
        print(line, flush=True)
        log_lines.append(line)

    node = None
    context = None
    try:
        import rclpy
        from geometry_msgs.msg import TransformStamped
        from tf2_msgs.msg import TFMessage

        context = rclpy.context.Context()
        rclpy.init(context=context)
        node = rclpy.create_node("bt_config_ui_pose_pub", context=context)
        pub = node.create_publisher(TFMessage, "/tf", 10)
        time.sleep(0.2)
        for _ in range(80):
            now = node.get_clock().now().to_msg()
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


def publish_gimbal_state(lower):
    mode = 5 if lower else 1
    label = "低头成功" if lower else "抬头成功"
    start_info = [
        f"正在发布云台电控反馈：{label}",
        "话题：/gimbal_controller/gimbal_status",
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
        from rm_msgs.msg import GimbalStatus

        context = rclpy.context.Context()
        rclpy.init(context=context)
        node = rclpy.create_node("bt_config_ui_gimbal_state_pub", context=context)
        pub = node.create_publisher(GimbalStatus, "/gimbal_controller/gimbal_status", 10)

        msg = GimbalStatus()
        msg.gimbal_mode = int(mode)

        # 发布多次，给 DDS 发现订阅者留一点时间，避免一次性发布被错过。
        time.sleep(0.2)
        for _ in range(60):
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
  </style>
</head>
<body>
  <header>
    <div>
      <h1>第一进攻点巡逻补给行为树</h1>
      <div class="status">状态：{run_text}　{html.escape(status_message)}</div>
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
        if self.path != "/":
            self.send_error(404)
            return
        self.respond(render_page())

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
                sim_message = start_simulator(fields)
                decision_message = restart_decision("True")
                status_message = f"{sim_message}；{decision_message}"
            elif self.path == "/sim_save":
                save_sim_config(fields)
                status_message = "模拟裁判参数已保存"
            elif self.path == "/sim_stop":
                status_message = stop_simulator()
            elif self.path == "/gimbal_lower_ok":
                status_message = publish_gimbal_state(True)
            elif self.path == "/gimbal_raise_ok":
                status_message = publish_gimbal_state(False)
            elif self.path == "/nav_start":
                status_message = start_nav_simulator()
            elif self.path == "/nav_stop":
                status_message = stop_nav_simulator()
            elif self.path == "/pose_low_trigger":
                status_message = publish_robot_pose(3.0, 0.0, "低头提前触发点")
            elif self.path == "/pose_low_wait":
                status_message = publish_robot_pose(6.0, 0.0, "低头等待点")
            elif self.path == "/pose_head_up":
                status_message = publish_robot_pose(4.5, -2.0, "抬头触发点")
            elif self.path == "/pose_first_attack":
                status_message = publish_robot_pose(5.27, -3.07, "第一进攻点")
            elif self.path == "/pose_supply":
                status_message = publish_robot_pose(2.0, 0.0, "补给点")
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

    if args.open_browser:
        threading.Timer(0.7, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        stop_simulator()
        stop_decision()
        print("\n已停止 UI")


if __name__ == "__main__":
    main()
