#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"

#define N_soglia 50

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./noiseHisto [rootFileName] [channel]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  int channel(atoi(argv[2]));


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check spectrum of channel: " << channel << std::endl;

  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();

  int ev;
  int ch_max;
  float pshape[128][1024];


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "ch_max"   , &ch_max    );
  tree->SetBranchAddress( "pshape", &pshape );
 

  TH1D* h_noise = new TH1D("h_noise", "h_noise", 100, 0.99, 1.01 );

  int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

    /*if( channel>=nch ) {
      std::cout << "Event " << ev << " does not have channel " << channel << " (nch=" << nch << ")." << std::endl;
      exit(11);
      }*/

    std::cout << "Event : " << ev << std::endl;

    if (iEntry % 100 == 0) 
      std::cout << "Event : " << ev << std::endl;

    //std::cout << "charge = " << vcharge[channel] << std::endl;

    for(int i = 0; i < N_soglia; i++){
    h_noise->Fill(pshape[channel][i]);  // riempie istogramma della carica a canale fissato (fornito dall'utente)
    }
  } // for entries

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  TFile* outfile = TFile::Open(Form("%s/noise_histograms_ch%d.root",plotsDir.c_str(),channel),"recreate");
  outfile->cd();
  h_noise->Write();
  outfile->Close();
  std::cout << "Output file saved: " << Form("%s/noise_histograms_ch%d.root",plotsDir.c_str(),channel) << std::endl;

  return 0;

}
