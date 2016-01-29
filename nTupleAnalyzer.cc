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
    // Muon variables
    #define maxReco 30
    int mu_n;
    int mu_charge[maxReco];
    double mu_px[maxReco];
    double mu_py[maxReco];
    double mu_pz[maxReco];
    double mu_phi[maxReco];
    double mu_theta[maxReco];
    double mu_eta[maxReco];
      
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
    myTree->SetBranchAddress("mu_n", &mu_n);
    myTree->SetBranchAddress("mu_charge", mu_charge);
    myTree->SetBranchAddress("mu_px", mu_px);
    myTree->SetBranchAddress("mu_py", mu_py);
    myTree->SetBranchAddress("mu_pz", mu_pz);
    myTree->SetBranchAddress("mu_phi", mu_phi);
    myTree->SetBranchAddress("mu_theta", mu_theta);
    myTree->SetBranchAddress("mu_eta", mu_eta);
    
    Long64_t nentries = myTree->GetEntries();
    if (nentries == 0){
        cerr << "ROOT tree had no events\n";
        return 1;
    }
    cout << "Entries in ROOT tree: " << nentries << endl;
    
    for (int i=0; i<nentries; i++){
        myTree->GetEntry(i);
        cout << "Event: " << i << ", muons: " << mu_n << endl;
        for (int j=0; j< mu_n; j++){
            cout << "Muon #" << j << ": " 
                 << "mu_charge: " << mu_charge[j]
                 << ", px: " << mu_px[j]
                 << ", py: " << mu_py[j]
                 << ", pz: " << mu_pz[j]
                 << ", phi: " << mu_phi[j]
                 << ", theta: " << mu_theta[j]
                 << ", eta: " << mu_eta[j]
                 << endl;
        }
    }
        
        
        
}