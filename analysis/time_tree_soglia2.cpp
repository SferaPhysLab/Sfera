#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <TF1.h>
#include <TSpectrum.h>

//Na22_And charge min 14 max 18
//Na_Time charge min 20 max 100

#define CHARGE_MIN 20 //mettere i valori tree_new
#define CHARGE_MAX 100
#define CONV 0.312 //1 sample = 0.312 ns

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./time_tree_soglia2 [rootFileName_new] [threshold] (type Na22 in AND) . Crea un nuovo tree contenente solo gli eventi interessanti (carica intorno al picco del Na22) e aggiunge le informazioni temporali: tempo associato al segnale stimato da un algoritmo che prende una frazione data del massimo." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  double threshold =(double)atof(argv[2]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");


  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
 
  int ev, newev;
  int nch, newnch, ch_max, channel;
  float pshape[128][1024];
  float vcharge[128];
  float vamp[128];
  float newvcharge[16];
  double newvamp[16];
  double newPShapeTime[16];
  bool selection;
  double timepeak;

  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "vamp", &vamp ); 

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }
  
  //crea file di output
  TFile* outfile = TFile::Open(Form("%s_time2.root", prefix.c_str() ),"recreate");
  TTree *newtree =  new TTree( "tree", "" );

  newtree->Branch( "ev"       , &newev      , "newev/I"            );
  newtree->Branch( "nch"      , &newnch     , "newnch/I"           );
  newtree->Branch( "vcharge"   , newvcharge   , "newvcharge[newnch]/F");
  newtree->Branch( "vamp"   , newvamp   , "newvamp[newnch]/D");
  newtree->Branch( "PShapeTime"   , newPShapeTime   , "newPShapeTime[newnch]/D");
  
  tree->GetEntry(0);
  ch_max = nch;

  int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );
       
  for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

    tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

    selection = false;
	 
    for( channel = 0; channel < ch_max; channel++){

      if(vcharge[channel] < CHARGE_MAX && vcharge[channel] > CHARGE_MIN){ 

	selection = true;

	newvcharge[channel] = vcharge[channel];
	newvamp[channel] = (double)vamp[channel];

	 
	for( unsigned i=0; i<1024; i++ ) {
	  h1->SetBinContent( i+1, pshape[channel][i]);
	}
   
	newPShapeTime[channel] = h1->GetMaximumBin() * threshold; 
	  
      }else{

	newvcharge[channel] = 0.;
	newvamp[channel] = 0.;
	newPShapeTime[channel] = 0.;
	    
      }
    }
    if(selection){
      newev = ev;
      newnch = nch;
      newtree->Fill();
    }
  } // for entries  


  newtree->Write();
  outfile->Close();

  std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;
  
  return 1;

}
