# 哨兵姿态切换结果反馈
# 用途：电控系统向导航系统反馈姿态切换执行结果

uint8 pose_type    # 0=MOVEMENT, 1=ATTACK, 2=DEFENSE（对应命令）
bool pose_success  # 执行是否成功
