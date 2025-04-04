list_commands=(${PATH_MANA}/macro/ME_PR.exe
  ${PATH_MANA}/macro/SE_PR.exe
  ${PATH_MANA}/macro/ME_RR.exe
  ${PATH_MANA}/macro/SE_RR.exe)

base=$(realpath .)
path_work="/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4"

for run in $(ls /lustre/alice/users/szhu/job/JpsiFlowPair/output2_22highIR_pass4 | grep -E "+([0-9])"); do
  [ -d ${path_work}/${run} ] || mkdir -p ${path_work}/${run}
  input_se_pr="/lustre/alice/users/szhu/job/JpsiFlowPair/output3_22highIR_pass4/${run}/O2dqflowvecd.root"
  input_me_pr="/lustre/alice/users/szhu/job/JpsiFlowPair/output2_22highIR_pass4/${run}/O2dqflowpairpr.root"
  input_se_rr="/lustre/alice/users/szhu/job/JpsiFlowPair/output2_22highIR_pass4/${run}/O2dqflowvecd.root"
  input_me_rr="/lustre/alice/users/szhu/job/JpsiFlowPair/output_22highIR_pass4/${run}/O2dqflowpairrr.root"
  output_se_pr="${path_work}/${run}/SE_PR.root"
  output_me_pr="${path_work}/${run}/ME_PR.root"
  output_se_rr="${path_work}/${run}/SE_RR.root"
  output_me_rr="${path_work}/${run}/ME_RR.root"

  sed "s|SE_PR_INPUT|${input_se_pr}|g;s|SE_RR_INPUT|${input_se_rr}|g;s|ME_PR_INPUT|${input_me_pr}|g;s|ME_RR_INPUT|${input_me_rr}|g;s|SE_PR_OUTPUT|${output_se_pr}|g;s|SE_RR_OUTPUT|${output_se_rr}|g;s|ME_PR_OUTPUT|${output_me_pr}|g;s|ME_RR_OUTPUT|${output_me_rr}|g" ${PATH_MANA}/config.json > ${path_work}/${run}/config.json

done
