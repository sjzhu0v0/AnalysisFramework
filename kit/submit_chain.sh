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


declare -A LAST_JOBID_AT_LEVEL

while IFS= read -r CMD || [[ -n "$CMD" ]]; do
  [[ -z "$CMD" ]] && continue
  LEVEL=$(echo -n "$CMD" | grep -oP '^\t*' | wc -m)
  [ $LEVEL -gt 0 ] && LEVEL=$((LEVEL - 1))
  echo "Level: $LEVEL"
  CMD=$(echo "$CMD" | sed 's/^[\t]*//')
  if [[ $LEVEL -gt 0 ]]; then
    PARENT_JOBID=${LAST_JOBID_AT_LEVEL[$((LEVEL-1))]}
    if [[ -n "$PARENT_JOBID" ]]; then
      CMD="$CMD --dependency afterok:$PARENT_JOBID"
    fi
  fi
  echo "Running (level=$LEVEL): $CMD"
  OUTPUT=$($CMD)
  echo "$OUTPUT"
  JOBID=$(echo "$OUTPUT" | grep -oP 'Submitted batch job \K[0-9]+')
  echo "Job ID: $JOBID, Parent Job ID: $PARENT_JOBID, Command: $CMD" > ${1}.log
  if [[ -z "$JOBID" ]]; then
    echo "Fatal: no job found!!!!"
    exit 1
  fi
  LAST_JOBID_AT_LEVEL[$LEVEL]=$JOBID
done < "$1"
