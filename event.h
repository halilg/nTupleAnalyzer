#ifndef EVENT_H
#define EVENT_H

#define maxReco 30
#define maxHLT 255

class event {
    // private variables
    public:
        // gen variables
        bool is_genw;        
        double gen_weight;

        // Muon variables
        bool is_mu;
        int mu_n;
        int mu_charge[maxReco];
        double mu_px[maxReco];
        double mu_py[maxReco];
        double mu_pz[maxReco];
        double mu_energy[maxReco];
        double mu_phi[maxReco];
        double mu_theta[maxReco];
        double mu_eta[maxReco];  
        double mu_id[maxReco];
        double mu_relIso[maxReco];

        // Electron variables
        bool is_ele;
        int ele_n;
        int ele_charge[maxReco];
        double ele_px[maxReco];
        double ele_py[maxReco];
        double ele_pz[maxReco];
        double ele_energy[maxReco];
        double ele_phi[maxReco];
        double ele_theta[maxReco];
        double ele_eta[maxReco];
        double ele_id[maxReco];
        double ele_relIso[maxReco];
 
        // Photon variables
        bool is_phot;
        int phot_n;
        double phot_px[maxReco];
        double phot_py[maxReco];
        double phot_pz[maxReco];
        double phot_energy[maxReco];
        double phot_phi[maxReco];
        double phot_theta[maxReco];
        double phot_eta[maxReco];

        // Jet variables
        bool is_jet;
        int jet_n;
        double jet_energy[maxReco];
        double jet_px[maxReco];
        double jet_py[maxReco];
        double jet_pz[maxReco];
        double jet_phi[maxReco];
        double jet_theta[maxReco];
        double jet_eta[maxReco];
        //double jet_nhf[maxReco];
        //double jet_nef[maxReco];
        //double jet_chf[maxReco];
        //double jet_cef[maxReco];
        //int jet_nconstituents[maxReco];
        //int jet_nch[maxReco];
        //double jet_pfCombinedInclusiveSecondaryVertexV2BJetTags[maxReco];
        double jet_btagd[maxReco];

        // MET variables
        bool is_MET;
        //double MET_E;
        double MET_px;
        double MET_py;
        //double MET_pz;
        double MET_phi;
        double MET_theta;
        //double MET_eta;

        size_t HLT_n;
        bool HLT[maxHLT];

};

#endif
