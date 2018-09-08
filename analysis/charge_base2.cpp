#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "TGraph.h"
#include "TMultiGraph.h"




int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./charge [rootFileName1(canale 2)] [rootFileName(new)1]. Questo programma confronta la differenza della carica calcolata dal programma con l'integrale della pulse shape, per verificare se il programma sottrae o no la baseline." << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  std::string fileName2(argv[2]);


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");
  
  TFile* file2 = TFile::Open(fileName2.c_str());
  TTree* newtree = (TTree*)file2->Get("tree");

  
  int ev;
  int nch;
  float pshape[128][1024];
  float vcharge[128];

  tree->SetBranchAddress( "ev" , &ev     );
  tree->SetBranchAddress( "nch"   , &nch    );
  tree->SetBranchAddress( "pshape", &pshape );
  tree->SetBranchAddress( "vcharge", &vcharge );
  
  int newev;
  int newnch;
  float newpshape[128][1024];
  float newvcharge[128];
  

  newtree->SetBranchAddress( "ev" , &newev     );
  newtree->SetBranchAddress( "nch"   , &newnch    );
  newtree->SetBranchAddress( "pshape", &newpshape );
  newtree->SetBranchAddress( "vcharge"   , &newvcharge);

  
  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );
  
  int nentries = tree->GetEntries();
  double* x = new double;
  double* y = new double;
  double x_value = 0., y_value = 0.;
  double* X = new double;
  double* Y = new double;
  double X_value = 0., Y_value = 0.;
  double baseline = 1.01563;
  
  
  
  TGraph *f1 = new TGraph(20000, x, y);
  TGraph *f2 = new TGraph(20000, X, Y);
  
  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }

  std::string plotsDir(Form("plots/%s", prefix.c_str()));
  system( Form("mkdir -p %s", plotsDir.c_str()) );


  TFile* outfile = TFile::Open(Form("%s/charge_base2.root",plotsDir.c_str() ),"recreate");
 


  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

  c1->cd();

  TMultiGraph* mg = new TMultiGraph("mg", "");
  
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) { // iEntry 0 -> Evento 1

    tree->GetEntry(iEntry);
    newtree->GetEntry(iEntry);

    x_value = vcharge[2]  ; //cariche calcolate da wavecatcher
    y_value = newvcharge[2] ; //cariche calcolate con getsum sottraendo la baseline
	
    for( unsigned i=0; i<1024; ++i ) {
      h1->SetBinContent( i+1, newpshape[2][i] + baseline );
    }

    X_value = x_value;
    Y_value = h1->GetSum(); //cariche calcoltate con getsum non sottraendo la baseline ma positiva (__/^\__ )
 
    
    f1->SetPoint(iEntry, x_value, y_value);
    f2->SetPoint(iEntry, X_value, Y_value);

  } // for entries
  
  f1->SetMarkerColor(kBlue);
  f1->SetMarkerStyle(21);
  f2->SetMarkerColor(kRed);
  f2->SetMarkerStyle(21);
      
  mg->Add(f1);
  mg->Add(f2);
  mg->Draw("AP");
  
  outfile->cd();
  mg->Write();
  
  outfile->Close();
  std::cout << "Output file saved: " << Form("%s/charge_base2.root",plotsDir.c_str()) << std::endl;
      
  delete c1;     

  delete f1;
  delete h1;
  delete f2;

  
  return 0;

}
