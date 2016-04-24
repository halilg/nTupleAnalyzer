#include <cstdio>
#include <iostream>
#include <fstream>

#include "nTupleAnalyzer.h"
#include "event.h"
#include "analysis.h"

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TMath.h"
#include "TH1I.h"
#include <json/json.h>
#include <json2tchain.h>

// https://github.com/halilg/singleTopAnalysis/blob/master/analysis.cc
// Main analyzer. Reads a single ROOT tree, applies cuts, reports cut efficiencies and generates histograms
//

TString basename(const TString & path){
    TString _path(path);
    return _path.Remove(0,_path.Last('/')+1);
}

using namespace std;
int main(int argc, char **argv){
    bool debug=true;
    debug=false;
    event myEvent;
    analysis myAnalysis;
    TString treeName;
    //treeName=tdir+"/"+ttree;
    TChain * T;
    Json::Value rooti;   // will contain the root value for configuration
    Json::Value rooto;   // will contain the root value for report

    double wevents = 0;
    double swevents = 0;

    if (argc < 2){
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <configuration file>" << std::endl;
        return 1;
    }
    std::string cfgfile(argv[1]);

    int minpt=0;
    if (argc > 2){
        minpt=std::atoi(argv[2]);
    }    
    if (debug) std::cout << "command line argument: " << minpt << std::endl;
    
    /////////////////////////////////////////////////////////
    // read configuration file
    {
        Json::Reader reader;
        std::string line, inputConfig;
        
        if (debug)  std::cout << "Reading global configuration " << cfgfile << " : ";
        std::ifstream myfile (cfgfile);
        if (myfile.is_open()){
            while ( getline (myfile,line) ){ inputConfig += line; }
            myfile.close();
        } else {
          std::cerr << "Unable to open file: " << cfgfile << std::endl;
          exit(1);
        }
        
        if (debug) std::cout << "parsing json : ";
        bool parsingSuccessful = reader.parse( inputConfig, rooti );
        if ( !parsingSuccessful )
        {
            // report to the user the failure and their locations in the document.
            std::cerr  << "Failed to parse configuration\n"
                       << reader.getFormattedErrorMessages();
            exit(1);
        }
        if (debug) std::cout << "success.\n";
    }
    
    long long int nevents=rooti["analysis"]["maxEvents"].asInt64();
    TString rjfile(rooti["io"]["inRootFile"].asString());
    TString tdir(rooti["io"]["treeDir"].asString());
    TString ttree(rooti["io"]["treeName"].asString());
    TString ordir(rooti["io"]["outRootDir"].asString());
    TString ojdir(rooti["io"]["outJSONDir"].asString());
    if (debug) std::cout << nevents <<   std::endl
              << rjfile << std::endl
              << ordir << std::endl
              << ojdir << std::endl
              << tdir << std::endl
              << ttree << std::endl;
    rooto["configuration"]["io"]=rooti["io"];
    rooto["configuration"]["analysis"]=rooti["analysis"];
    /////////////////////////////////////////////////////////
    //return 0;
    TH1D * h_gen_weight;
    TH1D * h_muPt;
    TH1D * h_muEta;
    TH1I * h_muMult;
    TH1D * h_elePt;
    TH1D * h_eleEta;
    TH1I * h_eleMult;
    TH1D * h_eleID;
    TH1D * h_photPt;
    TH1D * h_photEta;
    TH1I * h_photMult;
    TH1D * h_jetPt;
    TH1D * h_jetEta;
    TH1I * h_jetMult;
    TH1D * h_METPt;
    TH1D * h_METEta;
    TH1I * h_METMult;
    
    treeName=tdir+"/"+ttree;
    bool use_weights=true;        // is the input file a json file?
    if (debug) cout << "will analyze " << rjfile << endl;
    if (rjfile.EndsWith(".json")){
        T = json2tchain(treeName, rjfile);
        if (debug) cout << "received chain contains " <<  T->GetEntries() << " events" << endl;
    } else {
        T = new TChain(treeName);
        cout << "Opening root file: " << rjfile << endl;
        T->Add(rjfile);
    }
    T->LoadTree(0);
    cout << "Starting analysis\n";
    //cout << "Accessing tree: " << treeName << endl;
    //TTree* myTree = (TTree*)T.Get(treeName);

    // Create the output ROOT file
    TString ofname(ordir+TString("/h-")+basename(rjfile.Data()));
    if (ofname.EndsWith(".json")) ofname=ofname+TString(".root");

    if (debug) std::cout << "Creating: " << ofname << std::endl;
    TFile E(ofname,"recreate");
    
    myEvent.is_mu=false;
    myEvent.is_ele=false;
    myEvent.is_phot=false;
    myEvent.is_jet=false;
    myEvent.is_MET=false;
    
    TTree* myTree = T; //->GetTree();
    myTree->SetBranchAddress("gen_weight",&myEvent.gen_weight);
    h_gen_weight = new TH1D ("h_gen_weight","Event Weight (GEN); Weight (GeV); Events", 50, -400, 400);
    if (myTree->GetBranch("mu_n")) {
        myEvent.is_mu=true;
        myTree->SetBranchAddress("mu_n", &myEvent.mu_n);
        myTree->SetBranchAddress("mu_charge", myEvent.mu_charge);
        myTree->SetBranchAddress("mu_px", myEvent.mu_px);
        myTree->SetBranchAddress("mu_py", myEvent.mu_py);
        myTree->SetBranchAddress("mu_pz", myEvent.mu_pz);
        myTree->SetBranchAddress("mu_phi", myEvent.mu_phi);
        myTree->SetBranchAddress("mu_theta", myEvent.mu_theta);
        myTree->SetBranchAddress("mu_eta", myEvent.mu_eta);

        // book the histograms
        h_muPt = new TH1D ("h_muPt","Muon Pt; PT (GeV); Muons", 25, 0, 100);
        h_muEta = new TH1D ("h_muEta","Muon eta; eta; Muons", 25, -4, 4);
        h_muMult = new TH1I("h_muMult","Muon Multiplicity; Multiplicity; Events", 6, 0, 5);
    }

    if (myTree->GetBranch("ele_n")) {
        myEvent.is_ele=true;
        myTree->SetBranchAddress("ele_n", &myEvent.ele_n);
        myTree->SetBranchAddress("ele_charge", myEvent.ele_charge);
        myTree->SetBranchAddress("ele_px", myEvent.ele_px);
        myTree->SetBranchAddress("ele_py", myEvent.ele_py);
        myTree->SetBranchAddress("ele_pz", myEvent.ele_pz);
        myTree->SetBranchAddress("ele_phi", myEvent.ele_phi);
        myTree->SetBranchAddress("ele_theta", myEvent.ele_theta);
        myTree->SetBranchAddress("ele_eta", myEvent.ele_eta);
        myTree->SetBranchAddress("ele_id", myEvent.ele_id);

        // book the histograms
        h_elePt = new TH1D ("h_elePt","Electron Pt; PT (GeV); Electrons", 25, 0, 100);
        h_eleEta = new TH1D ("h_eleEta","Electron eta; eta; Electrons", 25, -4, 4);
        h_eleMult = new TH1I("h_eleMult","Electron Multiplicity; Multiplicity; Events", 6, 0, 5);
        h_eleID = new TH1D("h_eleID","Electron ID; Electrons", 11, 0, 10);
    }
    
    Long64_t nentries = myTree->GetEntries();
    if (nentries == 0){
        cerr << "ROOT tree had no events\n";
        return 1;
    }
    cout << "Entries in ROOT tree: " << nentries << endl;
    
    // determine the number of events to analyze
    nevents= (nevents < nentries) ? nevents : nentries; // std::min(nevents,nentries);
    if (nevents == -1) nevents = nentries;
    
    cout << "Will analyze " << nevents << " events\n";     //" events\n 1000X:";    
    Long64_t i;
    Long64_t y=0;
    Long64_t c0=0;
    double c0w=0;
    unsigned int cut_ele_PT = minpt; // GeV
    bool eventOK;
    bool c0OK;
    for (i=0; i<nevents; i++){
        //if (i % 1000 == 0) cout << "+" << std::flush;
        eventOK = false;
        c0OK = false;
        myAnalysis.new_event(myEvent);
        myTree->GetEntry(i);
        swevents += myEvent.gen_weight;
        h_gen_weight->Fill(myEvent.gen_weight);
        if (myEvent.is_mu){
            if (debug) cout << "Event: " << i << ", muons: " << myEvent.mu_n << endl;
            h_muMult->Fill(myEvent.mu_n);
            for (int j=0; j< myEvent.mu_n; j++){
                h_muEta->Fill(myEvent.mu_eta[j]);
                h_muPt->Fill(TMath::Sqrt(myEvent.mu_px[j]*myEvent.mu_px[j]+myEvent.mu_py[j]*myEvent.mu_py[j]));
                if (debug) cout << "Muon #" << j << ": " 
                     << "mu_charge: " << myEvent.mu_charge[j]
                     << ", px: " << myEvent.mu_px[j]
                     << ", py: " << myEvent.mu_py[j]
                     << ", pz: " << myEvent.mu_pz[j]
                     << ", phi: " << myEvent.mu_phi[j]
                     << ", theta: " << myEvent.mu_theta[j]
                     << ", eta: " << myEvent.mu_eta[j]
                     << endl;
            }
        }
        
        if (myEvent.is_ele){
            if (debug) cout << "Event: " << i << ", electrons: " << myEvent.ele_n << endl;
            h_eleMult->Fill(myEvent.ele_n, myEvent.gen_weight);
            double PT2;
            for (int j=0; j< myEvent.ele_n; j++){
                PT2 = myEvent.ele_px[0]*myEvent.ele_px[0] + myEvent.ele_py[0]*myEvent.ele_py[0];
                if (PT2 > cut_ele_PT*cut_ele_PT && myEvent.ele_id[j] == 1){
                    c0OK=true;
                }
                h_eleEta->Fill(myEvent.ele_eta[j], myEvent.gen_weight);
                h_elePt->Fill(TMath::Sqrt(myEvent.ele_px[j]*myEvent.ele_px[j]+myEvent.ele_py[j]*myEvent.ele_py[j]), myEvent.gen_weight);
                h_eleID->Fill(myEvent.ele_id[j], myEvent.gen_weight);
                if (debug) cout << "Electron #" << j << ": " 
                     << "ele_charge: " << myEvent.ele_charge[j]
                     << ", px: " << myEvent.ele_px[j]
                     << ", py: " << myEvent.ele_py[j]
                     << ", pz: " << myEvent.ele_pz[j]
                     << ", phi: " << myEvent.ele_phi[j]
                     << ", theta: " << myEvent.ele_theta[j]
                     << ", eta: " << myEvent.ele_eta[j]
                     << ", id: " << myEvent.ele_id[j]
                     << endl;
            }
            if (c0OK){
                c0 += 1;
                c0w += myEvent.gen_weight;
            }
        }

        eventOK=c0OK;
        if (eventOK){
            wevents += myEvent.gen_weight;
            y += 1;
        }

    }
    E.Write();
    E.Close();

    if ( !myEvent.is_mu && !myEvent.is_ele && !myEvent.is_phot && !myEvent.is_jet && !myEvent.is_MET)
        cout << "\nLooped through " << i << " entries\n";
    else
        cout << "\nAnalyzed " << i << " entries\n";
    cout << "histograms written to: " << ofname << endl;
    
    // write report to JSON file
    
    char buff[100];
    sprintf(buff,"Max Electron PT > %i GeV)", cut_ele_PT);
// std::string buffAsStdStr = buff;
    
    //rooto["root_file"]=rjfile.Data();
    //rooto["root_tree_path"]=tdir.Data();
    //rooto["root_tree_name"] = ttree.Data();
    rooto["events_total"] = nentries;
    rooto["events_analyzed"] = nevents;
    rooto["events_analyzed_sumw"] = swevents;
    rooto["events_yield_w"] = wevents;
    rooto["events_yield"] = y;
    rooto["Selection"]["Cut_0"]["Description"] = buff;
    rooto["Selection"]["Cut_0"]["Passed_i"] = c0;
    rooto["Selection"]["Cut_0"]["Passed_sumw"] = c0w;
    //rooto["Selection"]["Cut_1"]["Description"] = (isElectronDST) ? "MET Cut" : "MTW Cut";
    //rooto["Selection"]["Cut_1"]["Passed"] = c1;
    
    Json::StyledWriter writer;
    std::string outputConfig = writer.write( rooto );
    TString stemp;
    stemp=(rjfile.Remove(0,rjfile.Last('/')+1)).Data();
    if (stemp.EndsWith(".json")) stemp.Remove(stemp.Length()-5,stemp.Length());
    if (stemp.EndsWith(".root")) stemp.Remove(stemp.Length()-5,stemp.Length());
    stemp = ojdir+'/'+stemp + "-analysis.json";
    std::ofstream out(stemp);
    out << outputConfig << std::endl;
    out.close();
    std::cout << "analysis report written to: " << stemp << "\n";
    delete T; 
    return 0;        
}