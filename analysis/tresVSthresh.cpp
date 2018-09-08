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
    cout << "USAGE: ./tresVSthresh [rootFileName_new]. Questo programma esegue in sequenza time_tree e time_resolution, dando in ingresso diverse soglie per trovare la soglia con migliore risoluzione (per ciascuna coppia di canali)." << endl;
    exit(EXIT_FAILURE);
  }

  string fileName(argv[1]);

  size_t pos = 0;
  string prefix;
  if((pos = fileName.find(".")) != string::npos) {
    prefix = fileName.substr(0, pos);
  }

  ofstream fout;
  fout.open("tresVSthresh.dat");
  
  fout.close();

  for(float threshold = 0.015; threshold < 0.0155; threshold+= 0.001){

    string th = to_string(threshold);
    
      if( system(Form("./time_tree_soglia %s %s ; \n ", fileName.c_str(), th.c_str() )) ){

      cout << "Command executed" << endl;

      }
      else{
      cout << "Command failed" << endl;
      exit(EXIT_FAILURE);
      }

  fout.open("tresVSthresh.dat", std::ios_base::app);

  fout << threshold << " ";

  fout.close();

  cout << Form(" ./time_resolution %s_time.root ; ",  prefix.c_str() ) << endl;
  
     if( system(Form(" ./time_resolution %s_time.root ; ",  prefix.c_str() )) ){

      cout << "Command executed" << endl;

      }
      else{
      cout << "Command failed" << endl;
      exit(EXIT_FAILURE);
      }
  

  }
   
  return EXIT_SUCCESS;

}
