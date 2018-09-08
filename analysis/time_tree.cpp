#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <TF1.h>
#include <TSpectrum.h>

#define CHARGE_MIN 14 //mettere i valori tree_new
#define CHARGE_MAX 18

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./time_tree [rootFileName_new] (type Na22 in AND) [threshold] . Crea un nuovo tree contenente solo gli eventi interessanti (carica intorno al picco del Na22) e aggiunge le informazioni temporali: tempo associato al segnale e tempo di scintillazione, stimati a partire da un fit della pulseshape." << std::endl;
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
  double newScintTime[16];
  double newPShapeTime[16];
  bool selection;
  double peak, timepeak;

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
  newtree->Branch( "ScintTime"   , newScintTime   , "newScintTime[newnch]/D");
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

	TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );
	 
	for( unsigned i=0; i<1024; ++i ) {
	  h1->SetBinContent( i+1, pshape[channel][i] );
	}

	h1->Rebin(8);

        peak = h1->GetMaximum();

	TSpectrum *s = new TSpectrum(1);

	if (s->Search(h1, 2, "goff", 0.1) != 1) {
	  std::cerr << "Error: Number of peaks not as expected (1). Exiting" << std::endl;
	  exit(11);
	}

        timepeak = *s->GetPositionX();

	TF1 *doubleExp = new TF1("doubleExp", Form(" [0]*exp(- (x - %lf) * [1] ) - [2]* exp(- (x - %lf) * [3])", timepeak, timepeak), 0.78 * timepeak ,  5. * timepeak );  //0.75 * timepeak, 5*timepeak con Rebin(8)
  
	doubleExp->SetParLimits(0, 0.7* peak, 10. * peak);     //parametri 0, 1 ok per esponenziale decrescente   da 0.9* timepeak a 5*timepeak                
	doubleExp->SetParLimits(1, 0.001, 0.1);
	doubleExp->SetParLimits(2, 0.01, 5);  
	doubleExp->SetParLimits(3, 0.05, 10);
 
	doubleExp->SetParameter(0, 5 * peak);                 
	doubleExp->SetParameter(1, 0.01);
	doubleExp->SetParameter(2, 5);
	doubleExp->SetParameter(3, 0.5);


	int FitStatus = h1->Fit("doubleExp", "RL");

	if(FitStatus == 0){

	  timepeak = doubleExp->GetMaximumX();
   
	  newPShapeTime[channel] = threshold * timepeak * 2.5; //tempo in nanosecondi ( 1 sample = 2.5 ns)  
	  newScintTime[channel] = 1 / ( doubleExp->GetParameter(1)) * 2.5;
	  //	  std::cerr << "Event " << ev << " channel " << channel << std::endl;
	  
   
	}else   {
	  std::cerr << "Problems while fitting event " << ev << " channel " << channel << std::endl;
	  // exit(EXIT_FAILURE);
	}

	delete h1;
	delete doubleExp;
	
      }else{
	
	newvcharge[channel] = 0.;
	newvamp[channel] = 0.;
	newScintTime[channel] = 0.;
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
