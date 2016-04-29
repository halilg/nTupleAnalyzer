#!/usr/bin/env zsh

isols=(-1 0)
pts=(00 01 02 04 05 07 10 15 18 20 22 24 26 30 35 40 50 75)

for isol in $isols; do
    for pt in $pts; do
        echo "***" isol=$isol pt=${pt}
        prefix=pt${pt}-id${isol}
        ./nTupleAnalyzer.exe analyze_st.cfg analysis:cut_ele_PT=$pt analysis:cut_ele_ID=$isol framework:outRootFile=root/${prefix}-nTuple_ST_multi-analysis.root framework:outJSONFile=results/${prefix}-nTuple_ST_multi-analysis.json
        ./nTupleAnalyzer.exe analyze_qcd.cfg analysis:cut_ele_PT=$pt analysis:cut_ele_ID=$isol framework:outRootFile=root/${prefix}-nTuple_QCD_multi-analysis.root framework:outJSONFile=results/${prefix}-nTuple_QCD_multi-analysis.json
    done
done