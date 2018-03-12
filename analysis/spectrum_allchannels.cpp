#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "TString.h"

using namespace std;

int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    cout << "USAGE: ./spectrum_allChannels [rootFileName] [n_channels]" << endl;
    exit(EXIT_FAILURE);
  }

  string fileName(argv[1]);
  int n_channels(atoi(argv[2]));

  string nchannels = to_string(n_channels - 1);

  string s= "for i in `seq 0 " + nchannels + "` ; do ./makeSpectrum " + fileName +" "+ "$i" + " ; done";

  cout << "comando = "<< s << endl;
  
  if( system(Form("%s", s.c_str() )) ){

    cout << "Command executed" << endl;

  }
  else{
    cout << "Command failed" << endl;
    exit(EXIT_FAILURE);
  }  

  return EXIT_SUCCESS;

}
