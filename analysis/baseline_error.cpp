#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"

#define N_soglia 50

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./baseline_error [rootFileName] [/path/rootFileName (type noise_histograms_filtered)]. Calcola l'errore dQ/Q vs Q che si fa sottraendo la media dei primi 50 punti anziché la media dell'istogramma della baseline (ovvero parte dell'errore che fa wavecatcher)." << std::endl;
    exit(1);
  } 

  std::string fileName(argv[1]);
  std::string fileName2(argv[2]);

  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  TFile* file2 = TFile::Open(fileName2.c_str());

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;
  std::cout << "-> Will check noise of all non-empty channels " << std::endl;

 
  int ev;
  int nch, ch_max, channel;
  float pshape[128][1024];
  float vcharge[128];
  float newpshape[128][1024];
  float newvcharge[128], baseline_charge[128];
  float mean_baseline[128], baseline[128]; 


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );  

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  double* x = new double;
  double* y = new double;
  double x_value = 0., y_value = 0.;
  
  TGraph *f = new TGraph(1, x, y);  
  
  //crea file di output
  TFile* outfile = TFile::Open(Form("%s/baseline_error.root",plotsDir.c_str()),"recreate");

  outfile->Close();
  
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

  for( channel = 0; channel < ch_max; channel++){


    TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

    c1->cd();
       
    for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

      tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)
	
      //if( ev % 1000 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;	

      baseline[channel] = 0.;
      
      for(int i = 0; i < N_soglia; i++){
	baseline[channel] += pshape[channel][i];
      }
	  
      baseline[channel]/=N_soglia;
      baseline_charge[channel] = (baseline[channel] - mean_baseline[channel]) * 1024.;
	  
      for(int i = 0; i < 1024; i++){
	  
	newpshape[channel][i] = baseline[channel] - pshape[channel][i];  // media baseline a canale fissato (fornito dall'utente)
	h1->SetBinContent( i+1, newpshape[channel][i] );
      }
      newvcharge[channel] = h1->GetSum();

      x_value = vcharge[channel];
      y_value =  baseline_charge[channel] / newvcharge[channel];

      f->SetPoint(iEntry, x_value , y_value);
	   
    } //for entries

    c1->SetGrid();
    f->Draw("AP");
    f->SetName(Form("f_%d", channel));
    f->SetLineColor(kWhite);

    f->SetMarkerStyle(21);

    f->GetXaxis()->SetTitle("Q[pC]");
    f->GetYaxis()->SetTitle("#frac{#delta Q}{Q}");

    f->GetXaxis()->SetRangeUser(10.,5000.);
    f->GetYaxis()->SetRangeUser(-0.1,0.1);
            
    TFile* outfile = TFile::Open(Form("%s/baseline_error.root", plotsDir.c_str()), "update");
    outfile->cd();
    f->Write();
    outfile->Close();
    std::cout << "Output file saved: " << Form("%s/baseline_error.root",plotsDir.c_str()) << std::endl;
      
    delete c1;

    for(int i = 0 ; i < f->GetN(); i++){
      f->RemovePoint(i);
    }
      
	
  } // for channels  
 
  
  return 0;

}
