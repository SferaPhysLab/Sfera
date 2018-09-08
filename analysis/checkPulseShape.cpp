#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TF1.h>
#include <TSpectrum.h>

//Na22_And charge min 14 max 18 //2.5 ns
//Na_Time charge min 20 max 100 //0.312 ns

#define CHARGE_MIN 48 //mettere i valori tree_new
#define CHARGE_MAX 60

#define FIT

int main( int argc, char* argv[] ) {

  if( argc!= 4 ) {
    std::cout << "USAGE: ./checkPulseShape [rootFileName] [event] [channel]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  int event  (atoi(argv[2]));
  int channel(atoi(argv[3]));

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check pulse shape of event: " << event << ", channel: " << channel << std::endl;

  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd(); 
  
  int ev;
  int nch;
  float pshape[128][1024];
  float vcharge[128];
  float vamp[128];
  double peak;
  double timepeak;
  double timeShape;
  double par[4];
  
  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "vamp", &vamp);
  
  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );

  
  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

#ifdef FIT

    if(vcharge[channel] < CHARGE_MAX && vcharge[channel] > CHARGE_MIN){

#endif
#ifndef FIT
      if( ev!=event ) continue;
      /*if( channel>=nch ) {
	std::cout << "Event " << ev << " does not have channel " << channel << " (nch=" << nch << ")." << std::endl;
	exit(11);
	}*/ //non ha senso, channel può essere maggiore del numero di canali
#endif
      for( unsigned i=0; i<1024; ++i ) {
	h1->SetBinContent( i+1, pshape[channel][i]);
      }

      std::cout << "Vamp= " << vamp[channel] << " Carica= " << vcharge[channel] << "Evento= " << ev << std::endl;

#ifndef FIT
      break;
#endif
      
#ifdef FIT
      break;
    }
#endif
  } // for entries
  /*
#ifdef FIT

  h1->Rebin(8);

  peak = h1->GetMaximum();
  

    std::cout << "Picco: " << peak << std::endl;

  
    TSpectrum *s = new TSpectrum(1);

    if (s->Search(h1, 2, "goff", 0.1) != 1) {
      std::cerr << "Error: Number of peaks not as expected (1). Exiting" << std::endl;
      exit(11);
    }

    timepeak = *s->GetPositionX();
 
    std::cout << "Tempo Picco: " << timepeak << std::endl;  //[3] + [4]* x + [5]*x*x +[6]*x*x*x +
  
    TF1 *doubleExp = new TF1("doubleExp", Form(" [0]*exp(- (x - %lf) * [1] ) - [2]* exp(- (x - %lf) * [3])", timepeak, timepeak), 0.78 * timepeak ,  5. * timepeak );  //0.75 * timepeak, 5*timepeak con Rebin(8)

    doubleExp->SetParLimits(0, 0.7* peak, 10. * peak);     //parametri 0, 1 ok per esponenziale decrescente   da 0.9* timepeak a 5*timepeak                         
    doubleExp->SetParLimits(1, 0.001, 0.1);

    doubleExp->SetParLimits(2, 0.05, 5);  //0.7 con Rebin(8)

    doubleExp->SetParLimits(3, 0.05, 10);


 
    doubleExp->SetParameter(0, 5 * peak);                 
    doubleExp->SetParameter(1, 0.01);
    doubleExp->SetParameter(2, 5);
    doubleExp->SetParameter(3, 0.5); 

    h1->Fit("doubleExp", "RL");
#endif
  */
  
    h1->Draw();

    /*
#ifdef FIT
    doubleExp->GetParameters(par);

    timepeak = doubleExp->GetMaximumX();

    timeShape = timepeak*0.75;

    std::cout << "Tempo PShape: " << timeShape << std::endl;
   
#endif
    */
    size_t pos = 0;
    std::string prefix;
    if((pos = fileName.find(".")) != std::string::npos) {
      prefix = fileName.substr(0, pos);
    }

    std::string plotsDir(Form("plots/%s", prefix.c_str()));
    system( Form("mkdir -p %s", plotsDir.c_str()) );

    h1->SaveAs(Form("%s/pulseShape_ev%d_ch%d_provafit.root",plotsDir.c_str(),event,channel));
    // c1->SaveAs(Form("%s/pulseShape_ev%d_ch%d.pdf",plotsDir.c_str(),event,channel));

    return 0;

  }
