#!/usr/bin/env zsh

pts=(00 01 02 04 05 07 10 15 18 20 22 24 26 30 35 40 50 75)
for pt in $pts; do
    echo pt=$pt
    ./nTupleAnalyzer.exe analyze_st.cfg $pt
    ./nTupleAnalyzer.exe analyze_qcd.cfg $pt
    
    mv results/nTuple_ST_multi-analysis.json results/PT${pt}-nTuple_ST_multi-analysis.json
    mv results/nTuple_QCD_multi-analysis.json results/PT${pt}-nTuple_QCD_multi-analysis.json
done



