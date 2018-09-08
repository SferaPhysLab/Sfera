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

#define MIN 50
#define MAX 5000
#define NBIN 200

#define GRAPH
//#define DEBUG
//#define LEGEND


int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "USAGE: ./sodio [rootfileName]" << std::endl;
    exit(1);
  }
  
  std::string fileName(argv[1]);
	      
  TFile *file = TFile::Open(fileName.c_str());
  TTree *tree = (TTree *) file->Get("tree");
  TH1D *hcharge = new TH1D("hcharge", "Spettro ^{22}Na", NBIN, MIN, MAX);

  tree->Project("hcharge", "vcharge");

  std::cerr << "-> Opened file " << fileName.c_str() << std::endl;
  std::cerr << "-> Will fit the sodium spectrum." << std::endl;
  
  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();


  hcharge->GetXaxis()->SetRange((int) (hcharge->GetMaximumBin() * 1.3), NBIN);
    
  TSpectrum *s = new TSpectrum(2);

  if (s->Search(hcharge, 2, "goff", 0.1) != 2) {
    std::cerr << "Error: Number of peaks not as expected (2). Exiting" << std::endl;
    exit(11);
  }


  double A1, sigma1, A2, sigma2;
  
  hcharge->SetAxisRange(.5 * *s->GetPositionX(), *s->GetPositionX(), "X");
  A1 = *s->GetPositionY() - hcharge->GetMinimum();
  sigma1 = *s->GetPositionX() - (MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMinimumBin());

  hcharge->SetAxisRange(*s->GetPositionX(), *(s->GetPositionX() + 1), "X");
  A2 = *(s->GetPositionY() + 1) - hcharge->GetMinimum();
  sigma2 = *(s->GetPositionX() + 1) - (MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMinimumBin());
  
  hcharge->SetAxisRange(MIN, 1.3 * *(s->GetPositionX() + 1));

 TF1 *all = new TF1("all", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3])) + [4] * exp(-(x - [5]) * (x - [5]) / (2 * [6] * [6])) + [7] / (exp((x - [8]) * [9]) + 1) + [10] * exp(-(x - [11]) * (x - [11]) / (2 * [12] * [12])) + [13] / (exp((x - [14]) * [15]) + 1)", (MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin()), 1.3 * *(s->GetPositionX() + 1)); 

  all->SetParLimits(0, .5 * hcharge->GetMaximum(), 10. * hcharge->GetMaximum());          // Finestra intorno a scala data dall'esponenziale
  all->SetParLimits(1, 0.01, 0.99);                                                       // Peso di ciascun esponenziale maggiore di 0.01
  all->SetParLimits(2, .1 / *s->GetPositionX(), 10 / *s->GetPositionX());                 // Finestra di prova intorno a scala energetica data dal primo picco
  all->SetParLimits(3, .1 / *s->GetPositionX(), 10 / *s->GetPositionX());                 // Finestra di prova intorno a scala energetica data dal primo picco
  all->SetParLimits(4, .5 * A1, 1.5 * A1);                                                // Normalizzazione prima gaussiana stimata +o- 50%
  all->SetParLimits(5, .9 * *s->GetPositionX(), 1.1 * *s->GetPositionX());                // Media prima gaussiana stimata +o- 10%
  all->SetParLimits(6, .3 * sigma1, 1.7 * sigma1);                                        // Sigma prima gaussiana stimata +o- 50%
  all->SetParLimits(7, 0, .3 * hcharge->GetMaximum());                                    // Compton piccolo rispetto ad esponenziale
  all->SetParLimits(8, .8 * *s->GetPositionX(), *s->GetPositionX());                      // Scala energetica data dal primo picco
  all->SetParLimits(9, .05, 2);  // Copiato da Gruppo 2017
  all->SetParLimits(10, .5 * A2, 1.5 * A2);                                               // Normalizzazione seconda gaussiana stimata +o- 50%
  all->SetParLimits(11, .8 * *(s->GetPositionX() + 1), 1. * *(s->GetPositionX() + 1));   // Media seconda gaussiana stimata +o- 10%                                                     
  all->SetParLimits(12, .5 * sigma2, 1.5 * sigma2);                                       // Sigma prima gaussiana stimata +o- 50%
  all->SetParLimits(13, 0, .3 * hcharge->GetMaximum());                                   // Compton piccolo rispetto ad esponenziale
  all->SetParLimits(14, .8* *(s->GetPositionX() + 1),  *(s->GetPositionX() + 1));         // Scala energetica data dal secondo picco
  all->SetParLimits(15, .05, 2); // Copiato da Gruppo 2017
  
  all->SetParameter(0, hcharge->GetMaximum());         // Normalizzazione doppio esponenziale da scala
  all->SetParameter(1, .5);                            // Componenti doppio esponenziale lenta e veloce pesano uguali
  all->SetParameter(2, 1 / *s->GetPositionX());        // Scala energetica doppio esponenziale come compton
  all->SetParameter(3, 1 / *s->GetPositionX());        // Scala energetica doppio esponenziale come compton
  all->SetParameter(4, A1);                            // Normalizzazione prima gaussiana stimata
  all->SetParameter(5, *s->GetPositionX());            // Media prima gaussiana stimata
  all->SetParameter(6, sigma1);                        // Sigma prima gaussiana stimata
  all->SetParameter(7, .1 * hcharge->GetMaximum());    // Compton piccolo rispetto a doppio esponenziale
  all->SetParameter(8, .9 * *s->GetPositionX());       // Scala energetica compton
  all->SetParameter(9, 1);    // Da Gruppo 2017
  all->SetParameter(10, A2);                           // Normalizzazione seconda gaussiana stimata
  all->SetParameter(11, .9* *(s->GetPositionX() + 1));           // Media seconda gaussiana stimata
  all->SetParameter(12, sigma2);                       // Sigma seconda gaussiana stimata
  all->SetParameter(13, .1 * hcharge->GetMaximum());   // Compton piccolo rispetto a doppio esponenziale
  all->SetParameter(14, *(s->GetPositionX() + 1));           // Scala energetica data dal secondo picco
  all->SetParameter(15, 1);   // Da Gruppo 2017
  
#ifdef DEBUG
  for(unsigned i = 0 ; i < 16 ; i++) std::cout << i << "\t" << all->GetParameter(i) << std::endl;
#endif

  
  hcharge->Fit("all", "RL");

  std::cout << "\nNDF: " << all->GetNDF() << std::endl;
  std::cout << "Chi^2: " << all->GetChisquare() << std::endl;
  
#ifdef GRAPH
  hcharge->Draw();

  

  c1->SaveAs("sodio.pdf");
#endif


#ifdef GRAPH

  TF1 *de = new TF1("de", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3]))", MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(), 1.3 * *(s->GetPositionX() + 1));
  TF1 *gauss1 = new TF1("gauss1", "gaus", MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(), 1.3 * *(s->GetPositionX() + 1));
  TF1 *FD1 = new TF1("FD1", "[0] / (exp((x - [1]) * [2]) + 1)", MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(), 1.3 * *(s->GetPositionX() + 1));
  TF1 *gauss2 = new TF1("gauss2", "gaus", MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(), 1.3 * *(s->GetPositionX() + 1));
  TF1 *FD2 = new TF1("FD2", "[0] / (exp((x - [1]) * [2]) + 1)", MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(), 1.3 * *(s->GetPositionX() + 1));
  
  de->SetParameters(all->GetParameter(0), all->GetParameter(1), all->GetParameter(2), all->GetParameter(3));
  de->SetLineStyle(2);
  de->SetLineColor(40);
  gauss1->SetParameters(all->GetParameter(4), all->GetParameter(5), all->GetParameter(6));
  gauss1->SetLineStyle(2);
  gauss1->SetLineColor(41);
  FD1->SetParameters(all->GetParameter(7), all->GetParameter(8), all->GetParameter(9));
  FD1->SetLineStyle(2);
  FD1->SetLineColor(30);
  gauss2->SetParameters(all->GetParameter(10), all->GetParameter(11), all->GetParameter(12));
  gauss2->SetLineStyle(2);
  gauss2->SetLineColor(46);
  FD2->SetParameters(all->GetParameter(13), all->GetParameter(14), all->GetParameter(15));
  FD2->SetLineStyle(2);
  FD2->SetLineColor(33); 

  
  de->Draw("same");
  gauss1->Draw("same");
  FD1->Draw("same");
  gauss2->Draw("same");
  FD2->Draw("same");

#ifdef LEGEND
  TLegend *legend = new TLegend(.1, .7, .48, .9);
  legend->AddEntry("de", "A #[]{p e^{-#omega_{1} x} + (1 - p) e^{-#omega_{2} x}}");
  legend->AddEntry("gauss1", "B_{1} e^{- #frac{(x - #mu_{1})^{2}}{#sigma_{1}^{2}}}");
  legend->AddEntry("FD1", "#frac{C_{1}}{e^{#beta_{1}(x - m_{1})} + 1}");
  legend->AddEntry("gauss2", "B_{2} e^{- #frac{(x - #mu_{2})^{2}}{#sigma_{2}^{2}}}");
  legend->AddEntry("FD2", "#frac{C_{2}}{e^{#beta_{2}(x - m_{2})} + 1}");
  legend->Draw();
#endif

  TGaxis *axis = new TGaxis(MIN, -100, 1.3 * *(s->GetPositionX() + 1) , -100, MIN ,1.3 * *(s->GetPositionX() + 1) / 1.449, 510, "" ); //NB costanti di calibrazione canale 5 : Q = E * 1.449
  axis->Draw("same");
  c1->SaveAs("totalsodio.root");
  //c1->SaveAs("spettrosodio.root");
#endif

  std::ofstream f;
  f.open("sorgenti.dat", std::ios_base::app);

  f << 511.0 << "\t" << all->GetParameter(5) << "\t" << all->GetParError(5) << "\t" << all->GetParameter(6) << "\t" << all->GetParError(6) << std::endl;
  f << 1274.5 << "\t" << all->GetParameter(11) << "\t" << all->GetParError(11) << "\t" << all->GetParameter(12) << "\t" << all->GetParError(12) << std::endl;
  
  f.close();
  
  return 0;
}
