path_base=/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge

cd ${path_base}/RFunc
for i in `seq 1 13`; do
        for j in `seq 1 20`; do
                config=config_group${i}_bs${j}.json
                cp ~/empty.json ${config}
                JsonSet.py ${config} RFunc_PR --force path_input_me=${path_base}/ME_PR_thn/group${i}.root \
                path_input_se=${path_base}/SE_PR_thn/group${i}_bs_${j}.root \
                path_output=${path_base}/RFunc/group${i}_bs_${j}.root \
                delta_eta=0.
        done
done


# for i in `seq 1 20` ;do /lustre/alice/users/szhu/work/Analysis/PairFlow/kit/RMerge.exe BS${i}.root ~/dir_lustre/job/JpsiFlowPair/analysis_22highIR_pass4/merge/RFunc/group*_bs_${i}.root ; done

# for i in `seq 1 13` ;do /lustre/alice/users/szhu/work/Analysis/PairFlow/kit/BSMerge.exe group${i}.root ~/dir_lustre/job/JpsiFlowPair/analysis_22highIR_pass4/merge/RFunc/group${i}_*.root ; done

# /lustre/alice/users/szhu/work/Analysis/PairFlow/kit/BSMerge.exe 0.9 result_r_bs.root *

#  ~/dir_lustre/work/Analysis/PairFlow
# /lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge/RFunc_R 

