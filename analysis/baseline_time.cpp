#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TF1.h"
#include "stat.h"
#include "TGraphErrors.h"

#define N_soglia 50
#define N_punti 3622
#define N_time 30 //30 minuti

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./baseline_time [rootFileName] [/path/rootFileName (type noise_histograms_filtered)]" << std::endl;
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
  int nev = 0;
  float pshape[128][1024];
  float offset = 1.;
  float mean_baseline = 0.;  //0 V
  float rms_threshold = 0.005; //5 mV
  int  spike_filter = 0, nspike = 0, npoints = 0;
  float check = 0.;
  bool spike = true;
  bool ev_Ok = true;


  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );

  tree->GetEntry(0);
  ch_max = nch;

  double* x = new double;
  double* y = new double;
  double* ex = new double;
  double* ey = new double;
  double* x_ch = new double;
  double* y_ch = new double;
  double* ex_ch = new double;
  double* ey_ch = new double;
  double x_value = 0., y_value = 0., ex_value = 0., ey_value = 0.;
  std::vector<double> v;
  
  TGraphErrors *f = new TGraphErrors(1, x, y, ex, ey);
  TGraphErrors *gr = new TGraphErrors(16, x_ch, y_ch, ex_ch, ey_ch); 

  //crea cartella dove mettere i plot se non presente

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );


  //crea file di output
  
  TFile* outfile = TFile::Open(Form("%s/baseline_time.root",plotsDir.c_str()),"recreate");

  outfile->Close();

  
  for(int channel = 0; channel < ch_max; channel++){

    if(fabs(pshape[channel][0] - offset) > 0.5){

      std::cout << "Channel "<< channel <<" is empty. Going on..." << std::endl;
      
    }else{

      std::cout << "Channel : "<< channel << std::endl;

      TCanvas* c1 = new TCanvas(Form("c1_%d", channel), Form("c1_%d", channel), 600, 600);

      c1->cd();

      TH1D* h_noise_filtered = (TH1D*)file2->Get(Form("h_noise_filtered_%d", channel));

      mean_baseline = h_noise_filtered->GetMean();

      std::cout << "Media: " << mean_baseline << std::endl;
	  
      gr->SetPoint(channel, channel , mean_baseline - offset );
      gr->SetPointError(channel, ex_value, h_noise_filtered->GetMeanError());

      int nentries = tree->GetEntries();  //numero di righe della tabella (tree) == numero eventi

      nspike = 0;
       npoints = 0;
       v.clear();
       
      for( unsigned iEntry=0; iEntry<nentries; iEntry++ ) {

	tree->GetEntry(iEntry);  //seleziona la riga i-esima (evento i-esimo)

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

	    //   std::cout << "Evento scartato : "<< ev << std::endl;

	    i = N_soglia; //rottura ciclo
	    ev_Ok = false;

	  }else{
	       
	    if( spike && spike_filter > 0){

	      //std::cout << "Evento con spike: "<< ev << std::endl;
	      nspike++;

	      spike = false;
		 
	    }
	    
	 
	  }
	}

	if(ev_Ok){

	  for(int i = 0; i < N_soglia; i++){
	    v.push_back(pshape[channel][i]);  // riempie con baseline a canale fissato
	  }

	    nev++;

	}
	
	if(nev % N_punti == 0){
	  
	  Statistics *a = new Statistics(v);

	  x_value = (double) (npoints +0.5) * N_time;
	  y_value = a->getMean() - mean_baseline;
	  ey_value = a->getMeanError();
	  
	  f->SetPoint(npoints, x_value , y_value);
	  f->SetPointError(npoints, ex_value, ey_value );

	  npoints++;

	  delete a;
	  v.clear();
	}
      } // for entries

      c1->SetGrid();
      f->Draw("AP");

      f->SetMarkerStyle(21);

      f->GetXaxis()->SetTitle("Time[min]");
      f->GetYaxis()->SetTitle("Variation of baseline");
            
      TFile* outfile = TFile::Open(Form("%s/baseline_time.root", plotsDir.c_str()), "update");
      outfile->cd();
      f->Write();
      outfile->Close();
      std::cout << "Output file saved: " << Form("%s/baseline_time.root",plotsDir.c_str()) << std::endl;
      
      delete h_noise_filtered;
      delete c1;

      for(int i = 0 ; i < f->GetN(); i++){
	f->RemovePoint(i);
      }

    } //else channel on 
    
  }//for channel

  delete f;

  TCanvas* c1 = new TCanvas("c1", "c1" , 600, 600);

  c1->cd();
  
  gr->Draw("AP");

  gr->SetMarkerStyle(21);
  
  TFile* outfile2 = TFile::Open(Form("%s/baseline_channel.root", plotsDir.c_str()), "recreate");
  outfile2->cd();
  gr->Write();
  outfile2->Close();
  std::cout << "Output file saved: " << Form("%s/baseline_channel.root",plotsDir.c_str()) << std::endl;
  delete gr;
  
  return 0;

}
