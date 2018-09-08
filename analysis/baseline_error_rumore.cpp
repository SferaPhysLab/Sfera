#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"

#define N_soglia 50

int main( int argc, char* argv[] ) {

  if( argc!= 2 ) {
    std::cout << "USAGE: ./baseline_error_rumore [rootFileName]. Sottrae la media dei primi 50 punti agli eventi di solo rumore, calcola la carica a mano (wavecatcher dà zero spaccato, ma poiché le due definizioni di carica coincidono a cariche != 0, si pensa vada bene uguale) per verificare che non ci siano shift sul valore della carica, ma che sia centrata in zero con la risoluzione." << std::endl;
    exit(1);
  } 

  std::string fileName(argv[1]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check noise of all non-empty channels " << std::endl;

 
  int ev;
  int nch, ch_max, channel;
  float pshape[128][1024];
  float vcharge[128];
  float vamp[128];
  float newpshape[128][1024];
  float newvcharge[128];
  float baseline[128]; 


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  //  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "vamp", &vamp );  

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );
 
  
  //crea file di output
  TFile* outfile = TFile::Open(Form("%s/charge_error.root",plotsDir.c_str()),"recreate");

  outfile->Close();
  
  tree->GetEntry(0);
  ch_max = nch;

  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );



  int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

  for( channel = 0; channel < ch_max; channel++){


    TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

    TH1D* h2 = new TH1D(Form("h2_%d", channel), "", 100, -21., 21.);

    c1->cd();
       
    for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

      tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)
	
      //if( ev % 1000 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;

        if(fabs(vcharge[channel]) < 20.){

	  //	std::cout << vcharge[channel] << " ";

	/*	baseline[channel] = 0.;
      
	for(int i = 0; i < N_soglia; i++){
	  baseline[channel] += pshape[channel][i];
	}
	  
	baseline[channel]/=N_soglia;
    
	for(int i = 0; i < 1024; i++){
	  
	  newpshape[channel][i] = baseline[channel] - pshape[channel][i];  // media baseline a canale fissato (fornito dall'utente)
	  h1->SetBinContent( i+1, newpshape[channel][i] );
	}
	newvcharge[channel] = h1->GetSum();

	h2->Fill(newvcharge[channel]);*/

	h2->Fill(vcharge[channel]);
	}
	   
    } //for entries

    c1->SetGrid();
     
            
    TFile* outfile = TFile::Open(Form("%s/charge_error.root",plotsDir.c_str()),"update");
    outfile->cd();
    h2->Write();
    outfile->Close();
    std::cout << "Output file saved: " << Form("%s/charge_error.root",plotsDir.c_str()) << std::endl;

    delete h2;
    delete c1;
    
	
  } // for channels  
 
  
  return 0;

}
