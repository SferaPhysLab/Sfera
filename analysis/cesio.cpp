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
#define NBIN 500

#define GRAPH


int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "USAGE: ./completo [rootfileName]" << std::endl;
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


  TSpectrum *s = new TSpectrum(1);

  if (s->Search(hcharge, 2, "goff", 0.1) != 1) {
    std::cerr << "Error: Number of peaks not as expected (1). Exiting" << std::endl;
    exit(11);
  }

  double maxpos = *s->GetPositionX();
  double minpos;
  
  hcharge->SetAxisRange(.5 * maxpos, maxpos, "X");
  minpos = MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMinimumBin();

  TF1 *gauss = new TF1("gauss", "gaus", 1.1 * minpos, maxpos + .9 * (maxpos - minpos));
  
  hcharge->SetAxisRange(1.1 * minpos, maxpos + 0.9 * (maxpos - minpos), "X");

  hcharge->Fit(gauss, "R");

#ifdef GRAPH
  hcharge->Draw();

  c1->SaveAs("cesiogauss.pdf");
#endif

  double leftmax, leftmaxpos, comptonpos;

  hcharge->SetAxisRange(MIN, minpos, "X");
  hcharge->SetLineColor(1);
  leftmax = hcharge->GetMaximum();
  leftmaxpos = MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin();  

  hcharge->SetAxisRange(MIN, 1.3 * maxpos, "X");

  comptonpos = gauss->GetParameter(1) * (1 - 1 / (1 + 2 * 662. / 511));

  TF1 *all = new TF1("all", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3])) + [4] * exp(-(x - [5]) * (x - [5]) / (2 * [6] * [6])) + [7] / (exp((x - [8]) * [9]) + 1) + [10] / ([12] * exp((x - [13]) * [11]) + 1)", leftmaxpos, 1.3 * maxpos); 

  all->SetParLimits(0, .5 * leftmax, 10 * leftmax);                                 // Finestra di tre ordini di grandezza intorno al valore atteso
  all->SetParLimits(1, 0.01, 0.99);                                                 // Peso di ciascun esponenziale maggiore di 0.01
  all->SetParLimits(2, .1 / comptonpos, 1. / comptonpos);                           // Finestra di prova intorno al valore atteso (da Gruppo 2017)
  all->SetParLimits(3, 1. / comptonpos, 10. / comptonpos);                           // Finestra di prova intorno al valore atteso (da Gruppo 2017)
  all->SetParLimits(4, .9 * gauss->GetParameter(0), 1.1 * gauss->GetParameter(0));  // Risultati del primo fit possono cambiare del 10%
  all->SetParLimits(5, .9 * gauss->GetParameter(1), 1.1 * gauss->GetParameter(1));  // Risultati del primo fit possono cambiare del 10%
  all->SetParLimits(6, .8 * gauss->GetParameter(2), 1.2 * gauss->GetParameter(2));  // Risultati del primo fit possono cambiare del 20%, meno non funziona
  all->SetParLimits(7, 0, .5 * leftmax);                                                 // Non piu grande del massimo compton
  all->SetParLimits(8, .9 * comptonpos, 1.1 * comptonpos);                          // Scala energetica del compton +o- 10%
  //  all->SetParLimits(9, .1 * gauss->GetParameter(2), 10 * gauss->GetParameter(2));   // Finestra di tre ordini di grandezza intorno al valore atteso
  all->SetParLimits(9, .005, 2);  // Copiato da Gruppo 2017
  all->SetParLimits(10, 0, .25 * leftmax);                              // Finestra piu o meno ragionevole
  all->SetParLimits(11, .01, 1);                                                     // Da Gruppo 2017
  all->SetParLimits(12, .01, 20);                                                   // Da Gruppo 2017
  all->SetParLimits(13, .5 * gauss->GetParameter(1), 1.1 * gauss->GetParameter(1)); // Finestra asimmetrica intorno a valore atteso
  
  all->SetParameter(0, leftmax);                 // Normalizzazione doppio esponenziale da scala
  all->SetParameter(1, .5);                      // Componenti doppio esponenziale lenta e veloce pesano uguali
  all->SetParameter(2, .5 / comptonpos);          // Scala energetica doppio esponenziale come compton
  all->SetParameter(3, 5. / comptonpos);          // Scala energetica doppio esponenziale come compton
  all->SetParameter(4, gauss->GetParameter(0));  // Normalizzazione da fit gaussiano precedente
  all->SetParameter(5, gauss->GetParameter(1));  // Media da fit gaussiano precedente
  all->SetParameter(6, gauss->GetParameter(2));  // Sigma da fit gaussiano precedente
  all->SetParameter(7, leftmax / 3);             // Normalizzazione compton da scala minore di doppio esponenziale
  all->SetParameter(8, comptonpos);              // Scala energetica compton
  all->SetParameter(9, .5 * gauss->GetParameter(2));    // Da Gruppo 2017
  all->SetParameter(10, .05 * leftmax);          // Normalizzazione compton modulato da scala minore di compton
  all->SetParameter(11, .6);                     // Da Gruppo 2017  (secondo loro basta che parte)
  all->SetParameter(12, .95);                    // Da Gruppo 2017
  all->SetParameter(13, gauss->GetParameter(1)); // Parametro aggiunto per disaccoppiare FD modulata da gaussiana. Il valore atteso e` media gaussiana
		    

  hcharge->Fit("all", "R");

  std::cout << "\nNDF: " << all->GetNDF() << std::endl;
  std::cout << "Chi^2: " << all->GetChisquare() << std::endl;

#ifdef GRAPH
  hcharge->Draw();

  c1->SaveAs("cesio.pdf");
#endif

#ifdef GRAPH

  TF1 *de = new TF1("de", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3]))", leftmaxpos, 1.3 * maxpos);
  TF1 *FD = new TF1("FD", "[0] / (exp((x - [1]) * [2]) + 1)", leftmaxpos, 1.3 * maxpos);
  TF1 *FDM = new TF1("FDM", "[0] / ([1] * exp((x - [2]) * [3]) + 1)", leftmaxpos, 1.3 * maxpos);
  TF1 *prova = new TF1("prova", "gaus", leftmaxpos, 1.3 * maxpos);
  
  de->SetParameters(all->GetParameter(0), all->GetParameter(1), all->GetParameter(2), all->GetParameter(3));
  de->SetLineStyle(2);
  de->SetLineColor(40);
  prova->SetParameters(all->GetParameter(4), all->GetParameter(5), all->GetParameter(6));
  prova->SetLineStyle(2);
  prova->SetLineColor(41);
  FD->SetParameters(all->GetParameter(7), all->GetParameter(8), all->GetParameter(9));
  FD->SetLineStyle(2);
  FD->SetLineColor(30);
  FDM->SetParameters(all->GetParameter(10), all->GetParameter(12), all->GetParameter(5), all->GetParameter(11));
  FDM->SetLineStyle(2);
  FDM->SetLineColor(46);
  
  de->Draw("same");
  prova->Draw("same");
  FD->Draw("same");
  FDM->Draw("same");

#ifdef LEGEND
  TLegend *legend = new TLegend(.1, .7, .48, .9);
  legend->AddEntry(hcharge, "Istogramma carica integrata");
  legend->AddEntry("de", "f(x) = A #(){p e^{-#omega_{1} x} + (1 - p) e^{-#omega_{2} x}}");
  legend->Draw();
#endif

  TGaxis *axis = new TGaxis(MIN, -100, 1.3 * maxpos , -100, MIN , 1.3 * maxpos / 1.449, 510, "" ); //NB costanti di calibrazione canale 5 : Q = E * 1.449
  axis->Draw("same");
  c1->SaveAs("totalcesio.root");

  
#endif

  std::ofstream f;
  f.open("sorgenti.dat", std::ios_base::app);

  f << 661.7 << "\t" << all->GetParameter(5) << "\t" << all->GetParError(5) << "\t" << all->GetParameter(6) <<"\t" << all->GetParError(6) << std::endl;
  
  f.close();
  
  return 0;
}
