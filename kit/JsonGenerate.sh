#!/bin/bash

Generate1() {
  local file_json_init=${1}
  local file_json=${2}
  local path_work=${3}
  local tag_job=${4}
  local tag_bs=${5}

  local path_job=${path_work}/${tag_job}

  [ -d ${path_job} ] || mkdir -p ${path_job}

  if [ -z ${tag_bs} ]; then
    tag_bs=${tag_bs}
  else
    tag_bs="_bs_${tag_bs}"
  fi

  local file_json_job=${path_job}/${file_json}${tag_bs}.json
  cp ${file_json_init} ${file_json_job}
  JsonSet.py ${file_json_job} SE_PR_thn --force path_input=/lustre/alice/users/szhu/job/JpsiFlowPair/output3_22highIR_pass4/${tag_job}/O2dqflowvecd.root \
    path_output=/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/${tag_job}/SE_PR_thn${tag_bs}.root \
    path_cali_vtxz=/lustre/alice/users/szhu/job/Mult_22pass4/General/22highIR_pass4_study.root:fPosZ_fNumContribNoPileup_pfx_px \
    path_cali_run=/lustre/alice/users/szhu/job/Mult_22pass4/General/22pass4_highIR.root:fNumContribNoPileup \
    run_number=${tag_job}
}

Generate2() {
  local file_json_init=${1}
  local file_json=${2}
  local path_work=${3}
  local tag_job=${4}
  local tag_bs=${5}

  local path_job=${path_work}/${tag_job}

  [ -d ${path_job} ] || mkdir -p ${path_job}

  if [ -z ${tag_bs} ]; then
    tag_bs=${tag_bs}
  else
    tag_bs="_bs_${tag_bs}"
  fi

  local file_json_job=${path_job}/config${tag_bs}.json
  cp ${file_json_init} ${file_json_job}

  JsonSet.py ${file_json_job} ME_PR_thn --force path_input=/lustre/alice/users/szhu/job/JpsiFlowPair/output2_22highIR_pass4/${tag_job}/O2dqflowpairpr.root \
    path_output=/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/${tag_job}/ME_PR_thn${tag_bs}.root
}

for i in $(ls /u/szhu/dir_lustre/job/JpsiFlowPair/analysis_22highIR_pass4); do
  Generate2 ~/empty.json config_thn_me /lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4 ${i}
  for j in $(seq 1 20); do
    Generate1 ~/empty.json config_thn_se /lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4 ${i} ${j}
  done
done