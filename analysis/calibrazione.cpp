#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPaveText.h"


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <tgmath.h>



#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>


int main( int argc, char* argv[]) {
  if( argc != 7 ) { 
    std::cout << "USAGE: ./calibrazione [canale] [vMin] [vMax] [passo] [giorno] [mese]" << std::endl;
    exit(1);     
  }
  int ch = std::stoi(argv[1]);
  int step = std::stoi(argv[4]);
  int vMin = std::stoi(argv[2]);
  int vMax = std::stoi(argv[3]);
  int day = std::stoi(argv[5]);
  int month = std::stoi(argv[6]);

  double erA = 0;
  double erB = 0;
  double A = 0;
  double B = 0;
  double Volt = 0;
  double erVolt = 0;
  
  int nbin = 100;
  int cambio = 1;
  double Max = 10000;
  double Min = 100;
  double peak;
  double peakOld=0;

  int i=0;
  int n=0;
  n = (vMax-vMin)/step+1;
  TH1F *histo;

  TFile *f;
  TCanvas *c;
  TTree* tree;
  TDirectory *where;
  TF1* fitg;
  TPaveText *pv;

  double x[n];
  double y[n];
  
  std::string plotsDir(Form("plots/calibrazioni/canale_%d/spettri", ch));
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  
  std::string comando = Form(" for (( i = %d ; i < %d ; i += %d )) ; do ./measToTree Ch_%d_V_${i}_Measurements_Only_%d_%d_2018.dat; done", vMin, vMin+step*n, step, ch, month, day);
  system(Form("%s", comando.c_str() ));
  
  for(int v=vMin; v<=vMax; v+=step){
    Max=10000;
    cambio=1;
    while(cambio==1){
      cambio=0;
      where = gDirectory;
      histo = new TH1F("charge", "charge", nbin, Min, Max);
      f = new TFile(Form("Ch_%d_V_%d_Measurements_Only_%d_%d_2018.root", ch, v, month, day));
      tree = (TTree*)f->Get("tree");
      where->cd();
      tree->Project("charge",Form("vcharge[%d]", ch));
      histo->GetXaxis()->SetRange(1+(peakOld/Max)*nbin,nbin);
      peak = histo->GetMaximumBin();
      peak=Min+((Max-Min)/nbin)*(peak);
      if(peak<(3./4.)*Max){
          Max=(7./8.)*Max;
          cambio=1;
	  delete histo;
      }
    }
    histo->GetXaxis()->SetRange(1,nbin);
    peakOld=peak;
    c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
    fitg = new TF1("fitg", "gaus", (peak+(12./100.)*peak), (peak-(8./100.)*peak));
    histo->Fit(fitg, "R");
    histo->SetTitle(Form("SPETTRO CANALE %d VOLTAGGIO %d", ch, v));
    histo->GetXaxis()->SetTitle("carica (pC)");
    histo->GetYaxis()->SetTitle("nEv");
    histo->GetXaxis()->CenterTitle();
    histo->GetYaxis()->CenterTitle();
    histo->Draw();
    pv = new TPaveText(.1,.9,.3,.75,"brNDC");
    pv->AddText("FIT GAUSSIANO");
    pv->AddLine(.0,.67,1.,.67);
    pv->AddText(Form("mean: %f +/- %f", fitg->GetParameter(1),fitg->GetParError(1)));
    pv->AddText(Form("stdev: %f +/- %f", fitg->GetParameter(2),fitg->GetParError(2)));
    pv->Draw();
    c->SaveAs(Form("plots/calibrazioni/canale_%d/spettri/Spettro_Ch_%d_V_%d.pdf", ch, ch, v));
    x[i]=v;
    y[i]=log((fitg->GetParameter(1)));
    i++;
    delete c;
    f->Close();
  }
  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  TGraph *gr = new TGraph(n, x, y);
  c->SetGrid();
  gr->SetMarkerStyle(8);
  gr->SetMarkerSize(0.8);
  gr->SetMarkerColor(2);
  gr->SetTitle(Form("CALIBRAZIONE CANALE %d",ch));
  gr->GetXaxis()->SetTitle("Voltaggio [Volt]");
  gr->GetYaxis()->SetTitle("Log(carica picco)");
  gr->GetXaxis()->CenterTitle();
  gr->GetYaxis()->CenterTitle();
  TF1 *f1 = new TF1("f1","[0]*x+[1]",0,3000);
  f1->SetLineColor(4);
  gr->Fit(f1,"R");
  A = f1->GetParameter(0);
  B = f1->GetParameter(1);
  erA = f1->GetParError(0);
  erB = f1->GetParError(1);
  Volt = (6.9-B)/A;
  erVolt = (fabs(erB/(6.9-B))+fabs(erA/A))*Volt;
  TPaveText pvl(.1,.9,.35,.65,"brNDC");
  pvl.AddText("y = a#upointx + b");
  ((TText*)pvl.GetListOfLines()->Last())->SetTextColor(kBlue);
  pvl.AddLine(.0,.75,1.,.75);
  pvl.AddText(Form("a: %f +/- %f", A, erA));
  pvl.AddText(Form("b: %f +/- %f", B, erB));
  pvl.AddText(Form("ln(Q)=6.9 - >V = %f +/- %f", Volt, erVolt));
  gr->Draw("AP");
  pvl.Draw();
  c->SaveAs(Form("plots/calibrazioni/canale_%d/calibrazione_Ch_%d.pdf",ch, ch));
 
  return 0;
}
