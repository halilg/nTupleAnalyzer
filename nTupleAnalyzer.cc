#include "nTupleAnalyzer.h"
#include <iostream>
//#include <fstream>
//#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TDirectoryFile.h"
//#include "TH1D.h"
//#include "TH1F.h"
//#include "TH1I.h"
//#include "analysis.h"
//#include <algorithm>
#include <json/json.h>
#include <json2tchain.h>
//#include <git_ref.h>

// https://github.com/halilg/singleTopAnalysis/blob/master/analysis.cc
// Main analyzer. Reads a single ROOT tree, applies cuts, reports cut efficiencies and generates histograms

using namespace std;
int main(int argc, char **argv){
    TString treeName;
    TString stemp;
    //treeName=tdir+"/"+ttree;
    TChain * T;
    Json::Value root;   // will contains the root value after parsing
    bool debug;
    debug=false;
    if (argc < 6){
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <events> <iroot file> <oroot file> <TDirectory> <TTree>" << std::endl;
        return 1;
    }

    Long64_t nevents = std::atoll(argv[1]);
    Long64_t wevents = 0;
    TString rjfile(argv[2]);
    TString orfile(argv[3]);
    TString tdir(argv[4]);
    TString ttree(argv[5]);

    treeName=tdir+"/"+ttree;
    bool use_weights=true;        // is the input file a json file?
    if (debug) cout << "will analyze " << rjfile << endl;
    if (rjfile.Contains(".json")){
        T = json2tchain(treeName, rjfile);
        if (debug) cout << "received chain contains " <<  T->GetEntries() << " events" << endl;
    } else {
        T = new TChain(treeName);
        cout << "Opening root file: " << rjfile << endl;
        T->Add(rjfile);
    }
    
    cout << "Starting analysis\n";
    //cout << "Accessing tree: " << treeName << endl;
    //TTree* myTree = (TTree*)T.Get(treeName);
    TTree* myTree = T; //->GetTree();
    
    Long64_t nentries = myTree->GetEntries();
    if (nentries == 0){
        cerr << "ROOT tree had no events\n";
        return 1;
    }else{
        cout << "Entries in ROOT tree: " << nentries << endl;
    }
}