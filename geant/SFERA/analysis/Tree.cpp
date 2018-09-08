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

  TTree *t1 = (TTree*)Fin->Get("g4data");
  TTree *t2 = new TTree("tree","tree"); 
  
  long int nentries = t1->GetEntries();
  
  double Edep[17];
  float Ni[17];
  float Emod[17];

  t1->SetBranchAddress("Eabs0", Edep);
  t1->SetBranchAddress("Eabs1", (Edep + 1));
  t1->SetBranchAddress("Eabs2", (Edep + 2));
  t1->SetBranchAddress("Eabs3", (Edep + 3));
  t1->SetBranchAddress("Eabs4", (Edep + 4));
  t1->SetBranchAddress("Eabs5", (Edep + 5));
  t1->SetBranchAddress("Eabs6", (Edep + 6));
  t1->SetBranchAddress("Eabs7", (Edep + 7));
  t1->SetBranchAddress("Eabs8", (Edep + 8));
  t1->SetBranchAddress("Eabs9", (Edep + 9));
  t1->SetBranchAddress("Eabs10", (Edep + 10));
  t1->SetBranchAddress("Eabs11", (Edep + 11));
  t1->SetBranchAddress("Eabs12", (Edep + 12));
  t1->SetBranchAddress("Eabs13", (Edep + 13));
  t1->SetBranchAddress("Eabs14", (Edep + 14));
  t1->SetBranchAddress("Eabs15", (Edep + 15));
  t1->SetBranchAddress("EabsTotal", (Edep + 16));

  /*
  TCanvas* c = new TCanvas("c","");
  c->cd();

  TH1D* h = new TH1D("h","",20,10,1500);
  t1->Project("h","Eabs0");
  h->Draw();

  c->SaveAs("histo.pdf");
  */

  t2->Branch("Nfotoel", Ni, "Nfotoel[17]/F");
  t2->Branch("Emod", Emod, "Emod[17]/F");
  //  TBranch *Nfotoel_branch = t2->GetBranch("Nfotoel");

  //efficienza fotoni di scintillazione
  float Ly = 38.; // (fotoni/keV)
//  float e_geo = 0.196;
  float e_geo = .1;
  float e_pmt = 0.15;
  float e = e_geo * e_pmt;
  //fluttuazioni guadagno
  float enf = 1.4;

//  enf = 29.3603 / 657.489 * sqrt(661.7 * Ly * e); // cesio
  enf = 26.2529 / 512.531 * sqrt(511 * Ly * e); // sodio

  float Nfotoel;
  float sigma;
  
  int i = 0;

  for ( i=0; i<nentries; i++ ) {

    t1->GetEntry(i);
    
    for (int j=0; j<17; j++) {
      if (Edep[j]!=0) {
	
	Nfotoel = Ly * e * Edep[j];
	sigma = enf * sqrt(Nfotoel);

	TF1 *gauss = new TF1("gauss", "gaus", Nfotoel/3., 3.*Nfotoel);

	gauss->SetParameter(0, 1/(sigma*sqrt(2.*TMath::Pi())));
	gauss->SetParameter(1, Nfotoel);
	gauss->SetParameter(2, sigma);
	Ni[j] = gauss->GetRandom();
	Emod[j] = Ni[j] / (Ly * e);
	//std::cout << "Nfotoel: " << Nfotoel << " \t Ni: " << Ni[j] << std::endl;

	delete gauss;
	
      } else {

	Ni[j] = 0;
	Emod[j] = 0;

      }
    }

    t2->Fill();

    if (!(i % 10000)) std::cout << "Events filled: " << i << std::endl;
  }

  std::cout << "Events filled: " << i << std::endl;

  //  t2->Scan("Nfotoel[1]");
  t2->Write();
  
  //  Nfotoel_branch->SetFile("dataTree.root");
  //  t2->Write();
  
  Fout->Close();
  Fin->Close();
  
  return 0;
}
