#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

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


std::vector<TString> split(TString text, const char & c){
    std::vector<TString> product;
    return product;
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
    Json::StyledWriter writer;
    std::string outputConfig;
    
    if (argc < 2){
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <configuration file>" << std::endl;
        return 1;
    }
    std::string cfgfile(argv[1]);
    
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

    /////////////////////////////////////////////////////////

    // Read command line arguments
    for (int i=2; i<argc; ++i){
        TString arg(argv[i]);
        Ssiz_t j = arg.First('=');
        TString cfgvar=arg;
        cfgvar.Remove(j,arg.Length()-j);
        TString cfgval(arg);
        cfgval.Remove(0, j+1);
        
        TString cfgvar1(cfgvar);
        TString cfgvar2(cfgvar);
        Ssiz_t k = cfgvar.First(':');
        cfgvar1.Remove(k,arg.Length()-k);
        cfgvar2.Remove(0, k+1);
        
        //cout << i << ": " << arg << ", j=" << j << ", var=" << cfgvar << ", val=" << cfgval << endl;
        //cout << "v1=" << cfgvar1 << ", v2=" << cfgvar2 << endl;
        cout << cfgvar1 << ":" << cfgvar2 << " : " << rooti[cfgvar1][cfgvar2] << " -> " << cfgval << endl;
        if (rooti[cfgvar1][cfgvar2].isUInt() || rooti[cfgvar1][cfgvar2].isInt()){
            rooti[cfgvar1][cfgvar2]=atoi(cfgval.Data());
        } else if (rooti[cfgvar1][cfgvar2].isDouble()){
            rooti[cfgvar1][cfgvar2]=atof(cfgval.Data());
        } else{
            rooti[cfgvar1][cfgvar2]=cfgval.Data();
        }
    }
    //outputConfig = writer.write( rooti );
    //cout << outputConfig << endl;
    
    long long int nevents=rooti["framework"]["maxEvents"].asInt64();
    TString rjfile(rooti["framework"]["inRootFile"].asString());
    TString tdir(rooti["framework"]["treeDir"].asString());
    TString ttree(rooti["framework"]["treeName"].asString());
    TString orfile(rooti["framework"]["outRootFile"].asString());
    TString ojfile(rooti["framework"]["outJSONFile"].asString());
    if (debug) std::cout << nevents <<   std::endl
              << rjfile << std::endl
              << orfile << std::endl
              << ojfile << std::endl
              << tdir << std::endl
              << ttree << std::endl;
    
    //if (argc > 2){
        //minpt=std::atoi(argv[2]);
    //}    
    //if (debug) std::cout << "command line argument: " << minpt << std::endl;

    // override cfg if command line value provided
    //if (minpt != -1) rooti["analysis"]["cut_ele_PT"] = minpt;

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
    TString ofname=orfile;//(orfile+TString("/h-")+basename(rjfile.Data()));
    //if (ofname.EndsWith(".json")) ofname=ofname+TString(".root");

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
    nevents= (nevents < nentries) ? nevents : nevents; // std::min(nevents,nentries);
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
    outputConfig = writer.write( rooto );
    //stemp=(rjfile.Remove(0,rjfile.Last('/')+1)).Data();
    //if (stemp.EndsWith(".json")) stemp.Remove(stemp.Length()-5,stemp.Length());
    //if (stemp.EndsWith(".root")) stemp.Remove(stemp.Length()-5,stemp.Length());
    //stemp = ojfile+'/'+stemp + "-analysis.json";
    std::ofstream out(ojfile);
    out << outputConfig << std::endl;
    out.close();
    std::cout << "analysis report written to: " << ojfile << "\n";
    delete T; 
    return 0;        
}