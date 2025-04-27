awk '{
    printf "Group %d: ", NR 
    gsub(/,/, " ")         
    for(i=1;i<=NF;i++) 
        printf "%s.root ", $i  
    print ""              
}' group.txt


awk '{
    gsub(/,/, " ")
    for (t in types) {
        printf "hadd -f /lustre/alice/users/szhu/job/${gname_job}/analysis_22highIR_pass4/merge/%s_cluster%d.root ", types[t], NR
        for (i = 1; i <= NF; i++)
            printf "/lustre/alice/users/szhu/job/${gname_job}/analysis_22highIR_pass4/%s/%s.root ", $i, types[t]
        print ""
    }
}
BEGIN {
    split("SE_PR ME_PR SE_RR ME_RR", types)
}' /lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1

