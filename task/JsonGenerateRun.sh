#!/bin/bash

gn_bootstrap=1
gpath_job=/lustre/alice/users/szhu/job
gname_job=JpsiFlowPair

GenerateSE() {
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
  JsonSet.py ${file_json_job} SE_PR_thn --force path_input=${gpath_job}/${gname_job}/output3_22highIR_pass4/${tag_job}/O2dqflowvecd.root \
    path_output=${gpath_job}/${gname_job}/analysis_22highIR_pass4/${tag_job}/SE_PR_thn${tag_bs}.root \
    path_cali_vtxz=${gpath_job}/Mult_22pass4/General/22highIR_pass4_study.root:fPosZ_fNumContribNoPileup_pfx_px \
    path_cali_run=${gpath_job}/Mult_22pass4/General/22pass4_highIR.root:fNumContribNoPileup \
    run_number=${tag_job}
}

GenerateME() {
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

  JsonSet.py ${file_json_job} ME_PR_thn --force path_input=${gpath_job}/${gname_job}/output2_22highIR_pass4/${tag_job}/O2dqflowpairpr.root \
    path_output=${gpath_job}/${gname_job}/analysis_22highIR_pass4/${tag_job}/ME_PR_thn${tag_bs}.root
}

for i in $(ls /u/szhu/dir_lustre/job/${gname_job}/analysis_22highIR_pass4); do
  GenerateME ~/empty.json config_thn_me ${gpath_job}/${gname_job}/analysis_22highIR_pass4 ${i}
  # for j in $(seq 1 20); do
  #   GenerateSE ~/empty.json config_thn_se ${gpath_job}/${gname_job}/analysis_22highIR_pass4 ${i} ${j}
  # done
  if [ ${gn_bootstrap} -eq 1 ]; then
    GenerateSE ~/empty.json config_thn_se ${gpath_job}/${gname_job}/analysis_22highIR_pass4 ${i}
  else
    for j in $(seq 1 ${gn_bootstrap}); do
      GenerateSE ~/empty.json config_thn_se ${gpath_job}/${gname_job}/analysis_22highIR_pass4 ${i} ${j}
    done
  fi
done
