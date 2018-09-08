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
#include "TGraphErrors.h"

#include <boost/algorithm/string/predicate.hpp> //da inserire per usare boost::
#include <boost/lexical_cast.hpp>

#define START 2.
#define EPSILON 0.01
#define STEP 0.001

using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 6) {
    cerr << "USAGE: ./sorgenti [Bario_rootfileName] [Cesio_rootfileName] [Cobalto_rootfileName] [Sodio_rootfileName] [canale]" <<  endl;
    exit(1);
  }
  
  string fileName(argv[1]);
  string fileName2(argv[2]);
  string fileName3(argv[3]);
  string fileName4(argv[4]);
  int ch(atoi(argv[5]));

  ofstream fout;
  fout.open("sorgenti.dat");

  //f << "#Linearità in energia" <<  endl;
  
  fout.close();

  if( !system(Form("./bario %s", fileName.c_str() )) ){

    cout << "Command executed (bario)" << endl;

  }
  else{
    cout << "Command failed (bario)" << endl;
    exit(EXIT_FAILURE);
  }
  
  //------------------------------------------//

    if( !system(Form("./cesio %s", fileName2.c_str() )) ){

    cout << "Command executed (cesio)" << endl;

  }
  else{
    cout << "Command failed (cesio)" << endl;
    exit(EXIT_FAILURE);
    }

  //------------------------------------------//

  if( !system(Form("./cobalto %s", fileName3.c_str() )) ){

    cout << "Command executed (cobalto)" << endl;

  }
  else{
    cout << "Command failed (cobalto)" << endl;
    exit(EXIT_FAILURE);
  }
  //------------------------------------------//

  if( !system(Form("./sodio %s", fileName4.c_str() )) ){

    cout << "Command executed (sodio)" << endl;

  }
  else{
    cout << "Command failed (sodio)" << endl;
    exit(EXIT_FAILURE);
  }
  //------------------------------------------//


  ifstream infile("sorgenti.dat");
  int npoints = 0;
  double Energy, Charge, ChargeError;
  string line;

  double* x = new double;
  double* y = new double;
  double* ex = new double;
  double* ey = new double;
  double C[16]={0.,0.,0.,0.,0.,0.69013062,0.66726970,0.,0.,0.67670111,0.,0.65935270,0.,0.,0.,0.};
  double NDF = 7., sigma = 0.;
  bool flag = false;
  
  string plotsDir = "plots/Linearity" ;
  
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  
  TFile* outfile = TFile::Open(Form("%s/sorgenti_Ch%d.root", plotsDir.c_str(), ch),"recreate");

  TGraphErrors *f = new TGraphErrors(1, x, y, ex, ey);

  
  while (getline(infile, line)) {
    
    istringstream iss(line);

    /*    if( boost::starts_with(line, "#") ) {
	  line.erase( 0, 10);

	  }else{*/
      
    if (!(iss >> Energy >> Charge >> ChargeError)) { break; } // error

    cout << Energy << " " << Charge << " "<< ChargeError << endl;

    f->SetPoint(npoints, Energy, Charge * C[ch]);
    f->SetPointError(npoints, 0., 0.);

    npoints++;
      
    //}

  }

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);

  c1->SetGrid();
  c1->SetTickx();
  c1->SetTicky();
  f->SetMarkerStyle(20);

  f->GetXaxis()->SetTitle("Nominal Energy [keV]");
  f->GetYaxis()->SetTitle("Measured Energy [keV]");
  
  c1->cd();

    

  
  for(sigma = START; flag == false ; sigma+= STEP){
 
    for(int i = 0; i < 9; i++){
      f->SetPointError(i, 0., sigma );
    }

    std::cout<<"\n\n" << sigma<< "\n\n" <<std::endl;
  
    f->Draw("AP");

  
    TF1 *linear = new TF1("linear", "[0] * x + [1]", 0., 1500.);  //Carica = [0] * Energia + [1] -> Energia = (Carica - [1]) / [0]

    f->Fit("linear", "R");

    if (fabs(linear->GetChisquare() - NDF) < EPSILON){
      flag = true;
    }
  
  }
  
            
  outfile->cd();
  f->Write();
  outfile->Close();
  cout << "Output file saved: " << Form("%s/sorgenti_Ch%d.root", plotsDir.c_str(), ch) << endl;
      
  delete c1;
  
  delete f;

  
  return 0;
}
