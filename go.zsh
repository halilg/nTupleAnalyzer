#!/usr/bin/env zsh

# General file naming scheme: ${prefix}_${dataset}_${tag}.<extension>

myname=$(basename $0)
if [[ $1 = "-h" ]]; then
    echo Usage: ${myname} "[dataset] [tag] [prefix]"
    exit
fi

# Let's check the ROOT installation just in case
if ((! $+ROOTSYS)); then
  echo ROOTSYS is not defined. Exiting.
  exit
fi

# There seems to be a problem with zsh environment variable handling on Mac OS.
# This piece of code attempts to fix it.
ostype=$(uname)
if [ $ostype = "Darwin" ]; then
    # Fix for El Capitan:
    # http://stackoverflow.com/questions/35568122/why-isnt-dyld-library-path-being-propagated-here
    if ((! $+DYLD_LIBRARY_PATH)); then
        #echo "[$myname] DYLD_LIBRARY_PATH is not defined. Attempting to fix."
        #wdir=$PWD
        #cd $ROOTSYS
        #source bin/thisroot.sh
        #cd $wdir
        DYLD_LIBRARY_PATH=$ROOTSYS/lib
    fi
#else if [ $ostype = "Linux" ]; then
#    if ((! $+LD_LIBRARY_PATH)); then
#        echo LD_LIBRARY_PATH is not defined
#        exit
#    fi
fi

# Define the defaults
dataset=TTBar
#dataset=Data
tag=Notag
prefix=Events

# if provided, assign command line parameters to variables
[[ $1 != "" ]] && dataset=$1
[[ $2 != "" ]] && tag=$2
[[ $3 != "" ]] && prefix=$3

EVENTS=-1  # -1 for all events
RDIR=nTupler #Tree is under this directory in the input ROOT file 
RTREE=Events #Name of the tree
DIROROOT=root #Dir under which the output ROOT file is written
mkdir -p $DIROROOT #Make sure the dir exists
FROOT_O=root/h-${prefix}_${dataset}_${tag}.root #Name of the output ROOT file

if [[ $dataset == "Data" ]]; then #Real data
    FPATH=data
    FROOT_I=${prefix}_${dataset}_${tag}.json
else
    FPATH=MC
    FROOT_I=${prefix}_${dataset}_${tag}.root
fi

#Let's check if the input file exists
if [ ! -f $FPATH/$FROOT_I ]; then
    echo "[$myname] Input file not found: $FPATH/$FROOT_I"
    exit
fi
#echo $FROOT_O

OFCODE=0
if [[ $1 != "-na" ]]; then
    make -s nTupleAnalyzer.exe || exit
    mkdir -p results
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



