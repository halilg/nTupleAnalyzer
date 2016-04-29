#!/usr/bin/env zsh

PT=41
ELEID=-1 #no ID
ELEID=0 #ID
eta=2.1

time ./nTupleAnalyzer.exe analyze_st.cfg analysis:cut_ele_PT=$PT \
                                         framework:maxEvents=5 \
                                         analysis:cut_ele_eta=$eta \
                                         analysis:cut_ele_ID=$ELEID \
                                         framework:outRootFile=root/pt${PT}-id${ELEID}-nTuple_ST_multi-analysis.root \
                                         framework:outJSONFile=results/pt${PT}-id${ELEID}-nTuple_ST_multi-analysis.json