#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "TString.h"
#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TPaveText.h"

#include <boost/algorithm/string/predicate.hpp> //da inserire per usare boost::
#include <boost/lexical_cast.hpp>

using namespace std;

int main( int argc, char* argv[] ) {

  if( argc!= 2 ) {
    cout << "USAGE: ./baseline_voltage [date] (format m'_'gg'_'yyyy)" << endl;
    exit(EXIT_FAILURE);
  }

  string date(argv[1]);
  
  // string nchannels = to_string(n_channels - 1); come trasformare un numero in una stringa

  /*  string channel = " for ch in `seq 0 15`; \n ";
      string tensioni = "do for V in 1400 1450 1500 1550 1600 1650 1700 1750 1800 1850 1900 1950; \n";
 
      string fileName = "Ch_$ch'_'V'_'$V'_'Data'_'" + date + "'_'Ascii.dat";
      string path = date + "/canale'_'$ch/";

      string asciiToTree = " do ./asciiToTree " + fileName + " " + path + " ; \n";

      size_t pos = 0;
      string rootfileName; //aggiunge .root alla stringa (togliendo .dat)
      string prefix;
      if((pos = fileName.find(".")) != std::string::npos) {
      prefix = fileName.substr(0, pos);
      rootfileName = prefix + ".root";
      }
  
      string noiseHisto_all = " ./noiseHisto'_'all " +  rootfileName + " ; \n";

      string baseline_noise = " ./baseline'_'noise " + rootfileName + " plots/" + prefix + "/noise'_'histograms.root" + " ; \n";

      string removeRootFile = " rm " + rootfileName + " \n done \n";

      string endloop = "  done";
  

      string command = channel + tensioni + asciiToTree + noiseHisto_all + baseline_noise + removeRootFile + endloop ; 

      cout << "comando = "<< command << endl;
  
      if( system(Form("%s", command.c_str() )) ){

      cout << "Command executed" << endl;

      }
      else{
      cout << "Command failed" << endl;
      exit(EXIT_FAILURE);
      }
  */
   
  /* string histo, nchannel, voltage, command2;

     ofstream fout;
     fout.open(Form("baseline_voltage_%s.dat", date.c_str()) );
     fout << "# Date " << date << endl;
     fout.close();


     for(int channel = 0; channel < 16; channel++){

     nchannel = to_string(channel);

     fout.open(Form("baseline_voltage_%s.dat", date.c_str()), ios_base::app);

     fout << "# Channel " << channel << endl;

     fout.close();

     for (int V = 1400; V <= 1950 ; V+=50){

      
     voltage = to_string(V);
     histo = "plots/Ch'_'" + nchannel + "'_'V'_'" + voltage + "'_'Data'_'" + date + "'_'Ascii/noise'_'histograms'_'filtered.root";
     command2 = "./voltage'_'file " + histo + " " + nchannel + " " + voltage + " " + date + "; \n";

     cout << "comando = "<< command2 << endl;
      
     if( system(Form("%s", command2.c_str() )) ){

     cout << "Command executed" << endl;

     }
     else{
     cout << "Command failed" << endl;
     exit(EXIT_FAILURE);
     }
       
     }// V loop
     }// channel loop */

  ifstream infile("baseline_voltage.dat");
  int V, npoints = 0, counter = 0;
  double mean_baseline;
  string line, string_channel;

  double* x = new double;
  double* y = new double;

  
  TGraph *f = new TGraph(1, x, y);

  
  TFile* outfile = TFile::Open("baseline_voltage.root","recreate");

  outfile->Close();
  
  while (getline(infile, line)) {
    
    istringstream iss(line);

    if( boost::starts_with(line, "#") ) {  
      line.erase( 0, 10);
      

      if(counter > 0 ){
	TCanvas* c1 = new TCanvas(Form("c1_%s", string_channel.c_str()), Form("c1_%s", string_channel.c_str()), 600, 600);

	c1->cd();

	//	cout << npoints <<" "<< f->GetN() <<endl;
	
	c1->SetGrid();
	f->SetTitle("");
	c1->SetTicks();

	f->SetMarkerStyle(21);

	f->GetXaxis()->SetTitle("Voltage[V]");
	f->GetYaxis()->SetTitle("Baseline - offset");
	
	f->Draw("AP");

	f->SetMarkerStyle(21);
            
        TFile* outfile = TFile::Open("baseline_voltage.root", "update");
	outfile->cd();
	f->Write();
	outfile->Close();
	cout << "Output file saved: " << "baseline_voltage.root" << endl;

	delete c1;
	for(int i = 0; i < f->GetN(); i++){
	  f->RemovePoint(i);
	}
      }	  

      counter++;
      string_channel = line;     

      //  cout << string_channel << endl;
      npoints = 0;
      
    }else{
      
      if (!(iss >> V >> mean_baseline)) { break; } // error

      //  cout << V << " " << mean_baseline << endl;

      f->SetPoint(npoints, V, mean_baseline);

      
      npoints++;
      
    }

  }

  TCanvas* c1 = new TCanvas(Form("c1_%s", string_channel.c_str()), Form("c1_%s", string_channel.c_str()), 600, 600);

  c1->SetGrid();
  f->Draw("AP");
  c1->SetTickx();
  c1->SetTicky();
  f->SetMarkerStyle(21);

  f->GetXaxis()->SetTitle("High Voltage [V]");
  f->GetYaxis()->SetTitle("Baseline - offset [V]");
  
  c1->cd();
  
  f->Draw("AP");

  f->SetMarkerStyle(21);
            
  outfile = TFile::Open("baseline_voltage.root", "update");
  outfile->cd();
  f->Write();
  outfile->Close();
  cout << "Output file saved: " << "baseline_voltage.root" << endl;
      
  delete c1;
  
  delete f;
   
  return EXIT_SUCCESS;

}
