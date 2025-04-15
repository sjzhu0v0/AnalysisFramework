commands="hadd" # 你的命令列表
awk -F', ' -v cmds="$commands" 'BEGIN {
    split(cmds, cmd_list, " ");  # 拆分成数组
}
{
    # 遍历所有命令
    for (cmd_idx in cmd_list) {
        # 先打印当前命令
        printf "%s %s %s ", cmd_list[cmd_idx], cmd_idx, NR

        # 再打印当前行的所有字段（前面加aa/）
        for (i=1; i<=NF; i++) {
            printf "aa/%s.root ", $i;
        }
        printf "\n";  # 换行
    }
}' /lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1

# commands="SE_PR_thn ME_PR_thn" # 你的命令列表
# awk -F', ' -v cmds="$commands" 'BEGIN {
#     split(cmds, cmd_list, " ");  # 拆分成数组
# }
# {
#     # 遍历所有命令
#     for (cmd_idx in cmd_list) {
#         # 先打印当前命令
#         printf "hadd -f /lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge/%s/group%s.root ", cmd_list[cmd_idx], NR

#         # 再打印当前行的所有字段（前面加aa/）
#         for (i=1; i<=NF; i++) {
#             printf "/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/%s/%s.root ", $i, cmd_list[cmd_idx];
#         }
#         printf "\n";  # 换行
#     }
# }' /lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1
