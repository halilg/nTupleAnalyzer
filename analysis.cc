#include "analysis.h"
#include <iostream>
#include <cmath>
#include "TMath.h"

analysis::analysis(){
    debug=false;
    //debug=true;
}

analysis::~analysis(){
    ;
}

void analysis::begin_analysis(const Json::Value &config, const event &myEvent){
    iEvent=0;
    iAcceptedEvents=0;
    c0=0;
    c0w=0;
    wevents = 0;
    swevents = 0;
    fillHistos=false;
    
    cut_ele_PT=config["cut_ele_PT"].asUInt();
    cut_ele_eta=config["cut_ele_eta"].asFloat();
    std::cout << "PT cut: " << cut_ele_PT << ", eta cut: " << cut_ele_eta << std::endl;
    
    if (myEvent.is_genw)
        h_gen_weight = new TH1D ("h_gen_weight","Event Weight (GEN); Weight (GeV); Events", 50, -400, 400);
    
    if (myEvent.is_mu){
        // book histograms
        h_muPt = new TH1D ("h_muPt","Muon Pt; PT (GeV); Muons", 25, 0, 100);
        h_muEta = new TH1D ("h_muEta","Muon eta; eta; Muons", 25, -4, 4);
        h_muMult = new TH1I("h_muMult","Muon Multiplicity; Multiplicity; Events", 6, 0, 5);
    }
    
    if (myEvent.is_ele) {
        // book histograms
        h_elePt = new TH1D ("h_elePt","Electron Pt; PT (GeV); Electrons", 25, 0, 100);
        h_eleEta = new TH1D ("h_eleEta","Electron eta; eta; Electrons", 25, -4, 4);
        h_eleMult = new TH1I("h_eleMult","Electron Multiplicity; Multiplicity; Events", 6, 0, 5);
        h_eleID = new TH1D("h_eleID","Electron ID; Electrons", 11, 0, 10);
    }
    
}

void analysis::new_event(const event &myEvent){
    using namespace std;
    ++iEvent;
    eventOK = false;
    c0OK = false;

    swevents += myEvent.gen_weight;
    if (fillHistos) h_gen_weight->Fill(myEvent.gen_weight);
    if (myEvent.is_mu){
        if (debug) cout << "Event: " << iEvent << ", muons: " << myEvent.mu_n << endl;
        if (fillHistos)h_muMult->Fill(myEvent.mu_n);
        for (int j=0; j< myEvent.mu_n; j++){
            if (fillHistos) h_muEta->Fill(myEvent.mu_eta[j]);
            if (fillHistos) h_muPt->Fill(TMath::Sqrt(myEvent.mu_px[j]*myEvent.mu_px[j]+myEvent.mu_py[j]*myEvent.mu_py[j]));
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
        if (debug) cout << "Event: " << iEvent << ", electrons: " << myEvent.ele_n << endl;
        if (fillHistos) h_eleMult->Fill(myEvent.ele_n, myEvent.gen_weight);
        double PT2;
        for (int j=0; j< myEvent.ele_n; j++){
            PT2 = myEvent.ele_px[0]*myEvent.ele_px[0] + myEvent.ele_py[0]*myEvent.ele_py[0];
            if (PT2 > cut_ele_PT*cut_ele_PT &&
                abs(myEvent.ele_eta[j]) < cut_ele_eta &&
                myEvent.ele_id[j] == 1){
                c0OK=true;
            }
            if (fillHistos) h_eleEta->Fill(myEvent.ele_eta[j], myEvent.gen_weight);
            if (fillHistos) h_elePt->Fill(TMath::Sqrt(myEvent.ele_px[j]*myEvent.ele_px[j]+myEvent.ele_py[j]*myEvent.ele_py[j]), myEvent.gen_weight);
            if (fillHistos) h_eleID->Fill(myEvent.ele_id[j], myEvent.gen_weight);
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
        iAcceptedEvents += 1;
    }
}

void analysis::end_analysis(Json::Value& rooto){
    char buff[100];
    sprintf(buff,"Max Electron PT > %i GeV)", cut_ele_PT);
// std::string buffAsStdStr = buff;
    
    //rooto["root_file"]=rjfile.Data();
    //rooto["root_tree_path"]=tdir.Data();
    //rooto["root_tree_name"] = ttree.Data();

    rooto["events_analyzed_sumw"] = swevents;
    rooto["events_yield_w"] = wevents;
    rooto["events_yield"] = iAcceptedEvents;
    rooto["Selection"]["Cut_0"]["Description"] = buff;
    rooto["Selection"]["Cut_0"]["Passed_i"] = c0;
    rooto["Selection"]["Cut_0"]["Passed_sumw"] = c0w;
}