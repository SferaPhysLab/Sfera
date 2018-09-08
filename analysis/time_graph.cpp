#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream> //necessario per ifstream ofstream

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TGraphErrors.h>

int main( int argc, char* argv[] ) {

  if( argc!= 2 ) {
    std::cout << "USAGE: ./time_graph [rootFileName_new_time]. Grafica media e dev std in funzione della coppia di canali opposti." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);

  std::cout << "-> Will check the time resolution."<< std::endl;

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  
  double* x1 = new double;
  double* y1 = new double;
  double* ex1 = new double;
  double* ey1 = new double;
  double* x2 = new double;
  double* y2 = new double;
  double* ex2 = new double;
  double* ey2 = new double;
  
  TGraphErrors *f_mean = new TGraphErrors(8, x1, y1, ex1, ey1);  
  TGraphErrors *f_sigma = new TGraphErrors(8, x2, y2, ex2, ey2);
  
  for(int i = 0; i < 8; i++){
    
    TFile* infile = TFile::Open(Form("%s/time_histograms_%d.root",plotsDir.c_str(), i));   

    TH1D* h_time = (TH1D*)infile->Get(Form("h_time_%d", i));

    f_mean->SetPoint(i, i,  h_time->GetMean());
    f_mean->SetPointError(i, 0.,  h_time->GetMeanError());
    f_sigma->SetPoint(i, i,  h_time->GetStdDev());
    f_sigma->SetPointError(i, 0., h_time->GetStdDevError());

    delete h_time;

    infile->Close();
  }


  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

  c1->cd();
  c1->SetGrid();
  f_mean->Draw("AP");
  f_mean->SetName("f_mean");

  f_mean->SetMarkerStyle(20);

  f_mean->GetXaxis()->SetTitle("Channels");
  f_mean->GetYaxis()->SetTitle("#mu [ns]");
            
  TFile* outfile1 = TFile::Open(Form("%s/time_mean.root", plotsDir.c_str()), "recreate");
  outfile1->cd();
  f_mean->Write();
  outfile1->Close();
  std::cout << "Output file saved: " << Form("%s/time_mean.root",plotsDir.c_str()) << std::endl;

  delete c1;

  
  TCanvas* c2 = new TCanvas("c1", "c1", 600, 600);

  c2->cd();
  c2->SetGrid();
  f_sigma->Draw("AP");
  f_sigma->SetName("f_sigma");

  f_sigma->SetMarkerStyle(20);

  f_sigma->GetXaxis()->SetTitle("Channels");
  f_sigma->GetYaxis()->SetTitle("#sigma [ns]");
            
  TFile* outfile2 = TFile::Open(Form("%s/time_sigma.root", plotsDir.c_str()), "recreate");
  outfile2->cd();
  f_sigma->Write();
  outfile2->Close();
  std::cout << "Output file saved: " << Form("%s/time_sigma.root",plotsDir.c_str()) << std::endl;

  delete c2;


  return 1;

}
