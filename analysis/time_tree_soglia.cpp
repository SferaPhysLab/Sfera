#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <TF1.h>
#include <TSpectrum.h>

//Na22_And charge min 14 max 18, 2.5 ns
//Na_Time charge min 20 max 100, 0.312 ns
//Na22c charge min 30 max 60, 0.625 ns / solo picco min 48 max 60

#define CHARGE_MIN 48 //mettere i valori tree_new
#define CHARGE_MAX 60
//#define THRESHOLD 0.008 //(5sigma(rumore) del canale più rumoroso (ch 14))
#define CONV 0.625 //1 sample = 0.312 ns

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./time_tree [rootFileName_new] [threshold] (type Na22 in AND) . Crea un nuovo tree contenente solo gli eventi interessanti (carica intorno al picco del Na22) e aggiunge le informazioni temporali: tempo associato al segnale stimato da un algoritmo a soglia (>0.05V)" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  #ifdef THRESHOLD
  double threshold = THRESHOLD;
  #endif
  #ifndef THRESHOLD
  double threshold =(double)atof(argv[2]);
  #endif
  
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
  TFile* outfile = TFile::Open(Form("%s_time.root", prefix.c_str() ),"recreate");
  TTree *newtree =  new TTree( "tree", "" );

  newtree->Branch( "ev"       , &newev      , "newev/I"            );
  newtree->Branch( "nch"      , &newnch     , "newnch/I"           );
  newtree->Branch( "vcharge"   , newvcharge   , "newvcharge[newnch]/F");
  newtree->Branch( "vamp"   , newvamp   , "newvamp[newnch]/D");
  newtree->Branch( "PShapeTime"   , newPShapeTime   , "newPShapeTime[newnch]/D");
  
  tree->GetEntry(0);
  ch_max = nch;

  int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

       
  for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

    tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

    selection = false;
	 
    for( channel = 0; channel < ch_max; channel++){

      if(vcharge[channel] < CHARGE_MAX && vcharge[channel] > CHARGE_MIN){ 

	selection = true;

	newvcharge[channel] = vcharge[channel];
	newvamp[channel] = (double)vamp[channel];

	 
	for( unsigned i=0; i<1024; i++ ) {
	  if( pshape[channel][i] >= threshold ){
	    
	    if(pshape[channel][i+1] >= threshold && pshape[channel][i+2] >= threshold){
	    timepeak = (double)i * CONV;
	    break;
	    }else{ //probabile spike nel tratto pre-segnale
	      //   std::cout << "Spike pre-segnale sopra soglia, evento= " << ev << " canale= " << channel << std::endl;
	    }
	  }
	}
   
	newPShapeTime[channel] = timepeak; 
	  
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
