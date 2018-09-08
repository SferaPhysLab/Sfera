#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TF1.h"
#include "TLegend.h"
#include "TGaxis.h"

#define MIN 20
#define MAX 1000
#define MAX2 800
#define NBIN 200
#define YMAX 2500

#define GRAPH


int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "USAGE: ./bario [rootfileName]" << std::endl;
    exit(1);
  }
  
  std::string fileName(argv[1]);
	      
  TFile *file = TFile::Open(fileName.c_str());
  TTree *tree = (TTree *) file->Get("tree");
  TH1D *hcharge = new TH1D("hcharge", "", NBIN, MIN, MAX);

  tree->Project("hcharge", "vcharge");

  std::cerr << "-> Opened file " << fileName.c_str() << std::endl;
  std::cerr << "-> Will find the photopeak and fit." << std::endl;
  
  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();

 
  TSpectrum *s = new TSpectrum(5);

  double peak1, peak2, peak3, peak4, minpos;

  if (s->Search(hcharge, 2, "goff", 0.1) == 5) {
    
    peak1 = s->GetPositionX()[0];
    peak2 = s->GetPositionX()[4];
    peak3 = s->GetPositionX()[3];
    peak4 = s->GetPositionX()[1];
    minpos = s->GetPositionX()[2];

    if(s->GetPositionX()[3] < s->GetPositionX()[4]){
      peak3 = s->GetPositionX()[4];
      peak2 = s->GetPositionX()[3];
    }

  }else if(s->Search(hcharge, 2, "goff", 0.1) == 4){
    
    peak1 = s->GetPositionX()[0];
    peak2 = s->GetPositionX()[3];
    peak3 = s->GetPositionX()[2];
    peak4 = s->GetPositionX()[1];
  
  }else{
    std::cerr << "Error: Number of peaks not as expected (5 or 4) s= "<< s->Search(hcharge, 2, "goff", 0.1) <<" . Exiting" << std::endl;
    exit(11);
  }

 

  hcharge->SetAxisRange(MIN, MAX2, "X");

  hcharge->SetLineColor(1);
   

  TF1 *all = new TF1("all", "[0] * exp(-x * [1]) + [2] * exp(-(x - [3]) * (x - [3]) / (2 * [4] * [4])) + [5] * exp(-(x - [6]) * (x - [6]) / (2 * [7] * [7])) + [8] * exp(-(x - [9]) * (x - [9]) / (2 * [10] * [10])) + [11] * exp(-(x - [12]) * (x - [12]) / (2 * [13] * [13]) ) + [14] * exp(-(x - [15]) * (x - [15]) / (2 * [16] * [16]) )", 0.7 * peak1 , 1.3 * peak4); 

  all->SetParLimits(0, .1 * peak1, 10 * peak1);                         
  all->SetParLimits(1, 0.001, 0.1);                         
  all->SetParLimits(2, .7 * YMAX, 1.3 * YMAX); 
  all->SetParLimits(3, .9 * peak1, 1.1 * peak1); 
  all->SetParLimits(4, 5. , 100.);  
  all->SetParLimits(5, .7 * YMAX / 5. , 1.3 * YMAX / 5);
  all->SetParLimits(6, .9 * peak2, 1.1 * peak2);
  all->SetParLimits(7, 50., 100.);
  all->SetParLimits(8, .5 * YMAX / 5. , 1.2 * YMAX / 5.); 
  all->SetParLimits(9, .95 * 428 , 1.05 * 428);
  all->SetParLimits(10, 10., 100.);
  all->SetParLimits(11, .8 * YMAX / 5. , 1.3 * YMAX / 5);
  all->SetParLimits(12, .95 * 470, 1.05 * 470);
  all->SetParLimits(13, 10., 100.);
  all->SetParLimits(14, .65 * YMAX *0.8 , 1.3 * YMAX * 0.8);
  all->SetParLimits(15, .95 * peak4, 1.1 * peak4);
  all->SetParLimits(16, 15., 100.);
 
  
  all->SetParameter(0, peak1);                 
  all->SetParameter(1, 0.01);
  all->SetParameter(2, YMAX);
  all->SetParameter(3, peak1);
  all->SetParameter(4, 20.);
  all->SetParameter(5, YMAX * 0.2);
  all->SetParameter(6, peak2);
  all->SetParameter(7, 50.);
  all->SetParameter(8, YMAX * 0.2);
  all->SetParameter(9, 428);
  all->SetParameter(10, 20.);
  all->SetParameter(11, YMAX * 0.2);
  all->SetParameter(12, 470);
  all->SetParameter(13, 30.);
  all->SetParameter(14, YMAX * 0.8);
  all->SetParameter(15, peak4);
  all->SetParameter(16, 20.);
  

  hcharge->Fit("all", "R");

  std::cout << "\nNDF: " << all->GetNDF() << std::endl;
  std::cout << "Chi^2: " << all->GetChisquare() << std::endl;

#ifdef GRAPH
  hcharge->Draw();

  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  
  c1->SaveAs(Form("%s/bario.root", plotsDir.c_str()));
#endif

#ifdef GRAPH

  TF1 *de = new TF1("de", "[0] * exp(-x * [1])", 0.7 * peak1, 1.3 * peak4);
  TF1 *gaus1 = new TF1("gaus1", "gaus", 0.7 * peak1, 1.3 * peak4);
  TF1 *gaus2 = new TF1("gaus2", "gaus", 0.7 * peak1, 1.3 * peak4);
  TF1 *gaus3 = new TF1("gaus3", "gaus", 0.7 * peak1, 1.3 * peak4);
  TF1 *gaus4 = new TF1("gaus4", "gaus", 0.7 * peak1, 1.3 * peak4);
  TF1 *gaus5 = new TF1("gaus5", "gaus", 0.7 * peak1, 1.3 * peak4);
  
  de->SetParameters(all->GetParameter(0), all->GetParameter(1));
  de->SetLineStyle(2);
  de->SetLineColor(40);
  gaus1->SetParameters(all->GetParameter(2), all->GetParameter(3), all->GetParameter(4));
  gaus1->SetLineStyle(2);
  gaus1->SetLineColor(41);
  gaus2->SetParameters(all->GetParameter(5), all->GetParameter(6), all->GetParameter(7));
  gaus2->SetLineStyle(2);
  gaus2->SetLineColor(42);
  gaus3->SetParameters(all->GetParameter(8), all->GetParameter(9), all->GetParameter(10));
  gaus3->SetLineStyle(2);
  gaus3->SetLineColor(43);
  gaus4->SetParameters(all->GetParameter(11), all->GetParameter(12), all->GetParameter(13));
  gaus4->SetLineStyle(2);
  gaus4->SetLineColor(44);
  gaus5->SetParameters(all->GetParameter(14), all->GetParameter(15), all->GetParameter(16));
  gaus5->SetLineStyle(2);
  gaus5->SetLineColor(45);


  hcharge->Draw();
  de->Draw("same");
  gaus1->Draw("same");
  gaus2->Draw("same");
  gaus3->Draw("same");
  gaus4->Draw("same");
  gaus5->Draw("same");

  TGaxis *axis2 = new TGaxis(MIN, -200, MAX2 , -200, MIN , MAX2 / 1.449, 510, "" ); //NB costanti di calibrazione canale 5 : Q = E * 1.449

  axis2->Draw("same");
  
  c1->SaveAs("totalbario.root");

  
#endif

  std::ofstream f;
  f.open("sorgenti.dat", std::ios_base::app);

  f << 81.0 << "\t" << all->GetParameter(3) << "\t" << all->GetParError(3) << "\t" << all->GetParameter(4) << "\t" << all->GetParError(4) << std::endl;
  f << 160.6 << "\t" << all->GetParameter(6) << "\t" << all->GetParError(6) << "\t" << all->GetParameter(7) << "\t" << all->GetParError(7) << std::endl;
  f << 302.9 << "\t" << all->GetParameter(12) << "\t" << all->GetParError(12) << "\t" << all->GetParameter(13) << "\t" << all->GetParError(13) << std::endl;
  f << 356.0 << "\t" << all->GetParameter(15) << "\t" << all->GetParError(15) << "\t" << all->GetParameter(16) << "\t" << all->GetParError(16) << std::endl;
  
  f.close();

  
  return 0;
}
