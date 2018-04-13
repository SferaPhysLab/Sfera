#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>




bool isNumber(const std::string& s);                                                           
                                                                                                                                  // funzione che riconosce i numeri
                                                                                                                                  //
int main( int argc, char* argv[] ) {                                                                                              //
                                                                                                                                  //
  if( argc != 2 ) {                                                                                                               // spiega utilizzo del programma
    std::cout << "USAGE: ./measToTree [filename]" << std::endl;                                                                   // --------
    exit(1);                                                                                                                      // --------
  }                                                                                                                               // --------
                                                                                                                                  //
  std::string fileName(argv[1]);                                                                                                  // nome del file .dat
                                                                                                                                  //
  if( boost::starts_with(argv[1], "../data/") ) {                                                                                 // se fileName inizia con../data/ cancella questa parte
    fileName.erase( 0, 8 );                                                                                                       // così fileName è veramente il nome del file .dat
  }                                                                                                                               // --------
                                                                                                                                  //
  std::ifstream fs(Form("../data/%s", fileName.c_str()));                                                                         // fs contiene l'indirizzo al file .dat
  if( !fs.good() ) {                                                                                                              // se non c'è niente all'indirizzo specificato dillo e termina 
    std::cout << "-> No file called '" << fileName << "' found in '../data/'. Exiting." << std::endl;                             // il programma
    exit(1);                                                                                                                      // --------
  }                                                                                                                               // --------
                                                                                                                                  //
  std::cout << "-> Opened measurements-only data file: " << fileName << std::endl;                                                // avverte che si è aperto il file .dat
                                                                                                                                  //
  size_t pos = 0;                                                                                                                 // definisce pos e lo inizializa a 0
  std::string outfileName;                                                                                                        // conterrà il nome del file root creato
  if((pos = fileName.find(".")) != std::string::npos) {                                                                           // assegna a pos la posizione del punto in fileName
    std::string prefix = fileName.substr(0, pos);                                                                                 // salva in prefix tutto fileName tranne il .dat
    outfileName = prefix + ".root";                                                                                               // mette il nome del file .root in  outfileName
  }                                                                                                                               // --------
                                                                                                                                  //
  TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );                                                                // crea il file .root
  TTree* tree = new TTree( "tree", "" );                                                                                          // crea il tree
                                                                                                                                  //
                                                                                                                                  //
  int ev;                                                                                                                         // definisce le variabili e i branch
  int nch;                                                                                                                        // --------
  float base     [128];                                                                                                           // --------
  float vamp     [128];                                                                                                           // --------
  float vcharge  [128];                                                                                                           // --------
  float letime   [128];                                                                                                           // --------
  float tetime   [128];                                                                                                           // --------
                                                                                                                                  // --------
  tree->Branch( "ev"       , &ev      , "ev/I"            );                                                                      // --------
  tree->Branch( "nch"      , &nch     , "nch/I"           );                                                                      // --------
  tree->Branch( "base"     , base     , "base[16]/F"     );                                                                      // --------
  tree->Branch( "vamp"     , vamp     , "vamp[16]/F"     );                                                                      // --------
  tree->Branch( "vcharge"  , vcharge  , "vcharge[16]/F"  );                                                                      // --------
  tree->Branch( "letime"   , letime   , "letime[16]/F"   );                                                                      // --------
  tree->Branch( "tetime"   , tetime   , "tetime[16]/F"   );                                                                      // --------
                                                                                                                                  //
  std::string line;                                                                                                               //
  bool wasReadingEvent = false;                                                                                                   //
  int ch = -1;                                                                                                                    //
                                                                                                                                  //
                                                                                                                                  //
  if( fs.good() ) {                                                                                                               // se in all'indirizzo fs c'è il file .dat
                                                                                                                                  //
    std::cout << "-> Starting parsing file." << std::endl;                                                                        // avverte che lavora sul file
    nch=0;                                                                                                                        // pone a 0 il numero di canali
                                                                                                                                  //
    while( getline(fs,line) ) {                                                                                                   // prende dal file nell'indirizzo fs la prima riga e la mette in line
                                                                                                                                  //
      line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());                                                        // cancella da line tutti i \n
      std::string delimiter = " ";                                                                                                // definisce il delimitatore tra le parole (lo spazio)
      size_t pos = 0;                                                                                                             // indice della posizione
      std::vector<std::string> words;                                                                                             // definisce un vettore di stringhe words
      std::string word;                                                                                                           // definisce una stringa word
      while ((pos = line.find(delimiter)) != std::string::npos) {                                                                 // mette pos = posizione di uno spazio
        word = line.substr(0, pos);                                                                                               // salva in word la prima parola di line
        line.erase(0, pos + delimiter.length());                                                                                  // cancella da line la parola scritta in word
        words.push_back(word);                                                                                                    // mette in words la parola in word
      }                                                                                                                           //
                                                                                                                                  //
      std::vector< std::string > words_cleaned;                                                                                   // un altro vettore di stringhe
      for( unsigned i=0; i<words.size(); ++i ) {                                                                                  // 
        if( isNumber(words[i]) ) words_cleaned.push_back( words[i] );                                                             // se l'elemento i di words è un numero lo salva in words_cleaned
      }                                                                                                                           //
                                                                                                                                  //
      if( words[0]=="===" && words[1]=="Event" && wasReadingEvent ) {                                                             // se la prima parola è "=== Event" e sta leggendo l'evento
                                                                                                                                  //
        if( ev % 100 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;                                           // ogni 100 eventi aggiorna su quale evento sta lavorando
                                                                                                                                  //
        tree->Fill();                                                                                                             // riempi il tree
                                                                                                                                  //
        nch = 0;                                                                                                                  // numero di canali = 0
        ch = -1;                                                                                                                  // canale -1 (nessun canale)
        wasReadingEvent = false;                                                                                                  // non sta leggendo l'evento
                                                                                                                                  //
      } else if( words[0]!="===" && words_cleaned.size()==7 ) {                                                                   // se invece la prima parola non è === e words_cleaned ha 7 elementi
                                                                                                                                  //
        wasReadingEvent = true;                                                                                                   // sta leggendo l'evento
                                                                                                                                  //
        nch += 1;                                                                                                                 // aggiungi un canale al numero totale di canali
                                                                                                                                  //
        ch            = atoi(words_cleaned[0].c_str());                                                                           // assegna i valori giusti alle variabili del tree
        base     [ch] = -atof(words_cleaned[2].c_str());                                                                          // -------
        vamp     [ch] = -atof(words_cleaned[3].c_str());                                                                          // -------
        vcharge  [ch] = -atof(words_cleaned[4].c_str());                                                                          // -------
        letime   [ch] = atof(words_cleaned[5].c_str());                                                                           // -------
        tetime   [ch] = atof(words_cleaned[6].c_str());                                                                           // -------
      }                                                                                                                           //
                                                                                                                                  //
      if( words[0]=="===" && words[1]=="Event" && wasReadingEvent==false ) {                                                      // se non sta leggendo l'evento e la prima parola è "=== Event"
	ev = atoi(words[2].c_str());	                                                                                          // salva l'evento
      }                                                                                                                           //
                                                                                                                                  //
    }                                                                                                                             //
                                                                                                                                  //
  }                                                                                                                               // ha finito di leggere il file .dat
  if( wasReadingEvent )                                                                                                           // mette nel tree gli ultimi valori presi
    {                                                                                                                             // -------
      std::cout << "   ... analyzing event: " << ev << std::endl;                                                                 // -------
      tree->Fill();                                                                                                               // -------
    }                                                                                                                             // -------
                                                                                                                                   //
  fs.close();                                                                                                                     // chiude il file .dat
                                                                                                                                  //
  tree->Write();                                                                                                                  // scrive il tree nel file .root
  outfile->Close();                                                                                                               // chiude il file .root
                                                                                                                                  //
  std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;                                                           // avverte che ha finito
                                                                                                                                  //
  return 0;                                                                                                                       //
                                                                                                                                  //
}                                                                                                                                 //
                                                                                                                                  //
                                                                                                                                  //
                                                                                                                                  //
bool isNumber(const std::string& s) {                                                                                             //
                                                                                                                                  //
  std::string::const_iterator it = s.begin();                                                                                     //
  while (it != s.end() && (std::isdigit(*it) || (*it)==std::string(".") || (*it)==std::string("-")) ) ++it;                       //
  return !s.empty() && it == s.end();                                                                                             //
                                                                                                                                  //
}                                                                                                                                 //
