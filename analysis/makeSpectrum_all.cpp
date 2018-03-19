#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"


int main( int argc, char* argv[] ) {

  if( argc!= 2 ) {
    std::cout << "USAGE: ./makeSpectrum [rootFileName]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check spectrum of channel all non-empty channels " << std::endl;


  int ev;
  int nch, ch_max;
  float pshape[128][1024];
  float vcharge [128];
  float offset = 1.;
  
  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );

  tree->GetEntry(0);
  ch_max = nch;

  //crea cartella dove mettere i plot se non presente
  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  //crea file di output

  TFile* outfile = TFile::Open(Form("%s/histograms.root",plotsDir.c_str()),"recreate");

  outfile->Close();

   
  for(int channel = 0; channel < ch_max; channel++){

    if(fabs(pshape[channel][0] - offset) > 0.5){

      std::cout << "Channel "<< channel <<" is empty. Going on..." << std::endl;
      
    }else{

      std::cout << "Channel : "<< channel << std::endl;

      TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

      c1->cd();
  
      TH1D* h_charge = new TH1D(Form("h_charge_%d", channel), Form("h_charge_%d", channel), 100, -100, 20000 );
  
      int nentries = tree->GetEntries();  //numero di righe della tabella (tree)

      for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

	tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

	/* if( channel>=nch ) {
	   std::cout << "Event " << ev << " does not have channel " << channel << " (nch=" << nch << ")." << std::endl;
	   exit(11);
	   }*/


	if (iEntry % 100 == 0) 
	  std::cout << "Event : " << ev << std::endl;

	h_charge->Fill(fabs(vcharge[channel]));  // riempie istogramma della carica a canale fissato (fornito dall'utente)

      } // for entries


      TFile* outfile = TFile::Open(Form("%s/histograms.root",plotsDir.c_str()),"update");
      outfile->cd();
      h_charge->Write();
      outfile->Close();
      std::cout << "Output file saved: " << Form("%s/histograms.root",plotsDir.c_str()) << std::endl;

      delete h_charge;
      delete c1;

    } //else 
    
  }//for channel
  

  return 0;

}
