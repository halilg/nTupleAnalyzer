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
#include <json/json.h>
#include <json2tchain.h>

// https://github.com/halilg/singleTopAnalysis/blob/master/analysis.cc
// Main analyzer. Reads a single ROOT tree, applies cuts, reports cut efficiencies and generates histograms
//
void assign_branches(TTree* , event &);


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
    
    long long int nevents=rooti["framework"]["maxEvents"].asInt64();
    TString rjfile(rooti["framework"]["inRootFile"].asString());
    TString tdir(rooti["framework"]["treeDir"].asString());
    TString ttree(rooti["framework"]["treeName"].asString());
    TString ordir(rooti["framework"]["outRootDir"].asString());
    TString ojdir(rooti["framework"]["outJSONDir"].asString());
    if (debug) std::cout << nevents <<   std::endl
              << rjfile << std::endl
              << ordir << std::endl
              << ojdir << std::endl
              << tdir << std::endl
              << ttree << std::endl;

    /////////////////////////////////////////////////////////
    //return 0;

    
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
    
    TTree* myTree = T; //->GetTree();
    assign_branches(myTree, myEvent);
    myAnalysis.begin_analysis(rooti["analysis"], myEvent);
    
    Long64_t nentries = myTree->GetEntries();
    if (nentries == 0){
        cerr << "ROOT tree had no events\n";
        return 0;
    }
    cout << "Entries in ROOT tree: " << nentries << endl;
    
    // determine the number of events to analyze
    nevents= (nevents < nentries) ? nevents : nentries; // std::min(nevents,nentries);
    if (nevents == -1) nevents = nentries;
    
    cout << "Will analyze " << nevents << " events\n";     //" events\n 1000X:";    
    Long64_t i;
    for (i=0; i<nevents; i++){
        myTree->GetEntry(i);
        myAnalysis.new_event(myEvent);
    }
    E.Write();
    E.Close();
    
    myAnalysis.end_analysis(rooto);
    rooto["events_total"] = nentries;
    rooto["events_analyzed"] = nevents;
    rooto["configuration"]["framework"]=rooti["framework"];
    rooto["configuration"]["analysis"]=rooti["analysis"];

    cout << "\nAnalyzed " << i << " entries\n";
    cout << "histograms written to: " << ofname << endl;
    
    // write report to JSON file
    
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