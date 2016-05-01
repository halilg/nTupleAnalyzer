#!/usr/bin/env python
import os, sys, json
from ROOT import TGraph, TMultiGraph, TCanvas, TLegend, gROOT
from array import array

# gROOT.SetBatch(True)
pt=[00,01,02,04,05,07,10,15,18,20,22,24,26,30,35,40,50,75]
etas=["2.1", "2.5"]
isols=["-1", "0"]
stfn="nTuple_ST_multi-analysis.json"
qcdfn="nTuple_QCD_multi-analysis.json"

def readJSON(fname):
    data=file(fname).readlines()
    data = "".join(data)
    return json.loads(data)


c1 = TCanvas()
c1.SetGrid()



stdata={}
qcddata={}

for eta in etas:
    stdata[eta]={}
    qcddata[eta]={}
    for iso in isols:
        stvals=[]
        qcdvals=[]
        print eta, iso
        print "  PT   Eff_Sig  Eff_QCD"
        print "(GeV)"
        print " ----------------------"
        for ptv in pt:
            prefix="pt%02i-id%s-eta%s-" % (ptv, iso, eta)
            fnamest=prefix + stfn
            fnameqcd=prefix + qcdfn
            datast=readJSON(fnamest)
            dataqcd=readJSON(fnameqcd)
            
            wsumst=float(datast["events_analyzed_sumw"])
            wpassedst=float(datast["events_yield_w"])
            
            wsumqcd=float(dataqcd["events_analyzed_sumw"])
            wpassedqcd=float(dataqcd["events_yield_w"])
            
            
            
            stvals.append(wpassedst/wsumst)
            qcdvals.append(wpassedqcd/wsumqcd)
            
            print "  %2d   %6.4f   %7.5f" % (ptv, stvals[-1], qcdvals[-1])
        stdata[eta][iso]=stvals
        qcddata[eta][iso]=qcdvals
        
# graphst_iso_eta21 =TGraph ( len(stvals) , array("f", pt) , array("f", stdata[2.1][0])) ;
# graphqcd_iso_eta21 =TGraph ( len(qcdvals) , array("f", pt) , array("f", qcddata[2.5][0])) ;

    
# graphst_iso_eta21.SetMarkerStyle(22)
# graphst_iso_eta21.SetMarkerColor(4)
# graphqcd_iso_eta21.SetMarkerStyle(21)
# graphqcd_iso_eta21.SetMarkerColor(8)

# graphst_iso_eta21.SetTitle("Electron P_{T} Cut Efficiency")
# graphst_iso_eta21.GetXaxis().SetTitle("P_{T} (GeV)")
# graphst_iso_eta21.GetYaxis().SetTitle("Efficiency")

# graphst_iso_eta21.Draw()
# graphqcd_iso_eta21.Draw("SAME PL")
# leg.Draw();

# c1.Print("trigger.png")
graphcorr0 =TGraph ( len(stdata["2.1"]["0"]) , array("f", qcddata["2.1"]["0"]) , array("f", stdata["2.1"]["0"])) 
graphcorr1 =TGraph ( len(stdata["2.1"]["-1"]) , array("f", qcddata["2.1"]["-1"]) , array("f", stdata["2.1"]["-1"])) 
graphcorr2 =TGraph ( len(stdata["2.5"]["0"]) , array("f", qcddata["2.5"]["0"]) , array("f", stdata["2.5"]["0"])) 
graphcorr3 =TGraph ( len(stdata["2.5"]["-1"]) , array("f", qcddata["2.5"]["-1"]) , array("f", stdata["2.5"]["-1"])) 


graphcorr0.SetLineColor(4);
graphcorr1.SetLineColor(4);
graphcorr2.SetLineColor(8);
graphcorr3.SetLineColor(8);



c1.Clear()

graphcorr0.SetTitle("QCD Efficiency vs Signal Efficiency")
# graphcorr0.SetMarkerColor(4)

mg0=TMultiGraph()
mg0.SetTitle("Signal Efficiency vs. QCD Efficiency (Isolated electrons);QCD Efficiency;Signal Efficiency");
mg1=TMultiGraph()
mg1.SetTitle("Signal Efficiency vs. QCD Efficiency;QCD Efficiency;Signal Efficiency");
# mg.GetXaxis().SetTitle("QCD Efficiency")
# mg.GetYaxis().SetTitle("Signal Efficiency")
mg0.Add(graphcorr0)
mg0.Add(graphcorr2)
mg1.Add(graphcorr1)
mg1.Add(graphcorr3)

leg0 = TLegend(0.70,0.55,0.9,0.7);
leg1 = TLegend(0.70,0.55,0.9,0.7);
# leg->SetHeader("Isolated electrons");
leg0.AddEntry(graphcorr0,"|#eta| < 2.1","lp");
leg0.AddEntry(graphcorr2,"|#eta| < 2.5","lp");
leg1.AddEntry(graphcorr1,"|#eta| < 2.1","lp");
leg1.AddEntry(graphcorr3,"|#eta| < 2.5","lp");



mg0.Draw("ac")
leg0.Draw()
c1.Print("corr_iso.png")
c1.Clear()
mg1.Draw("ac")
leg1.Draw()
c1.Print("corr_noiso.png")