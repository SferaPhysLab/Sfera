#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "TGraph.h"




int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./charge [rootFileName] [rootFileName(new)] . Questo programma confronta la carica calcolata dal programma con l'integrale della pulse shape per verificare un andamento lineare." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  std::string fileName2(argv[2]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");
  
  TFile* file2 = TFile::Open(fileName2.c_str(), "update");
  TTree* newtree = (TTree*)file2->Get("tree");
  
  int ev;
  int nch, ch_max;
  float pshape[128][1024];
  float vcharge[128];

  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  
  int newev;
  int newnch;
  float newpshape[128][1024];
  float newvcharge[128];
  

  newtree->SetBranchAddress( "ev" , &newev     );
  newtree->SetBranchAddress( "nch"   , &newnch    );
  newtree->SetBranchAddress( "pshape", &newpshape );
  newtree->Branch( "vcharge"   , newvcharge   , "newvcharge[newnch]/F"); //newnch o nch (nome root o nome variabile?) newnch ok

  
  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );

  int nentries = tree->GetEntries();
  float* x = new float;
  float* y = new float;
  float x_value = 0., y_value = 0.;
  float offset = 1.;
  
  TGraph *f = new TGraph(1, x, y);

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );


  TFile* outfile = TFile::Open(Form("%s/charge_all.root",plotsDir.c_str() ),"recreate");
  outfile->Close();

  tree->GetEntry(0);
  ch_max = nch;

  for(int channel = 14; channel < 15; channel++){  //modificare channel da 0 a ch_max

    if(fabs(pshape[channel][0] - offset) > 0.5){

      std::cout << "Channel "<< channel <<" is empty. Going on..." << std::endl;
      
    }else{

      std::cout << "Channel : "<< channel << std::endl;

      TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

      c1->cd();
  
      for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) { // iEntry 0 -> Evento 1

	tree->GetEntry(iEntry);
	newtree->GetEntry(iEntry);

	x_value = fabs( vcharge[channel]);

	for( unsigned i=0; i<1024; ++i ) 
	  h1->SetBinContent( i+1, newpshape[channel][i] );

	y_value = newvcharge[channel] = h1->GetSum();

	newtree->Fill(); //filla senza rovinare il resto?
    
	f->SetPoint(iEntry, x_value, y_value);

		if(x_value > 1000. && y_value < 8.){
	  std::cout << "Evento" << iEntry << std::endl;
		}
      } // for entries
  
      f->Draw("AP");

      f->SetMarkerStyle(21);
  
     
      //crea file di output  modificare charge_all
  
      TFile* outfile = TFile::Open(Form("%s/charge_14.root", plotsDir.c_str()), "update");
      outfile->cd();
      f->Write();
      outfile->Close();
      std::cout << "Output file saved: " << Form("%s/charge_all.root",plotsDir.c_str()) << std::endl;
      
      delete c1;
      
    } //else
  }//for channel

   file2->cd();
   newtree->Write();
  
  // file2->Close();

  delete f;
  delete h1;
  
  
  return 0;

}
