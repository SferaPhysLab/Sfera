#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TBranch.h"

#include <iostream>


int main( int argc, char* argv[] ) {

  if( argc != 2 ) {
    std::cout << "USAGE: ./Tree [rootfile]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  
  TFile *Fin = TFile::Open(fileName.c_str());
  TFile *Fout = new TFile("dataTree.root","recreate");

  TTree *t1 = (TTree*)Fin->Get("B3");
  TTree *t2 = new TTree("tree","tree"); 
  
  long int nentries = t1->GetEntries();
  
  double Edep;
  float Ni;
  float Emod;

  t1->SetBranchAddress("Eabs0", &Edep);

  t2->Branch("Nfotoel", &Ni, "Nfotoel/F");
  t2->Branch("Emod", &Emod, "Emod/F");

  //efficienza fotoni di scintillazione
  float Ly = 38000.; // (fotoni/MeV)
//  float e_geo = 0.338;
  float e_geo = 0.1;
  float e_pmt = 0.15;
  float e = e_geo * e_pmt;
  //fluttuazioni guadagno
//  float enf = 1.4;
  float enf = 37.582 / 958.804 * sqrt(e * 0.6617 * Ly);

  float Nfotoel;
  float sigma;
  
  for ( int i=0; i<nentries; i++ ) {

    t1->GetEntry(i);
    
    if (Edep != 0.) {
	
      Nfotoel = Ly * e * Edep;
      sigma = enf * sqrt(Nfotoel);
      
      TF1 *gauss = new TF1("gauss", "gaus", Nfotoel/3., 3.*Nfotoel);

      gauss->SetParameter(0, 1/(sigma*sqrt(2.*TMath::Pi())));
      gauss->SetParameter(1, Nfotoel);
      gauss->SetParameter(2, sigma);
      Ni = gauss->GetRandom();
      Emod = Ni / (Ly * e);

      delete gauss;
      
    } else {

      Ni= 0;
      Emod = 0;
      
    }
    

    t2->Fill();

    if (!(i % 10000)) std::cout << "Events filled: " << i << std::endl;
  }

  t2->Write();
  
  Fout->Close();
  Fin->Close();
  
  return 0;
}
