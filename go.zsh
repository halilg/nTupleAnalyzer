#!/usr/bin/env zsh

#/*  QCD_Ele.root
#    SChannel.root
#    SbarChannel.root
#    TChannel.root
#    TTBar.root
#    TTBarFullLep.root
#    TTBarSemiLep.root
#    TWChannel.root
#    TbarChannel.root
#    TbarWChannel.root
#    W1Jet.root
#    W2Jets.root
#    W3Jets.root
#    W4Jets.root
#    WJets.root
##    WW.root
#    WZ.root
#    ZJets.root
#    ZZ.root  */


# defaults
#dataset=TChannel
#dataset=TWChannel
#dataset=SbarChannel
dataset=TTBar
#dataset=WJets
dataset=ZJets
#dataset=WW
#dataset=WZ
#dataset=ZZ
#dataset=QCDEle
#dataset=Data
tag=Notag
prefix=TreesMu

# if provided, assign command line parameters to variables
[[ $1 != "" ]] && dataset=$1
[[ $2 != "" ]] && tag=$2
[[ $3 != "" ]] && prefix=$3

EVENTS=-1  # 10 for all events
RDIR=/ #TreesEle
RTREE=${dataset}_${tag}
FROOT_O=root/h-${prefix}_${dataset}_${tag}.root

if [[ $dataset == "Data" ]]; then
    FPATH=data
    FROOT_I=${prefix}_${dataset}_${tag}.json
else
    FPATH=MC
    FROOT_I=${prefix}_${dataset}_${tag}.root
fi

#echo $FROOT_O

OFCODE=0
if [[ $1 != "-na" ]]; then
    make -s nTupleAnalyzer.exe || exit
    time ./nTupleAnalyzer.exe $EVENTS $FPATH/$FROOT_I $FROOT_O $RDIR $RTREE
    OFCODE=$?
    [[ $OFCODE == 0 ]] || exit $OFCODE
fi

if [[ $1 == "-p" ]]; then
    mkdir -p plots/$FROOT_O
    ./make_plots.zsh $FROOT_O
    OFCODE=$?
    [[ $OFCODE == 0 ]] || exit $OFCODE
fi



