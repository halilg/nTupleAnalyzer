#include <iostream>
#include <fstream>
#include "TChain.h"
#include "TTree.h"
#include "TString.h"
#include <json/json.h>

using namespace std;
TChain * json2tchain(TString treeName, TString fname){
    //bool debug = true;
    bool debug = false;
    TChain * mychain = new TChain(treeName);
    Json::Reader reader;
    Json::Value root;   // will contains the root value after parsing
    std::string line, inputConfig;
    std::ifstream myfile (fname);
    TString stemp;
    
    if (myfile.is_open()){
        while ( getline (myfile,line) ){ inputConfig += line; }
        myfile.close();
    } else {
      cerr << "Unable to open file " << fname << endl;
      delete mychain;
      exit(1);
    }    
    bool parsingSuccessful = reader.parse( inputConfig, root );
    
    if ( !parsingSuccessful ){
        // report to the user the failure and their locations in the document.
        cerr  << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();
        delete mychain;
        exit(1);
    }
    
    if (debug) cout << "json file contains pointers to " << root["rootfiles"].size() << " ROOT files" << endl;
    for (int i = 0; i<root["rootfiles"].size(); i++) {
        stemp=root["path"].asString() + "/" + root["rootfiles"][i].asString();
        cout << "adding to chain: " << stemp << endl;
        mychain->Add(stemp);
    }
    
    if (debug) cout << "chain contains " <<  mychain->GetEntries() << " events" << endl;
    return mychain;
}