#include "TFile.h"
#include "TTree.h"
#include "TH1.h"


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>




bool isNumber(const std::string& s);


int main( int argc, char* argv[]) {
    
    if( argc != 4 ) {
        
        std::cout << "USAGE: ./measCleanToTree [filename] [soglia bassa] [soglia alta]" << std::endl;
        exit(1);
        
    }
    
    std::string fileName(argv[1]);
    double sogliab(std::stod(argv[2]));
    double sogliaa(std::stod(argv[3]));

    if( boost::starts_with(argv[1], "../data/") ) {
        fileName.erase( 0, 8 );
    }
    
    std::ifstream fs(Form("../data/%s", fileName.c_str()));
    if( !fs.good() ) {
        std::cout << "-> No file called '" << fileName << "' found in '../data/'. Exiting." << std::endl;
        exit(1);
    }
    
    std::cout << "-> Opened measurements-only data file: " << fileName << std::endl;
    
    size_t pos = 0;
    std::string outfileName;
    if((pos = fileName.find(".")) != std::string::npos) {
        std::string prefix = fileName.substr(0, pos);
        outfileName = prefix + ".root";
    }
    
    TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
    TTree* tree = new TTree( "tree", "" );
    float ratecount[128]={0};
    int conteggiRateCount[128]={0};
    
    int ev;
    int nch;
    int chs        [128];               //CANALE CHE SCINTILLA
    float base     [128];
    float vamp     [128];
    float vcharge  [128];
    float letime   [128];
    float tetime   [128];
    float lmttime  [128];
    
    tree->Branch( "ev"       , &ev      , "ev/I"            );
    tree->Branch( "nch"      , &nch     , "nch/I"           );
    tree->Branch( "chs"      , &chs     , "chs[nch]/I"      );
    tree->Branch( "base"     , base     , "base[nch]/F"     );
    tree->Branch( "vamp"     , vamp     , "vamp[nch]/F"     );
    tree->Branch( "vcharge"  , vcharge  , "vcharge[nch]/F"  );
    tree->Branch( "letime"   , letime   , "letime[nch]/F"   );
    tree->Branch( "tetime"   , tetime   , "tetime[nch]/F"   );
    tree->Branch( "lmttime"  , lmttime  , "lmttime[nch]/F"  );
    
    //tree->Branch( "ratecount", ratecount, "ratecount[nch]/F");
    
    
    std::string line;
    bool wasReadingEvent = false;
    int indArrayTree=0;    // VARIABILE CHE HO AGGIUNTO IO CHE SOSTITUISCE ch IN QUANTO IL PROGRAMMA USAVA ch COME INDICE DI base[], vamp[], vcharge[], letime[] e tetime[]. QUESTO VA BENE SE NON BUTTO EVENTI PERCHÈ IN QUEL CASO ch VA DA 0 A nch SENZA SALTARE NUMERI, IN QUESTO CASO PERÒ NON CONSIDERO TUTTI I CANALI E IL FATTO CHE INIZIALIZZO SOLO base[canale giusto] FA FALLIRE IL TREE.
    int ch = -1;
    int j=0;
    
    
    if( fs.good() ) {
        
        std::cout << "-> Starting parsing file." << std::endl;
        nch=0;
        
        while( getline(fs,line) ) {                       //estrae da fs i caratteri e li mette in line finchè non trova \n
            
            //std::cout << line << std::endl;
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());  //cancella da line tutti i \n
            line.push_back(' ');                                                  //aggiunge uno spazio bianco alla fine così che words contenga anche l'ultima parola di line
            
            std::string delimiter = " ";
            size_t pos = 0;
            std::vector<std::string> words;
            std::string word;
            while ((pos = line.find(delimiter)) != std::string::npos) {    //chiama pos la posizione del primo delimiter in line e se trova il delimiter in line e ci sono altri delimiter ...
                word = line.substr(0, pos);                                  //seleziona la parte da 0 a pos e la mette in word
                line.erase(0, pos + delimiter.length());                     //cancella la parte di line da 0 a pos + lunghezza delimiter
                words.push_back(word);                                       //mette word alla fine di words
            }

            std::vector< std::string > words_cleaned;
            for( unsigned i=0; i<words.size(); ++i ) {
                if( isNumber(words[i]) ) words_cleaned.push_back( words[i] );
            }
            words_cleaned.push_back( words[words.size()-1] );
	    
            if( words[0]=="===" && words[1]=="Event" && wasReadingEvent ) {
                for(j=0;j<15;j++){
                    if(indArrayTree!=1){
                        if(indArrayTree==j){std::cout << "EVENTO: " << ev << "..." << indArrayTree << " SCINTILLAZIONI  "<<std::endl;}
                    }
                }
                
		
                if( ev % 100 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;
			
                tree->Fill();
                indArrayTree=0;
                nch = 0;
                ch = -1;
                wasReadingEvent = false;
                
            } else if( words[0]!="===" && words_cleaned.size()==8 ) {
	      if(stof(words_cleaned[7])!=0.){
		ratecount[stoi(words_cleaned[0])] += stof(words_cleaned[7]);
                    conteggiRateCount[stoi(words_cleaned[0])]++;
	      }
	      if((stod(words_cleaned[4])<sogliab)&(stod(words_cleaned[4])>sogliaa)){
		nch += 1;
		wasReadingEvent = true;
		ch            = atoi(words_cleaned[0].c_str());
		chs      [indArrayTree]  = atoi(words_cleaned[0].c_str());
		base     [indArrayTree] = atof(words_cleaned[2].c_str());
		vamp     [indArrayTree] = atof(words_cleaned[3].c_str());
		vcharge  [indArrayTree] = atof(words_cleaned[4].c_str());
		letime   [indArrayTree] = atof(words_cleaned[5].c_str());
		tetime   [indArrayTree] = atof(words_cleaned[6].c_str());
		lmttime  [indArrayTree] = tetime[indArrayTree]-letime[indArrayTree];
		indArrayTree+=1;
	      }
            }
            
            if( words[0]=="===" && words[1]=="Event" && wasReadingEvent==false ) {
                ev            = atoi(words[2].c_str());
            }
            
        } // while get lines
        
    } // if file is good
    
    if( wasReadingEvent )
    {
        std::cout << "   ... analyzing event: " << ev << std::endl;
        tree->Fill();
    }
    
    fs.close();
    
    tree->Write();
    outfile->Close();
    
    std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;
    /*for(indArrayTree=0;indArrayTree<15;indArrayTree++){
     ratecount[indArrayTree]=ratecount[indArrayTree]/(conteggiRateCount/12.);
     std::cout << "canale:" << indArrayTree << "... rate medio:" << ratecount[indArrayTree] << std::endl;
     }*/
    std::string outfileName2;
    pos = 0;
    if((pos = fileName.find(".")) != std::string::npos) {
        std::string prefix = fileName.substr(0, pos);
        outfileName2 ="Rate_" + prefix + ".root";
    }
    std::cout << "histo rate saved in "<< outfileName2 << std::endl;
    TFile f(outfileName2.c_str(), "recreate");
    f.cd();
    TH1F histo("rate_histo", "rate", 17, 0, 16);
    for(indArrayTree=0;indArrayTree<16;indArrayTree++){
      if(conteggiRateCount[indArrayTree]!=0){
	ratecount[indArrayTree]=ratecount[indArrayTree]/(conteggiRateCount[indArrayTree]);
	histo.Fill(indArrayTree,ratecount[indArrayTree]);
      }
    }
    histo.Write();
    f.Close();
    
    return 0;
    
}



bool isNumber(const std::string& s) {
    
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || (*it)==std::string(".") || (*it)==std::string("-")) ) ++it;
    return !s.empty() && it == s.end();
    
}

