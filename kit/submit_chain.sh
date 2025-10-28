#!/bin/bash

PREV_JOBID=""

submit() {
    cd /lustre/alice/users/szhu/submit
    date >> history_submission
    echo $@ >> history_submission

    local partition="main"
    local job_name="my_job"
    local time_limit="2:00:00"
    local dependency=""
    local array_spec=""
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
            --dependency)
                dependency="${args[i+1]}"
                i=$((i + 2))
                ;;
            -a)
                array_spec="${args[i+1]}"
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
        echo "Usage: submit [-p partition] [-n job_name] [-t time_limit] [-a array_spec] [--dependency <dependency_spec>] <path_env> <path_command> [ncommands_onefile]"
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

    if [[ -z ${array_spec} ]]; then
        array_spec="1-${n_commands}"
    fi

    local sbatch_cmd=(
        sbatch
        --job-name=${job_name}
        --partition=${partition}
        --time=${time_limit}
        --array=${array_spec}
    )

    [[ -n ${dependency} ]] && sbatch_cmd+=(--dependency=${dependency})

    sbatch_cmd+=(/u/szhu/slurm_env/${path_env} ${path_command})

    "${sbatch_cmd[@]}"

    echo "Submitted job array with spec '${array_spec}' (partition: ${partition}, job name: ${job_name}, time limit: ${time_limit})."
    [[ -n ${dependency} ]] && echo "Dependency: ${dependency}"
    cd ${base}
}


#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <job_script_file>" >&2
  exit 1
fi

declare -A LAST_SUBMITTED_JOBID_AT_LEVEL

while IFS= read -r CMD || [[ -n "$CMD" ]]; do
  [[ -z "$CMD" ]] && continue
  [[ "$CMD" =~ ^[[:space:]]*# ]] && continue

  # 计算缩进层级（制表符数量）
  LEVEL=0
  temp="$CMD"
  while [[ $temp == $'\t'* ]]; do
    ((LEVEL++))
    temp="${temp#?}"
  done

  # 去掉开头的制表符
#   CMD="${CMD#$'\t'*}"
  # 上面写法不安全，改用：
  CMD="${CMD#"${CMD%%[!$'\t']*}"}"  # 去掉所有开头的制表符
  # 或简单点（因为已知开头是 LEVEL 个 \t）：
#   CMD="${CMD:$LEVEL}"

  # 查找最近的祖先作业 ID
  PARENT_JOBID=""
  if (( LEVEL > 0 )); then
    ancestor_level=$((LEVEL - 1))
    while (( ancestor_level >= 0 )); do
      if [[ -n "${LAST_SUBMITTED_JOBID_AT_LEVEL[$ancestor_level]}" ]]; then
        PARENT_JOBID="${LAST_SUBMITTED_JOBID_AT_LEVEL[$ancestor_level]}"
        break
      fi
      ((ancestor_level--))
    done
    if [[ -n "$PARENT_JOBID" ]]; then
      CMD="$CMD --dependency afterok:$PARENT_JOBID"
    fi
  fi

  echo "Running (level=$LEVEL): $CMD" >&2
  OUTPUT=$(eval "$CMD") || {
    echo "Error: Command failed: $CMD" >&2
    exit 1
  }
  echo "$OUTPUT" >&2

  # 提取 JOBID（兼容非 GNU grep）
  JOBID=$(echo "$OUTPUT" | grep -o 'Submitted batch job [0-9]*' | head -n1 | grep -o '[0-9]*')
  if [[ -z "$JOBID" ]]; then
    echo "Fatal: Failed to extract job ID from output!" >&2
    echo "Output was: $OUTPUT" >&2
    exit 1
  fi

  LAST_SUBMITTED_JOBID_AT_LEVEL[$LEVEL]=$JOBID
  echo "Job ID: $JOBID, Level: $LEVEL, Command: $CMD" >> "${1}.log"

done < "$1"