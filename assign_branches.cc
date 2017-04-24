#include "event.h"
#include "TTree.h"

void assign_branches(TTree* myTree, event &myEvent){
    myEvent.is_mu=false;
    myEvent.is_ele=false;
    myEvent.is_phot=false;
    myEvent.is_jet=false;
    myEvent.is_MET=false;
    myEvent.is_genw=false;

    if (myTree->GetBranch("gen_weight")) {
        myEvent.is_genw=true;
        myTree->SetBranchAddress("gen_weight",&myEvent.gen_weight);
    }
    
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
    }
}