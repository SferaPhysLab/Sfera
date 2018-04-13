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
  std::string comando = Form("./calibrazione 0 1350 1600 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 1 1500 1700 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 2 1650 1850 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 3 1450 1650 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 4 1450 1600 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 5 1650 1850 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 6 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 7 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 8 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 9 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 10 1550 1850 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 11 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 12 1550 1750 50 28 3");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 13 1650 1900 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 14 1650 1950 50 4 4");
  system(Form("%s", comando.c_str() ));
  comando = Form("./calibrazione 15 1500 1850 50 4 4");
  system(Form("%s", comando.c_str() ));
  
  return 0;
}
