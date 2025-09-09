#!/bin/bash

PREV_JOBID=""

submit() {
    cd /lustre/alice/users/szhu/submit
    date >> history_submission
    echo $@ >> history_submission

    local partition="main"       # 默认分区
    local job_name="my_job"      # 默认任务名称
    local time_limit="2:00:00"   # 默认运行时间限制
    local dependency=""          # 新增：默认无依赖
    local path_env path_command ncommands_onefile

    local args=("$@")
    local i=0

    while [[ $i -lt ${#args[@]} ]]; do
        case "${args[i]}" in
            -p)
                partition="${args[i+1]}"
                i=$((i + 2))
                ;;
            -n)
                job_name="${args[i+1]}"
                i=$((i + 2))
                ;;
            -t)
                time_limit="${args[i+1]}"
                i=$((i + 2))
                ;;
            --dependency)  # 新增：处理依赖选项
                dependency="${args[i+1]}"
                i=$((i + 2))
                ;;
            *)
                if [[ -z ${path_env} ]]; then
                    path_env="${args[i]}"
                elif [[ -z ${path_command} ]]; then
                    path_command=$(realpath "${args[i]}")
                elif [[ -z ${ncommands_onefile} ]]; then
                    ncommands_onefile="${args[i]}"
                else
                    echo "Error: Too many arguments."
                    return 1
                fi
                i=$((i + 1))
                ;;
        esac
    done

    if [[ -z ${path_env} || -z ${path_command} ]]; then
        echo "Usage: submit [-p partition] [-n job_name] [-t time_limit] [--dependency <dependency_spec>] <path_env> <path_command> [ncommands_onefile]"
        echo "Dependency examples:"
        echo "  --dependency afterany:123456   # 在作业ID 123456结束后运行"
        echo "  --dependency afterok:123456    # 在作业ID 123456成功完成后运行"
        return 1
    fi

    if [[ ! -f ${path_command} ]]; then
        echo "Error: Command file ${path_command} not found!"
        return 1
    fi
    if [[ ! -f /u/szhu/slurm_env/${path_env} ]]; then
        echo "Error: SLURM environment script /u/szhu/slurm_env/${path_env} not found!"
        return 1
    fi

    local base=$(realpath .)
    [[ -d ${base}/slurm_cache_${job_name} ]] && rm -r ${base}/slurm_cache_${job_name}
    mkdir -p ${base}/slurm_cache_${job_name}
    cd ${base}/slurm_cache_${job_name} || return 1

    if [[ -n ${ncommands_onefile} ]]; then
        mkdir -p ${base}/slurm_cache_${job_name}/list_commands
        split -l ${ncommands_onefile} ${path_command} ${base}/slurm_cache_${job_name}/list_commands/command_part_
        chmod +x ${base}/slurm_cache_${job_name}/list_commands/command_part_*
        realpath ${base}/slurm_cache_${job_name}/list_commands/command_part_* > ${base}/slurm_cache_${job_name}/run.sh
        path_command=${base}/slurm_cache_${job_name}/run.sh
    fi

    local n_commands=$(wc -l < ${path_command})

    local sbatch_cmd=(
        sbatch
        --job-name=${job_name}
        --partition=${partition}
        --time=${time_limit}
        --array=1-${n_commands}
    )

    [[ -n ${dependency} ]] && sbatch_cmd+=(--dependency=${dependency})

    sbatch_cmd+=(/u/szhu/slurm_env/${path_env} ${path_command})

    "${sbatch_cmd[@]}"

    echo "Submitted job array with ${n_commands} tasks (partition: ${partition}, job name: ${job_name}, time limit: ${time_limit})."
    [[ -n ${dependency} ]] && echo "Dependency: ${dependency}"
    cd ${base}
}

while IFS= read -r CMD || [[ -n "$CMD" ]]; do
  [[ -z "$CMD" ]] && continue
  if [[ -n "$PREV_JOBID" ]]; then
    CMD="$CMD --dependency=afterok:$PREV_JOBID"
  fi
  echo "Running: $CMD"
  OUTPUT=$($CMD)
  echo "$OUTPUT"
  JOBID=$(echo "$OUTPUT" | grep -oP 'Submitted batch job \K[0-9]+')
  if [[ -z "$JOBID" ]]; then
    echo "Fatal: no job found!!!!"
    exit 1
  fi
  PREV_JOBID=$JOBID
done < ${1}