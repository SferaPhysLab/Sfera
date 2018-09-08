{

// APERTURA DEI FILE

  TFile *bgfile = TFile::Open("~/Sfera/SIMULAZIONI/build/Experimental/noise_Measurements_Only_6_19_2018.root");			// rootfile del fondo
  TFile *gammanoalfile = TFile::Open("~/Sfera/CRISTALLO/build/Simulazioni/gnal.root"); 						// rootfile dei fotoni in cristallo non rivestito
  TFile *gammafile = TFile::Open("~/Sfera/CRISTALLO/build/Simulazioni/gamma.root"); 						// rootfile dei fotoni in cristallo rivestito
  TFile *cesionoalfile = TFile::Open("~/Sfera/CRISTALLO/build/Simulazioni/cesionoal.root");					// rootfile del cesio in cristallo non rivestito
  TFile *cesiofile = TFile::Open("~/Sfera/CRISTALLO/build/Simulazioni/cesio.root");						// rootfile del cesio in cristallo rivestito
  TFile *expcesiofile = TFile::Open("~/Sfera/SIMULAZIONI/build/Experimental/Cesio_Ch_5_Measurements_Only_5_16_2018.root");	// rootfile del cesio sperimentale
 
/////////////////////////////////////////////////////////////////

// ISTOGRAMMI

  int nbin = 100;		// Numero di bin per tutti gli istogrammi
  double ch_i = 300;		// Carica minima per tutti gli istogrammi
  double ch_f = 2200;		// Carica massima per tutti gli istogrammi
  double chcut = 1700;		// Carica di taglio per il segnale: eventi con carica maggiore sono attribuiti al fondo e sono utilizzati per la sua normalizzazione

  double csk = 661.7 / 958.804;	// Costante di calibrazione: Energia / Carica del picco

  // Cesio sperimentale

  TH1D *hexpcesio = new TH1D("sperimentale", "", nbin, ch_i * csk, ch_f * csk);		// Istogramma del cesio sperimentale
  float charge[128];
  TTree *expcesiotree = (TTree *)expcesiofile->Get("tree");
  expcesiotree->SetBranchAddress("vcharge", charge);
  int nentries = expcesiotree->GetEntries();
  for (int i = 0 ; i < nentries ; i++) {
    expcesiotree->GetEntry(i);
    hexpcesio->Fill(csk * charge[5]);
  }

  // Fondo

  TH1D *hbg = new TH1D("fondo", "", nbin, ch_i * csk, ch_f * csk);	// Istogramma del fondo
  TTree *bgtree = (TTree *)bgfile->Get("tree");
  bgtree->SetBranchAddress("vcharge", charge);
  nentries = bgtree->GetEntries();
  for(int i = 0 ; i < nentries ; i++) {
    bgtree->GetEntry(i);
    hbg->Fill(csk * charge[10]);
  }

  double bgnorm = hexpcesio->Integral(hexpcesio->FindBin(chcut * csk), hexpcesio->FindBin(ch_f * csk)) / hbg->Integral(hbg->FindBin(chcut * csk), hbg->FindBin(ch_f * csk));	// Normalizzazione del fondo

  hexpcesio->Add(hbg, -bgnorm);	// Sottrazione del fondo


  // Fotoni in cristallo non rivestito

  TH1D *hgammanoal = new TH1D("wrongfotoni", "", nbin,  ch_i * csk, ch_f * csk);	// Istogramma dei fotoni in cristallo non rivestito
  TTree *gammanoaltree = (TTree *)gammanoalfile->Get("tree");
  float E;
  gammanoaltree->SetBranchAddress("Emod", &E);
  nentries = gammanoaltree->GetEntries();
  for(int i = 0 ; i < nentries ; i++) {
    gammanoaltree->GetEntry(i);
    hgammanoal->Fill(1000 * E);
  }

  hgammanoal->SetNormFactor(hexpcesio->Integral());	// Normalizzazione della simulazione

  // Grafico

  TCanvas *c1 = new TCanvas();
  c1->cd();

  hgammanoal->SetAxisRange(ch_i * csk, 800, "X");
  hgammanoal->SetLineColor(2);
  hgammanoal->SetXTitle("E [keV]");
  hgammanoal->SetYTitle("eventi");
  hgammanoal->Draw("E");
  hexpcesio->SetAxisRange(ch_i * csk, 800, "X");
  hexpcesio->Draw("Esame");


  // Fotoni in cristallo rivestito

  TH1D *hgamma = new TH1D("fotoni", "", nbin, ch_i * csk, ch_f * csk);
  TTree *gammatree = (TTree *)gammafile->Get("tree");
  gammatree->SetBranchAddress("Emod", &E);
  nentries = gammatree->GetEntries();
  for(int i = 0 ; i < nentries ; i++) {
    gammatree->GetEntry(i);
    hgamma->Fill(1000 * E);
  }

  hgamma->SetNormFactor(hexpcesio->Integral());

  // Grafico

  TCanvas *c2 = new TCanvas();
  c2->cd();

  hgamma->SetAxisRange(ch_i * csk, 800, "X");
  hgamma->SetLineColor(2);
  hgamma->SetXTitle("E [keV]");
  hgamma->SetYTitle("eventi");
  hgamma->Draw("E");
  hexpcesio->SetAxisRange(ch_i * csk, 800, "X");
  hexpcesio->Draw("Esame");


  // Cesio in cristallo non rivestito

  TH1D *hcesionoal = new TH1D("wrongcesio", "", nbin, ch_i * csk, ch_f * csk);
  TTree *cesionoaltree = (TTree *)cesionoalfile->Get("tree");
  cesionoaltree->SetBranchAddress("Emod", &E);
  nentries = cesionoaltree->GetEntries();
  for(int i = 0 ; i < nentries ; i++) {
    cesionoaltree->GetEntry(i);
    hcesionoal->Fill(1000 * E);
  }

  hcesionoal->SetNormFactor(hexpcesio->Integral());

  // Grafico

  TCanvas *c3 = new TCanvas();
  c3->cd();
  
  hcesionoal->SetAxisRange(ch_i * csk, 800, "X");
  hcesionoal->SetLineColor(2);
  hcesionoal->SetXTitle("E [keV]");
  hcesionoal->SetYTitle("eventi");
  hcesionoal->Draw("E");
  hexpcesio->SetAxisRange(ch_i * csk, 800, "X");
  hexpcesio->Draw("Esame");


  // Cesio in cristallo rivestito

  TH1D *hcesio = new TH1D("cesio", "", nbin, ch_i * csk, ch_f * csk);
  TTree *cesiotree = (TTree *)cesiofile->Get("tree");
  cesiotree->SetBranchAddress("Emod", &E);
  nentries = cesiotree->GetEntries();
  for(int i = 0 ; i < nentries ; i++) {
    cesiotree->GetEntry(i);
    hcesio->Fill(1000 * E);
  }

  hcesio->SetNormFactor(hexpcesio->Integral());

  // Grafico

  TCanvas *c4 = new TCanvas();
  c4->cd();

  hcesio->SetAxisRange(ch_i * csk, 800, "X");
  hcesio->SetLineColor(2);
  hcesio->SetXTitle("E [keV]");
  hcesio->SetYTitle("eventi");
  hcesio->Draw("E");
  hexpcesio->SetAxisRange(ch_i * csk, 800, "X");
  hexpcesio->Draw("Esame");

}
