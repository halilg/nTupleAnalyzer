#!/usr/bin/env python
import os, sys, json
from ROOT import TGraph, TCanvas, TLegend, gROOT
from array import array

gROOT.SetBatch(True)
pt=[00,01,02,04,05,07,10,15,18,20,22,24,26,30,35,40,50,75]
stfn="id-1-nTuple_ST_multi-analysis.json"
qcdfn="id-1-nTuple_QCD_multi-analysis.json"

def readJSON(fname):
    data=file(fname).readlines()
    data = "".join(data)
    return json.loads(data)


c1 = TCanvas()
c1.SetGrid()

stvals=[]
qcdvals=[]

print "  PT   Eff_Sig  Eff_QCD"
print "(GeV)"
print " ----------------------"

for ptv in pt:
    prefix="pt%02i-" % ptv
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
    
graphst =TGraph ( len(stvals) , array("f", pt) , array("f", stvals)) ;
graphqcd =TGraph ( len(qcdvals) , array("f", pt) , array("f", qcdvals)) ;

graphcorr =TGraph ( len(qcdvals) , array("f", qcdvals) , array("f", stvals)) ;
    
graphst.SetMarkerStyle(22)
graphst.SetMarkerColor(4)
graphqcd.SetMarkerStyle(21)
graphqcd.SetMarkerColor(8)

graphst.SetTitle("Electron P_{T} Cut Efficiency")
graphst.GetXaxis().SetTitle("P_{T} (GeV)")
graphst.GetYaxis().SetTitle("Efficiency")

leg = TLegend(0.6,0.75,0.9,0.9);
# leg->SetHeader("The Legend Title");
leg.AddEntry(graphst,"Single top t-channel","lp");
leg.AddEntry(graphqcd,"QCD","lp");


graphst.Draw()
graphqcd.Draw("SAME PL")
leg.Draw();

c1.Print("trigger.png")

c1.Clear()

graphcorr.SetTitle("QCD Efficiency vs Signal Efficiency")
graphcorr.GetXaxis().SetTitle("QCD Efficiency")
graphcorr.GetYaxis().SetTitle("Signal Efficiency")
graphcorr.SetMarkerColor(4)
graphcorr.Draw("CA*")
c1.Print("corr.png")

