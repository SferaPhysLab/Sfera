// ROOT macro file for plotting example B3 histograms 
// 
// Can be run from ROOT session:
// root[0] .x plotHisto.C

{
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  
  // Draw histos filled by Geant4 simulation 
  //   

  // Open file filled by Geant4 simulation 
  TFile f("g4data.root");

  // Create a canvas
  TCanvas* c1 = new TCanvas("c1", "", 20, 20, 1000, 1000);
  
  // Draw Eabs histogram 
  TH1D* hist1 = (TH1D*)f.Get("Total Eabs");
  hist1->Draw();
}  
