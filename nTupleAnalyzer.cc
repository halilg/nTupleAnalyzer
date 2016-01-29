#include "nTupleAnalyzer.h"
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TMath.h"
//#include "TH1F.h"
#include "TH1I.h"
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
    
    // Create the output ROOT file and book the histograms
    TFile E(orfile,"recreate");
    TH1D h_muPt("h_muPt","Muon Pt; PT (GeV); Muons", 25, 0, 100);
    TH1D h_muEta("h_muEta","Muon eta; eta; Muons", 25, -4, 4);
    TH1I h_muMult("h_muMult","Muon Multiplicity; Multiplicity; Events", 6, 0, 5);
    
    // determine the number of events to analyze
    nevents= (nevents < nentries) ? nevents : nentries; // std::min(nevents,nentries);
    if (nevents == -1) nevents = nentries;
    
    cout << "Will analyze " << nevents << " events\n";     //" events\n 1000X:";    
    Long64_t i;
    for (i=0; i<nevents; i++){
        //if (i % 1000 == 0) cout << "+" << std::flush;
        myTree->GetEntry(i);
        if (debug) cout << "Event: " << i << ", muons: " << mu_n << endl;
        h_muMult.Fill(mu_n);
        for (int j=0; j< mu_n; j++){
            h_muEta.Fill(mu_eta[j]);
            h_muPt.Fill(TMath::Sqrt(mu_px[j]*mu_px[j]+mu_py[j]*mu_py[j]));
            if (debug) cout << "Muon #" << j << ": " 
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
    E.Write();
    E.Close();

    cout << "\nAnalyzed " << i << " entries\n";
    cout << "histograms written to: " << orfile << endl;
    
    // write report to JSON file
    root["root_file"]=rjfile.Data();
    root["root_tree_path"]=tdir.Data();
    root["root_tree_name"] = ttree.Data();
    root["events_total"] = nentries;
    root["events_analyzed"] = nevents;
    //root["events_yield_w"] = wevents;
    //root["events_yield"] = y;
    //root["Selection"]["Cut_0"]["Description"] = "Kinematic cuts";
    //root["Selection"]["Cut_0"]["Passed"] = c0;
    //root["Selection"]["Cut_1"]["Description"] = (isElectronDST) ? "MET Cut" : "MTW Cut";
    //root["Selection"]["Cut_1"]["Passed"] = c1;
    
    Json::StyledWriter writer;
    std::string outputConfig = writer.write( root );
    stemp=(rjfile.Remove(0,rjfile.Last('/')+1)).Data();
    stemp.ReplaceAll(".json","");
    stemp.ReplaceAll(".root","");
    stemp = "results/"+stemp + "-analysis.json";
    std::ofstream out(stemp);
    out << outputConfig << std::endl;
    out.close();
    std::cout << "analysis report written to: " << stemp << "\n";
    delete T; 
    return 0;        
        
}