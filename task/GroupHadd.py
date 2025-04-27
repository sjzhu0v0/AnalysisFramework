#!/usr/bin/env python3
import os

# 输入参数
commands = "SE_PR_thn"
input_file = "/lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1"
output_dir = "/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge"

# 解析命令列表
cmd_list = commands.split()

# 读取输入文件
with open(input_file, 'r') as f:
    lines = f.readlines()

# 处理每一行
for line_idx, line in enumerate(lines, start=1):
    # 拆分字段（假设以逗号+空格分隔）
    fields = [f.strip() for f in line.split(', ')]

    # 遍历bootstrap样本 (1-20)
    for j in range(1, 21):
        # 遍历每个命令
        for cmd in cmd_list:
            # 构建输出文件路径
            output_file = f"{output_dir}/{cmd}/group{line_idx}_bs_{j}.root"

            # 构建输入文件列表
            input_files = " ".join(
                f"/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/{field}/{cmd}_bs_{j}.root"
                for field in fields
            )

            # 打印hadd命令
            print(f"hadd -f {output_file} {input_files}")
