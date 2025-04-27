PATH_MANA="/lustre/alice/users/szhu/work/Analysis/PairFlow"
list_commands=(${PATH_MANA}/macro/ME_PR.exe
  ${PATH_MANA}/macro/SE_PR.exe
  ${PATH_MANA}/macro/ME_RR.exe
  ${PATH_MANA}/macro/SE_RR.exe)

base=$(realpath .)
path_work="/lustre/alice/users/szhu/job/${gname_job}/analysis_22highIR_pass4"

for run in $(ls /lustre/alice/users/szhu/job/${gname_job}/output2_22highIR_pass4 | grep -E "+([0-9])"); do
  [ -d ${path_work}/${run} ] || mkdir -p ${path_work}/${run}
  input_se_pr="/lustre/alice/users/szhu/job/${gname_job}/output3_22highIR_pass4/${run}/O2dqflowvecd.root"
  input_me_pr="/lustre/alice/users/szhu/job/${gname_job}/output2_22highIR_pass4/${run}/O2dqflowpairpr.root"
  input_se_rr="/lustre/alice/users/szhu/job/${gname_job}/output2_22highIR_pass4/${run}/O2dqflowvecd.root"
  input_me_rr="/lustre/alice/users/szhu/job/${gname_job}/output_22highIR_pass4/${run}/O2dqflowpairrr.root"
  output_se_pr="${path_work}/${run}/SE_PR.root"
  output_me_pr="${path_work}/${run}/ME_PR.root"
  output_se_rr="${path_work}/${run}/SE_RR.root"
  output_me_rr="${path_work}/${run}/ME_RR.root"

  sed "s|INPUT_SE_PR|${input_se_pr}|g;s|INPUT_SE_RR|${input_se_rr}|g;s|INPUT_ME_PR|${input_me_pr}|g;s|INPUT_ME_RR|${input_me_rr}|g;s|OUTPUT_SE_PR|${output_se_pr}|g;s|OUTPUT_SE_RR|${output_se_rr}|g;s|OUTPUT_ME_PR|${output_me_pr}|g;s|OUTPUT_ME_RR|${output_me_rr}|g" ${PATH_MANA}/config.json >${path_work}/${run}/config.json

  echo -n "cd ${path_work}/${run}"
  for i in ${list_commands[@]}; do
    echo  -n " ; $i ${path_work}/${run}/config.json"
  done
  echo 
done
