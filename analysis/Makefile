CC = g++
CFLAGS = -Wall -c -g -fpermissive 


INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lMathCore  -lMinuit  -lPhysics -lTreePlayer -lHtml -lGenVector -lEG -lz -lRooFitCore -lRooFit -lSpectrum

asciiToTree: asciiToTree.cpp 
	$(CC) -Wall $(INCLUDES) -o asciiToTree asciiToTree.cpp $(ROOTFLAG) $(EXTRALIBS)

measToTree: measToTree.cpp 
	$(CC) -Wall $(INCLUDES) -o measToTree measToTree.cpp $(ROOTFLAG) $(EXTRALIBS)

measCleanToTree: measCleanToTree.cpp 
	$(CC) -Wall $(INCLUDES) -o measCleanToTree measCleanToTree.cpp $(ROOTFLAG) $(EXTRALIBS)

time_tree: time_tree.cpp 
	$(CC) -Wall $(INCLUDES) -o time_tree time_tree.cpp $(ROOTFLAG) $(EXTRALIBS)

time_tree_soglia: time_tree_soglia.cpp 
	$(CC) -Wall $(INCLUDES) -o time_tree_soglia time_tree_soglia.cpp $(ROOTFLAG) $(EXTRALIBS)

time_tree_soglia2: time_tree_soglia2.cpp 
	$(CC) -Wall $(INCLUDES) -o time_tree_soglia2 time_tree_soglia2.cpp $(ROOTFLAG) $(EXTRALIBS)

time_resolution: time_resolution.cpp 
	$(CC) -Wall $(INCLUDES) -o time_resolution time_resolution.cpp $(ROOTFLAG) $(EXTRALIBS)

time_graph: time_graph.cpp 
	$(CC) -Wall $(INCLUDES) -o time_graph time_graph.cpp $(ROOTFLAG) $(EXTRALIBS)

checkPulseShape: checkPulseShape.cpp 
	$(CC) -Wall $(INCLUDES) -o checkPulseShape checkPulseShape.cpp $(ROOTFLAG) $(EXTRALIBS)


makeSpectrum: makeSpectrum.cpp
	$(CC) -Wall $(INCLUDES) -o makeSpectrum makeSpectrum.cpp $(ROOTFLAG) $(EXTRALIBS)

makeSpectrum_all: makeSpectrum_all.cpp
	$(CC) -Wall $(INCLUDES) -o makeSpectrum_all makeSpectrum_all.cpp $(ROOTFLAG) $(EXTRALIBS)

fitRoot: fitRoot.cpp
	$(CC) -Wall $(INCLUDES) -o fitRoot fitRoot.cpp $(ROOTFLAG) $(EXTRALIBS)

spectrum_allchannels: spectrum_allchannels.cpp
	$(CC) -Wall $(INCLUDES) -o spectrum_allchannels spectrum_allchannels.cpp $(ROOTFLAG) $(EXTRALIBS)

noiseHisto: noiseHisto.cpp
	$(CC) -Wall $(INCLUDES) -o noiseHisto noiseHisto.cpp $(ROOTFLAG) $(EXTRALIBS)

noiseHisto_all: noiseHisto_all.cpp
	$(CC) -Wall $(INCLUDES) -o noiseHisto_all noiseHisto_all.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_noise: baseline_noise.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_noise baseline_noise.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_time: baseline_time.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_time baseline_time.cpp stat.o $(ROOTFLAG) $(EXTRALIBS)

baseline_time_prova: baseline_time_prova.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_time_prova baseline_time_prova.cpp stat.o $(ROOTFLAG) $(EXTRALIBS)

baseline_pulseshape: baseline_pulseshape.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_pulseshape baseline_pulseshape.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_pulseshape2: baseline_pulseshape2.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_pulseshape2 baseline_pulseshape2.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_voltage: baseline_voltage.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_voltage baseline_voltage.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_error: baseline_error.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_error baseline_error.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_error_rumore: baseline_error_rumore.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_error_rumore baseline_error_rumore.cpp $(ROOTFLAG) $(EXTRALIBS)

baseline_error2: baseline_error2.cpp
	$(CC) -Wall $(INCLUDES) -o baseline_error2 baseline_error2.cpp $(ROOTFLAG) $(EXTRALIBS)

charge: charge.cpp
	$(CC) -Wall $(INCLUDES) -o charge charge.cpp $(ROOTFLAG) $(EXTRALIBS)

charge_base: charge_base.cpp
	$(CC) -Wall $(INCLUDES) -o charge_base charge_base.cpp $(ROOTFLAG) $(EXTRALIBS)

charge_base2: charge_base2.cpp
	$(CC) -Wall $(INCLUDES) -o charge_base2 charge_base2.cpp $(ROOTFLAG) $(EXTRALIBS)

voltage_file: voltage_file.cpp
	$(CC) -Wall $(INCLUDES) -o voltage_file voltage_file.cpp $(ROOTFLAG) $(EXTRALIBS)

verify: verify.cpp
	$(CC) -Wall $(INCLUDES) -o verify verify.cpp $(ROOTFLAG) $(EXTRALIBS)

RisTempScintillazione: RisTempScintillazione.cpp
	$(CC) -Wall $(INCLUDES) -o RisTempScintillazione RisTempScintillazione.cpp $(ROOTFLAG) $(EXTRALIBS)

bario: bario.cpp 
	$(CC) -Wall $(INCLUDES) -o bario bario.cpp $(ROOTFLAG) $(EXTRALIBS)

cesio: cesio.cpp 
	$(CC) -Wall $(INCLUDES) -o cesio cesio.cpp $(ROOTFLAG) $(EXTRALIBS)

cobalto: cobalto.cpp 
	$(CC) -Wall $(INCLUDES) -o cobalto cobalto.cpp $(ROOTFLAG) $(EXTRALIBS)

sodio: sodio.cpp 
	$(CC) -Wall $(INCLUDES) -o sodio sodio.cpp $(ROOTFLAG) $(EXTRALIBS)

sorgenti: sorgenti.cpp 
	$(CC) -Wall $(INCLUDES) -o sorgenti sorgenti.cpp $(ROOTFLAG) $(EXTRALIBS)

tresVSthresh: tresVSthresh.cpp 
	$(CC) -Wall $(INCLUDES) -o tresVSthresh tresVSthresh.cpp $(ROOTFLAG) $(EXTRALIBS)

all: asciiToTree checkPulseShape measToTree makeSpectrum spectrum_allchannels noiseHisto noiseHisto_all makeSpectrum_all baseline_noise baseline_pulseshape measCleanToTree charge fitRoot baseline_pulseshape2 baseline_time baseline_voltage voltage_file verify charge_base baseline_time_prova RisTempScintillazione charge_base2 time_tree bario time_tree_soglia time_tree_soglia2 time_resolution cesio cobalto sodio sorgenti baseline_error baseline_error2 baseline_error_rumore tresVSthresh time_graph


clean:
	rm asciiToTree checkPulseShape measToTree makeSpectrum noiseHisto spectrum_allchannels noiseHisto_all makeSpectrum_all baseline_noise baseline_pulseshape measCleanToTree charge fitRoot baseline_pulseshape2 baseline_time baseline_voltage voltage_file verify charge_base baseline_time_prova RisTempScintillazione charge_base2 time_tree bario time_tree_soglia time_tree_soglia2 time_resolution cesio cobalto sodio sorgenti baseline_error baseline_error2 baseline_error_rumore tresVSthresh time_graph


