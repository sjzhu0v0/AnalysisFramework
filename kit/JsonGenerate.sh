#!/bin/bash

Generate() {
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
  JsonSet.py ${file_json_job} SE_PR_thn --force path_input=/lustre/alice/users/szhu/job/JpsiFlowPair/output3_22highIR_pass4/${tag_job}/O2dqflowvecd.root
  JsonSet.py ${file_json_job} SE_PR_thn --force path_output=/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/${tag_job}/SE_PR_thn.root
  JsonSet.py ${file_json_job} SE_PR_thn --force path_cali_vtxz=/lustre/alice/users/szhu/job/Mult_22pass4/General/22highIR_pass4_study.root:fPosZ_fNumContribNoPileup_pfx_px
  JsonSet.py ${file_json_job} SE_PR_thn --force path_cali_run=/lustre/alice/users/szhu/job/Mult_22pass4/General/22pass4_highIR.root:fNumContribNoPileup
  JsonSet.py ${file_json_job} SE_PR_thn --force run_number=${tag_job}

  JsonSet.py ${file_json_job} ME_PR_thn --force path_input=/lustre/alice/users/szhu/job/JpsiFlowPair/output2_22highIR_pass4/${tag_job}/O2dqflowpairpr.root
  JsonSet.py ${file_json_job} ME_PR_thn --force path_output=/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/${tag_job}/ME_PR_thn.root
}

for i in $(ls /u/szhu/dir_lustre/job/JpsiFlowPair/analysis_22highIR_pass4); do
  Generate ~/empty.json config_thn.json /lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4 ${i}
done
