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
#include "TGaxis.h"

#define MIN 50
#define MAX 2500
#define NBIN 100

#define GRAPH


int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "USAGE: ./cobalto [rootfileName]" << std::endl;
    exit(1);
  }
  
  std::string fileName(argv[1]);
	      
  TFile *file = TFile::Open(fileName.c_str());
  TTree *tree = (TTree *) file->Get("tree");
  TH1D *hcharge = new TH1D("hcharge", "hcharge", NBIN, MIN, MAX);

  tree->Project("hcharge", "vcharge");

  std::cerr << "-> Opened file " << fileName.c_str() << std::endl;
  std::cerr << "-> Will find the photopeak and fit." << std::endl;
  
  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();

  TSpectrum *s = new TSpectrum(3);
  //std::cout<<s->Search(hcharge, 2, "", 0.1)<<std::endl;
  if (s->Search(hcharge, 2, "", 0.1) != 3) {
    std::cerr << "Error: Number of peaks not as expected (1). Exiting" << std::endl;
    exit(11);
  }
  
  double maxpos0 = s->GetPositionX()[0];
  double maxpos1 = s->GetPositionX()[1];
  double maxpos2 = s->GetPositionX()[2];
  double comptonpos1;
  double comptonpos2;
  double leftmax;
 
  leftmax = hcharge->GetMaximum();
  comptonpos1 = maxpos1   * (1 - 1 / (1 + 2 * 1170. / 511));
  comptonpos2 = maxpos2   * (1 - 1 / (1 + 2 * 1330. / 511));

  TF1 *all = new TF1("all", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3])) + [4] * exp(-(x - [5]) * (x - [5]) / (2 * [6] * [6]))+[7] * exp(-(x - [8]) * (x - [8]) / (2 * [9] * [9])) + [10] / (exp((x - [11]) * [12]) + 1) + [13] / (exp((x - [14]) * [15]) + 1)", maxpos0+100., maxpos2+300.); 

  all->SetParLimits(0, .5 * leftmax, 10 * leftmax);                                 // Finestra di tre ordini di grandezza intorno al valore atteso
  all->SetParLimits(1, 0.001, 0.99);                                                 // Peso di ciascun esponenziale maggiore di 0.01
  all->SetParLimits(2, .01 / comptonpos1, 6 / comptonpos1);                           // Finestra di prova intorno al valore atteso (da Gruppo 2017)
  all->SetParLimits(3, 4 / comptonpos1, 10 / comptonpos1);                           // Finestra di prova intorno al valore atteso (da Gruppo 2017)
  all->SetParLimits(4, .1*leftmax, .5*leftmax);  // Risultati del primo fit possono cambiare del 10%
  all->SetParLimits(5, .9 * maxpos1, 1.1 * maxpos1);  // Risultati del primo fit possono cambiare del 10%
  all->SetParLimits(6, 20., 150.);  // Risultati del primo fit possono cambiare del 10%
  all->SetParLimits(7, .1*leftmax, .8*leftmax);                                                 // Non piu grande del massimo compton
  all->SetParLimits(8, .9 * maxpos2, 1.1 * maxpos2);                          // Scala energetica del compton +o- 10%
  all->SetParLimits(9, 20., 150.);
  all->SetParLimits(10, .1*leftmax, .3 * leftmax);                                                 // Non piu grande del massimo compton
  all->SetParLimits(11, .9 * comptonpos1, 1.1 * comptonpos1);                          // Scala energetica del compton +o- 10%
  all->SetParLimits(12, .01, 2);  // Copiato da Gruppo 2017
  all->SetParLimits(13, 0, .25 * leftmax);                              // Finestra piu o meno ragionevole
  all->SetParLimits(14, .9 * comptonpos2, 1 * comptonpos2);                                                   // Da Gruppo 2017
  all->SetParLimits(15, .01, 2); // Finestra asimmetrica intorno a valore atteso
  //all->SetParLimits(16, 0, .25 * leftmax);                              // Finestra piu o meno ragionevole
  //all->SetParLimits(17, 0, 2400);                                                   // Da Gruppo 2017
  //all->SetParLimits(18, .01, 2); // Finestra asimmetrica intorno a valore atteso
  
  all->SetParameter(0, leftmax);                 // Normalizzazione doppio esponenziale da scala
  all->SetParameter(1, .5);                      // Componenti doppio esponenziale lenta e veloce pesano uguali
  all->SetParameter(2, .06 / comptonpos1);          // Scala energetica doppio esponenziale come compton
  all->SetParameter(3, 4 / comptonpos1);          // Scala energetica doppio esponenziale come compton
  all->SetParameter(4, .3*leftmax);  // Normalizzazione da fit gaussiano precedente
  all->SetParameter(5, maxpos1);  // Media da fit gaussiano precedente
  all->SetParameter(6, 50.);  // Sigma da fit gaussiano precedente
  all->SetParameter(7, .2*leftmax);             // Normalizzazione compton da scala minore di doppio esponenziale
  all->SetParameter(8, maxpos2);              // Scala energetica compton
  all->SetParameter(9, 50.);    // Da Gruppo 2017
  all->SetParameter(10, leftmax / 5.);             // Normalizzazione compton da scala minore di doppio esponenziale
  all->SetParameter(11, comptonpos1);              // Scala energetica compton
  all->SetParameter(12, .1);    // Da Gruppo 2017
  all->SetParameter(13, .05 * leftmax);          // Normalizzazione compton modulato da scala minore di compton
  all->SetParameter(14, comptonpos2);                    // Da Gruppo 2017
  all->SetParameter(15, .1); // Parametro aggiunto per disaccoppiare FD modulata da gaussiana. Il valore atteso e` media gaussiana
  //all->SetParameter(16, .05 * leftmax);          // Normalizzazione compton modulato da scala minore di compton
  //all->SetParameter(17, comptonpos2);                    // Da Gruppo 2017
  //all->SetParameter(18, .1);

  hcharge->Fit("all", "R");
  
#ifdef GRAPH
  hcharge->Draw();

  c1->SaveAs("fitall1.pdf");
#endif

#ifdef GRAPH
  TF1 *de = new TF1("de", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3]))", 0, maxpos2+300);
  TF1 *FD = new TF1("FD", "[0] / (exp((x - [1]) * [2]) + 1)", 0, maxpos2+300);
  TF1 *FDM = new TF1("FDM", "[0] / ( exp((x - [1]) * [2]) + 1)", 0, maxpos2+300);
  TF1 *FDF = new TF1("FDF", "[0] / ( exp((x - [1]) * [2]) + 1)", 0, maxpos2+300);
  TF1 *g1 = new TF1("g1", "gaus", 0, maxpos2+300);
  TF1 *g2 = new TF1("g2", "gaus", 0, maxpos2+300);
  
  de->SetParameters(all->GetParameter(0), all->GetParameter(1), all->GetParameter(2), all->GetParameter(3));
  g1->SetParameters(all->GetParameter(4), all->GetParameter(5), all->GetParameter(6));
  g2->SetParameters(all->GetParameter(7), all->GetParameter(8), all->GetParameter(9));
  FD->SetParameters(all->GetParameter(10), all->GetParameter(11), all->GetParameter(12));
  FDM->SetParameters(all->GetParameter(13), all->GetParameter(14),  all->GetParameter(15));
  //FDF->SetParameters(all->GetParameter(16), all->GetParameter(17),  all->GetParameter(18));
  
  de->SetLineColor(1);
  g1->SetLineColor(1);
  g2->SetLineColor(1);
  FD->SetLineColor(1);
  FDM->SetLineColor(1);
  de->SetLineWidth(1);
  g1->SetLineWidth(1);
  g2->SetLineWidth(1);
  FD->SetLineWidth(1);
  FDM->SetLineWidth(1);
  de->SetLineStyle(2);
  g1->SetLineStyle(2);
  g2->SetLineStyle(2);
  FD->SetLineStyle(2);
  FDM->SetLineStyle(2);
  //FDF->SetLineColor(7);
	    
  de->Draw("same");
  g1->Draw("same");
  g2->Draw("same");
  FD->Draw("same");
  FDM->Draw("same");
  //FDF->Draw("same");

  TGaxis *axis = new TGaxis(MIN, -100, MAX , -100, MIN , (double)MAX / 1.449, 510, "" ); //NB costanti di calibrazione canale 5 : Q = E * 1.449
  axis->Draw("same");
  c1->SaveAs("totalcobalto.root");
  #endif

  std::cout<<all->GetChisquare()<<std::endl;
  std::cout<<all->GetNDF()<<std::endl;

  std::ofstream f;
  f.open("sorgenti.dat", std::ios_base::app);

  f << 1173.2 << "\t" << all->GetParameter(5) << "\t" << all->GetParError(5) << "\t" << all->GetParameter(6) << "\t" << all->GetParError(6) << std::endl;
  f << 1332.5 << "\t" << all->GetParameter(8) << "\t" << all->GetParError(8) << "\t" << all->GetParameter(9) << "\t" << all->GetParError(9) << std::endl;
  
  f.close();
  
  
  return 0;
}
