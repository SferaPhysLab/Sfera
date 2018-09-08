#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream> //necessario per ifstream ofstream

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TF1.h>
#include <TSpectrum.h>


int main( int argc, char* argv[] ) {

  if( argc!= 2 ) {
    std::cout << "USAGE: ./time_resolution [rootFileName_new_time] (type Na22 in AND). Prende i tempi associati alle pulseshape e riempie un istogramma con le differenze tra canali opposti." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check the time resolution."<< std::endl;

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  TFile* outfile = TFile::Open(Form("%s/time_histogram_all.root",plotsDir.c_str()),"recreate");

  TFile* outfile2 = TFile::Open(Form("%s/time_histograms.root",plotsDir.c_str()),"recreate");

  outfile2->Close();

  double PShapeTime[16];
  
  tree->SetBranchAddress( "PShapeTime", &PShapeTime);

  TH1D* h_time_all = new TH1D("h_time_all", "", 64, -20, 20 );

  for(int i = 0; i<8; i++){ //ciclo sulle coppie di canali opposti (sui canali output 0 e 1 ci sono i ch 0 e 8...)
  
    int nentries = tree->GetEntries();

    TCanvas* c1 = new TCanvas(Form("c1_%d", i), "", 600, 600);

    c1->cd();

    TH1D* h_time = new TH1D(Form("h_time_%d", i), "", 64, -20., 20. );

    for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

      tree->GetEntry(iEntry);

      if(PShapeTime[i*2] > 0.1 && PShapeTime[i*2 + 1] > 0.1){

	h_time->Fill(PShapeTime[i*2] - PShapeTime[i*2 + 1]);
	h_time_all->Fill(PShapeTime[i*2] - PShapeTime[i*2 + 1]);
      }
    
    } // for entries

    h_time->GetXaxis()->SetTitle("Time Difference[ns]");
    h_time->GetYaxis()->SetTitle("Events");
  

    TFile* outfile2 = TFile::Open(Form("%s/time_histograms.root" ,plotsDir.c_str()),"update");
    outfile2->cd();
    h_time->Write();
    outfile2->Close();
    std::cout << "Output file saved: " << Form("%s/time_histograms.root",plotsDir.c_str()) << std::endl;

    std::ofstream f;
    f.open("tresVSthresh.dat", std::ios_base::app);

    f << h_time->GetStdDev() << " ";

    f.close();
  
    delete h_time;
    delete c1;

  
  }

  h_time_all->GetXaxis()->SetTitle("Time Difference[ns]");
  h_time_all->GetYaxis()->SetTitle("Events");
  
  outfile->cd();
  h_time_all->Write();
  outfile->Close();
  std::cout << "Output file saved: " << Form("%s/time_histogram_all.root",plotsDir.c_str()) << std::endl;  

  std::ofstream f;
  f.open("tresVSthresh.dat", std::ios_base::app);

  f << h_time_all->GetStdDev() << std::endl;

  f.close();
  
  delete h_time_all;
  
  return 1;

}
