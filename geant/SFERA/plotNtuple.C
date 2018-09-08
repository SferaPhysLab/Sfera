// ROOT macro file for plotting example B3 ntuple
// 
// Can be run from ROOT session:
// root[0] .x plotNtuple.C

{
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  
  // Draw histos filled by Geant4 simulation 
  //   

  // Open file filled by Geant4 simulation 
  TFile f("B3.root");

  // Create a canvas and divide it into 2x2 pads
  TCanvas* c1 = new TCanvas("c1", "", 20, 20, 1000, 1000);
  
  // Get ntuple
  TNtuple* ntuple = (TNtuple*)f.Get("g4data");

  // Draw Eabs histogram in the pad 1
  ntuple->Draw("Total Eabs");
}  
