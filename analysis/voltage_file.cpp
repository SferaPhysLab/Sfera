#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"

using namespace std;

int main( int argc, char* argv[] ){

  if( argc!= 5 ) {
    cout << "USAGE: ./voltage_file path/rootfilename(type noise_histograms_filtered) channel voltage date " << endl;
    exit(EXIT_FAILURE);
  }

  string fileName(argv[1]);
  int channel(atoi(argv[2]));
  int V(atoi(argv[3]));
  string date(argv[4]);

  double mean_baseline, offset = 1.;

  ofstream f;
  f.open(Form("baseline_voltage_%s.dat", date.c_str()), ios_base::app);

  TFile* file = TFile::Open(fileName.c_str());

  if (file->IsZombie()){

    cout << "No file called "<< fileName << "..." << endl;
    exit(1);
	   
  }else{

	
    TH1D* h_noise_filtered = (TH1D*)file->Get(Form("h_noise_filtered_%d", channel));

    mean_baseline = h_noise_filtered->GetMean();

    f << V << "\t" << mean_baseline - offset << endl;

    delete h_noise_filtered;
       
  }
  delete file; 
   
  return 1;
}
