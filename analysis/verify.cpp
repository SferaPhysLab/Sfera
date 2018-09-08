#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "TGraph.h"




int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./charge [rootFileName] [channel]. Questo programma confronta la carica calcolata dal programma e l'ampiezza del picco per verificare un andamento lineare." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  int channel(atoi(argv[2]));
  
  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");
  
  int ev;
  int nch, ch_max;
  float pshape[128][1024];
  float vcharge[128];
  float vamp[128];
  
  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "vamp", &vamp );

  int nentries = tree->GetEntries();
  float* x = new float;
  float* y = new float;
  float x_value = 0., y_value = 0.;
  
  TGraph *f = new TGraph(1, x, y);

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );


  TFile* outfile = TFile::Open(Form("%s/vamp_charge.root",plotsDir.c_str() ),"recreate");

  tree->GetEntry(0);
  ch_max = nch;


  TCanvas* c1 = new TCanvas("c1","c1", 600, 600);

      c1->cd();
  
      for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) { // iEntry 0 -> Evento 1

	tree->GetEntry(iEntry);

	x_value = fabs( vcharge[channel]);

	y_value = fabs(vamp[channel]);
    
	f->SetPoint(iEntry, x_value, y_value);

      } // for entries
  
      f->Draw("AP");

      f->SetMarkerStyle(21);
  
     
      //crea file di output  modificare charge_all
  
      outfile->cd();
      f->Write();
      outfile->Close();
      std::cout << "Output file saved: " << Form("%s/vamp_charge.root",plotsDir.c_str()) << std::endl;
      
      delete c1;

  delete f;
  
  
  return 0;

}
