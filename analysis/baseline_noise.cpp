#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TF1.h"

#define N_soglia 50

//NOTA: per essere sicuri che noise_filtered racchiuda tutto, definire h_noise_filtered tra h_baseline->GetMinimun() e GetMaximum()

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./baseline_noise [rootFileName] [/path/rootFileName (type noise_histograms)]" << std::endl;
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
  int nch, ch_max;
  int nbin = 0;
  float pshape[128][1024];
  float offset = 1.;
  float baseline = 0.;
  float mean_baseline = 0.;  //0 V
  float rms_threshold = 0.005; //5 mV
  float mean_new = 0., stdDev_new = 0.;
  int  spike_filter = 0, nspike = 0;
  float check = 0.;
  bool spike = true;
  bool ev_Ok = true;


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );

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
  
  TFile* outfile = TFile::Open(Form("%s/baseline.root",plotsDir.c_str()),"recreate");

  outfile->Close();

  
  TFile* outfile2 = TFile::Open(Form("%s/noise_histograms_filtered.root",plotsDir.c_str()),"recreate");

  outfile2->Close();
  
  for(int channel = 0; channel < ch_max; channel++){

    if(fabs(pshape[channel][0] - offset) > 0.5){

      std::cout << "Channel "<< channel <<" is empty. Going on..." << std::endl;
      
    }else{

      std::cout << "Channel : "<< channel << std::endl;

      TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

      c1->cd();

      TH1D* h_noise = (TH1D*)file2->Get(Form("h_noise_%d", channel));

      mean_baseline = h_noise->GetMean();

      TH1D* h_noise_filtered = new TH1D(Form("h_noise_filtered_%d", channel), Form("h_noise_filtered_%d", channel), 100, mean_baseline - 2* rms_threshold, mean_baseline + 2 * rms_threshold );

      std::cout << "Media: " << mean_baseline << std::endl;
  

      int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

       TH1D* h_baseline = new TH1D(Form("h_baseline_%d", channel),Form("h_baseline_%d", channel), nentries, 0., (float)nentries );

       nspike = 0;
       
      for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

	tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

	/*if( channel>=nch ) {
	  std::cout << "Event " << ev << " does not have channel " << channel << " (nch=" << nch << ")." << std::endl;
	  exit(11);
	  }*/

	spike = true;
	ev_Ok = true;
	
	for(int i = 0; i < N_soglia; i++){

	  spike_filter = 0;
	     
	  for(int j = 0; j < 5; j++){
	    check += pshape[channel][i+j];
	    if(fabs(pshape[channel][i+j] - mean_baseline) > rms_threshold){
	      spike_filter++;
	    }
	  }

	  check/=5.;
	  
	  if(fabs(check - mean_baseline) > rms_threshold && spike_filter > 4){ //evento da scartare (piccola scintillazione prima della soglia)

	    std::cout << "Evento scartato : "<< ev << std::endl;

	    i = N_soglia; //rottura ciclo
	    ev_Ok = false;

	  }else{
	       
	    if( spike && spike_filter > 0){

	      //std::cout << "Evento con spike: "<< ev << std::endl;
	      nspike++;

	      spike = false;
		 
	    }
	    
	    baseline += pshape[channel][i];  // media baseline a canale fissato (fornito dall'utente)
	    h_noise_filtered->Fill(pshape[channel][i]);
	  }
	} //ciclo su i 

	//	ev_Ok = true;  //aggiunta per mostrare l'efficacia dell'algoritmo
	
	if(ev_Ok){
	    baseline/= N_soglia;

	    nbin++;
	
	    h_baseline->SetBinContent(nbin, baseline);

	}
	    baseline = 0.;
	
      } // for entries

      std:: cout << "Numero eventi salvati: " << nbin << std:: endl;
      std:: cout << "Numero Spike: " << nspike << std:: endl;
      
      h_baseline->SetBins(nbin, 0., (float)nbin);
      
      nbin = 0;
      
      TFile* outfile = TFile::Open(Form("%s/baseline.root",plotsDir.c_str()),"update");
      outfile->cd();
      h_baseline->Write();
      outfile->Close();
      std::cout << "Output file saved: " << Form("%s/baseline.root",plotsDir.c_str()) << std::endl;


      mean_new = 0.;
      stdDev_new = 0.;

      mean_new = h_noise_filtered->GetMean();
      stdDev_new = h_noise_filtered->GetStdDev();

      TF1* gaus = new TF1("f1", "gaus", mean_new - 4* stdDev_new, mean_new + 4* stdDev_new);

      h_noise_filtered->Fit(gaus, "R");
      
       TFile* outfile2 = TFile::Open(Form("%s/noise_histograms_filtered.root",plotsDir.c_str()),"update");
      outfile2->cd();
      h_noise_filtered->Write();
      outfile2->Close();
      std::cout << "Output file saved: " << Form("%s/noise_histograms_filtered.root",plotsDir.c_str()) << std::endl;

      
      delete h_baseline;
      delete h_noise;
      delete h_noise_filtered;
      delete gaus;
      delete c1;

    } //else channel on 
    
  }//for channel
  
  return 0;

}
