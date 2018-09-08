//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B3aRunAction.cc 99559 2016-09-27 07:02:21Z gcosmo $
//
/// \file B3aRunAction.cc
/// \brief Implementation of the B3aRunAction class

#include "B3aRunAction.hh"
#include "B3PrimaryGeneratorAction.hh"
#include "B3Analysis.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3aRunAction::B3aRunAction()
 : G4UserRunAction(),
   fGoodEvents(0)
{  
 
  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fGoodEvents);
  
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B3Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("Eabs0","Edep in absorber 0", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs1","Edep in absorber 1", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs2","Edep in absorber 2", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs3","Edep in absorber 3", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs4","Edep in absorber 4", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs5","Edep in absorber 5", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs6","Edep in absorber 6", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs7","Edep in absorber 7", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs8","Edep in absorber 8", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs9","Edep in absorber 9", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs10","Edep in absorber 10", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs11","Edep in absorber 11", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs12","Edep in absorber 12", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs13","Edep in absorber 13", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs14","Edep in absorber 14", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Eabs15","Edep in absorber 15", 100, 100.*keV, 2000*keV);
  analysisManager->CreateH1("Total Eabs","Total Edep in absorber", 200, 100.*keV, 1700*keV);

  // Creating ntuple
  //
  analysisManager->CreateNtuple("B3", "Edep");
  analysisManager->CreateNtupleDColumn("Eabs0");
  analysisManager->CreateNtupleDColumn("Eabs1");
  analysisManager->CreateNtupleDColumn("Eabs2");
  analysisManager->CreateNtupleDColumn("Eabs3");
  analysisManager->CreateNtupleDColumn("Eabs4");
  analysisManager->CreateNtupleDColumn("Eabs5");
  analysisManager->CreateNtupleDColumn("Eabs6");
  analysisManager->CreateNtupleDColumn("Eabs7");
  analysisManager->CreateNtupleDColumn("Eabs8");
  analysisManager->CreateNtupleDColumn("Eabs9");
  analysisManager->CreateNtupleDColumn("Eabs10");
  analysisManager->CreateNtupleDColumn("Eabs11");
  analysisManager->CreateNtupleDColumn("Eabs12");
  analysisManager->CreateNtupleDColumn("Eabs13");
  analysisManager->CreateNtupleDColumn("Eabs14");
  analysisManager->CreateNtupleDColumn("Eabs15");
  analysisManager->CreateNtupleDColumn("EabsTotal");  
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3aRunAction::~B3aRunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3aRunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  
  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
  
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B3";
  analysisManager->OpenFile(fileName);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3aRunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  
  // Merge accumulables 
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const B3PrimaryGeneratorAction* generatorAction
    = static_cast<const B3PrimaryGeneratorAction*>(
        G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String partName;
  if (generatorAction) 
  {
    G4ParticleDefinition* particle 
      = generatorAction->GetParticleGun()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }  
          
  // Print results
  //
  if (IsMaster())
  {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------"
     << G4endl
     << "  The run was " << nofEvents << " events ";
  }
  else
  {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------"
     << G4endl
     << "  The run was " << nofEvents << " "<< partName;
  }      
  G4cout
     << "; Nb of 'good' e+ annihilations: " << fGoodEvents.GetValue()  << G4endl
     << G4endl 
     << "------------------------------------------------------------" << G4endl 
     << G4endl;

  auto analysisManager = G4AnalysisManager::Instance();

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
