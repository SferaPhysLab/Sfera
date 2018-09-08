#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"

#define N_soglia 50

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./baseline_pulseshape [rootFileName] [/path/rootFileName (type noise_histograms_filtered)]. Nuovo tree sottraendo la baseline e calcolando la carica." << std::endl;
    exit(1);
  } 

  std::string fileName(argv[1]);
  std::string fileName2(argv[2]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  TFile* file2 = TFile::Open(fileName2.c_str());

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check noise of all non-empty channels " << std::endl;

 
  int ev, newev;
  int nch, newnch, ch_max, channel;
  float pshape[128][1024];
  float vamp[128];
  float newpshape[128][1024];
  float newvcharge[128];
  float newvamp[128];
  float mean_baseline[128]; 


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vamp", &vamp );
  

   size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }
  
  //crea file di output
  TFile* outfile = TFile::Open(Form("%s_new.root", prefix.c_str() ),"recreate");
  TTree *newtree =  new TTree( "tree", "" );

  newtree->Branch( "ev"       , &newev      , "newev/I"            );
  newtree->Branch( "nch"      , &newnch     , "newnch/I"           );
  newtree->Branch( "pshape"   , newpshape   , "newpshape[newnch][1024]/F");
  newtree->Branch( "vcharge"   , newvcharge   , "newvcharge[newnch]/F");
  newtree->Branch( "vamp"   , newvamp   , "newvamp[newnch]/F");
  
  tree->GetEntry(0);
  ch_max = nch;

  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );

  
  for(channel = 0; channel < ch_max; channel++){


      std::cout << "Channel : "<< channel << std::endl;

      TH1D* h_noise_filtered = (TH1D*)file2->Get(Form("h_noise_filtered_%d", channel)); 

      mean_baseline[channel] = h_noise_filtered->GetMean();

      delete h_noise_filtered;

      std::cout << "Media: " << mean_baseline[channel] << std::endl;
  }

      int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

       
      for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

	tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

	newev = ev;
	newnch = nch;
	
        if( ev % 1000 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;

	for( channel = 0; channel < ch_max; channel++){
	  
	  for(int i = 0; i < 1024; i++){
	  
	    newpshape[channel][i] = mean_baseline[channel] - pshape[channel][i];  // media baseline a canale fissato (fornito dall'utente)
	     h1->SetBinContent( i+1, newpshape[channel][i] );
	  }
	   newvcharge[channel] = h1->GetSum();
	   newvamp[channel] = vamp[channel];
	}

	newtree->Fill();
	
      } // for entries  


      newtree->Write();
      outfile->Close();

      std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;
  
      return 0;

}
