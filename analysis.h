#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "TH1D.h"
#include "TH1I.h"
#include <json/json.h>
#include "event.h"

class analysis {
    // private variables
    bool debug;
    Long64_t iEvent;
    Long64_t iAcceptedEvents;
    Long64_t c0;
    double c0w;
    unsigned int cut_ele_PT; // GeV
    float cut_ele_eta;
    bool eventOK;
    bool c0OK;
    bool fillHistos;
    double wevents;
    double swevents;
    
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
    
    public:
        analysis();
        ~analysis();
        void new_event(const event&);
        void begin_analysis(const Json::Value&, const event&);
        void end_analysis(Json::Value&);
};

#endif