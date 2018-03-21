#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include <string> 

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>




bool isNumber(const std::string& s);


int main( int argc, char* argv[]) {
    
    if( argc != 5 ) {
        
        std::cout << "USAGE: ./measCleanToTreeSC [filename] [soglia bassa] [soglia alta] [channel]" << std::endl;
        exit(1);
        
    }
    
    std::string fileName(argv[1]);
    double sogliab(std::stod(argv[2]));
    double sogliaa(std::stod(argv[3]));
    int canale(std::stoi(argv[4]));

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
        outfileName = "ch_"+ std::to_string(canale) + "_" + prefix + ".root";
    }
    
    TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
    TTree* tree = new TTree( "tree", "" );
    
    int ev;
    float base;
    float vamp;
    float vcharge;
    float spectrum;
    float letime;
    float tetime;
    float lmttime;
    
    tree->Branch( "ev"       , &ev      , "ev/I"            );
    tree->Branch( "base"     , &base     , "base/F"     );
    tree->Branch( "vamp"     , &vamp     , "vamp/F"     );
    tree->Branch( "vcharge"  , &vcharge  , "vcharge/F"  );
    tree->Branch( "spectrum" , &spectrum , "spectrum/F"  );
    tree->Branch( "letime"   , &letime   , "letime/F"   );
    tree->Branch( "tetime"   , &tetime   , "tetime/F"   );
    tree->Branch( "lmttime"  , &lmttime  , "lmttime/F"  );
    
    
    
    std::string line;
    bool wasReadingEvent = false;
    int ch = -1;
    
    if( fs.good() ) {
        
        std::cout << "-> Starting parsing file." << std::endl;
        
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
               
		
                if( ev % 100 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;
			
                tree->Fill();
                ch = -1;
                wasReadingEvent = false;
                
            } else if( words[0]!="===" && words_cleaned.size()==8 ) {
                if(atoi(words_cleaned[0].c_str())==canale){
                    if((stod(words_cleaned[4])<sogliab)&(stod(words_cleaned[4])>sogliaa)){
                        wasReadingEvent = true;
                        ch            = atoi(words_cleaned[0].c_str());
                        base      = atof(words_cleaned[2].c_str());
                        vamp      = atof(words_cleaned[3].c_str());
                        vcharge   = atof(words_cleaned[4].c_str());
                        spectrum  = fabs(vcharge);
                        letime    = atof(words_cleaned[5].c_str());
                        tetime    = atof(words_cleaned[6].c_str());
                        lmttime   = tetime-letime;
                    }
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
    
    return 0;
    
}



bool isNumber(const std::string& s) {
    
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || (*it)==std::string(".") || (*it)==std::string("-")) ) ++it;
    return !s.empty() && it == s.end();
    
}

