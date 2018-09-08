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
// $Id: B3DetectorConstruction.cc 101905 2016-12-07 11:34:39Z gunter $
//
/// \file B3DetectorConstruction.cc
/// \brief Implementation of the B3DetectorConstruction class

#include "B3DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3DetectorConstruction::B3DetectorConstruction()
: G4VUserDetectorConstruction(),
   fCheckOverlaps(true)
{
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3DetectorConstruction::~B3DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3DetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;

  G4Element* Na = man->FindOrBuildElement("Na", isotopes);
  G4Element* I = man->FindOrBuildElement("I", isotopes);
  
  G4Material* NaI = new G4Material("NaI", 3.67*g/cm3, 2);
  NaI->AddElement(Na, 1);
  NaI->AddElement(I, 1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B3DetectorConstruction::Construct()
{  
  // Gamma detector Parameters

  G4double cryst_dX1 = 1.3 * cm, cryst_dY1 = 1.8 * cm;
  G4double cryst_dX = 8.4 * cm, cryst_dY = 11.8 * cm;
  G4double cryst_dZ = 28. * cm;
  G4int nb_cryst = 16;
  G4int nb_rings = 1;

  G4double dPhi = twopi / nb_cryst, half_dPhi = .5 * dPhi;
  G4double cosdPhih = std::cos(.5 * dPhi);
  
  G4double ring_R1 = .5 * 9.5 * cm;
  G4double ring_R2 = (ring_R1 + cryst_dZ) / cosdPhih;

  G4double tandPhi = std::tan(half_dPhi);

  G4double detector_dZ = nb_rings * cryst_dX;

  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* shell_mat = nist->FindOrBuildMaterial("G4_Al"); 		// Decommentare per cristallo rivestito
//  G4Material* shell_mat = nist->FindOrBuildMaterial("G4_AIR");	// Decommentare per cristallo non rivestito
  G4Material* cryst_mat   = nist->FindOrBuildMaterial("NaI");
  
  //     
  // World
  //
  G4double world_sizeXY = 2.4*ring_R2;
  G4double world_sizeZ  = 1.2*detector_dZ;
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ); //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        default_mat,         //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      fCheckOverlaps);       // checking overlaps 
                 
  //
  // Shell
  //
  G4double a = 1.01;   // grandezza relativa rivestimento (rispetto cristallo) [1.01 -> 1%]

  G4Trd *solidShell =
    new G4Trd("shell", a * cryst_dX1/2, a * cryst_dX/2, a * cryst_dY1/2, a * cryst_dY/2, a * cryst_dZ/2);
  
  G4LogicalVolume* logicShell =                         
    new G4LogicalVolume(solidShell,     //its solid
			shell_mat,      //its material
			"Shell");       //its name

  //     
  // define crystal
  //
  G4Trd *solidCryst = new G4Trd("crystal", cryst_dX1/2, cryst_dX/2, cryst_dY1/2, cryst_dY/2, cryst_dZ/2);
                     
  G4LogicalVolume* logicCryst = 
    new G4LogicalVolume(solidCryst,          //its solid
                        cryst_mat,           //its material
                        "CrystalLV");        //its name
  /* 
  // place crystals within a ring 
  //
  for (G4int icrys = 0; icrys < 1 ; icrys++) {
    G4double phi = icrys*dPhi;
    G4RotationMatrix rotm  = G4RotationMatrix();
    rotm.rotateY(90*deg); 
    rotm.rotateZ(phi);
    G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);     
    G4ThreeVector position = (ring_R1+0.5*cryst_dZ)*uz;
    G4Transform3D transform = G4Transform3D(rotm,position);

    new G4PVPlacement(
		      transform,             //rotation,position
                      logicCryst,            //its logical volume
                      "crystal",             //its name
                      logicShell,            //its mother  volume
                      false,                 //no boolean operation
                      icrys,                 //copy number
                      fCheckOverlaps);       // checking overlaps 
  }
  */
  new G4PVPlacement(0,                     //no rotation
		    G4ThreeVector(),       //at (0,0,0)
		    logicCryst,            //its logical volume
		    "crystal",             //its name
		    logicShell,            //its mother  volume
		    false,                 //no boolean operation
		    0,                 //copy number
		    fCheckOverlaps);       // checking overlaps 
  //
  // full detector
  //
  G4Trd *solidDetector =
    new G4Trd("detector", a * cryst_dX1/2, a * cryst_dX/2, a * cryst_dY1/2, a * cryst_dY/2, a * cryst_dZ/2);

  G4LogicalVolume* logicDetector =                         
    new G4LogicalVolume(solidDetector,       //its solid
                        default_mat,         //its material
                        "Detector");         //its name
                                 
  // 
  // place rings within detector 
  //
  for (G4int iring = 0; iring < nb_rings ; iring++) {
    
    new G4PVPlacement(0,                     //no rotation 
		      G4ThreeVector(),        //position (0,0,0)
		      logicShell,            //its logical volume
		      "Shell",               //its name
		      logicDetector,         //its mother  volume
		      false,                 //no boolean operation
		      0,                     //copy number
		      fCheckOverlaps);       // checking overlaps
  }

  //
  // place detector in world
  //                    
  G4RotationMatrix rotm  = G4RotationMatrix();
  rotm.rotateY(90*deg); 
  rotm.rotateZ(0.);
  G4ThreeVector uz = G4ThreeVector(std::cos(0.),  std::sin(0.),0.);     
  G4ThreeVector position = (ring_R1+0.5*cryst_dZ)*uz;
  G4Transform3D transform = G4Transform3D(rotm,position);
  
  new G4PVPlacement(
		    transform,             //rotation,position
		    logicDetector,            //its logical volume
		    "crystal",             //its name
		    logicWorld,            //its mother  volume
		    false,                 //no boolean operation
		    0,                 //copy number
		    fCheckOverlaps);       // checking overlaps 
  //
  // Source
  //
  /*
  G4double source_radius = 1.5*cm;
  G4double source_dZ = 0.5*cm;
  G4Material* source_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
    
  G4Tubs* solidSource =
    new G4Tubs("Source", 0., source_radius, 0.5*source_dZ, 0., twopi);
      
  G4LogicalVolume* logicSource =                         
    new G4LogicalVolume(solidSource,        //its solid
                        source_mat,         //its material
                        "SourceLV");        //its name
               
  //
  // place source in world
  //
  
  G4RotationMatrix* rotm2  = new G4RotationMatrix();
  rotm2->rotateX(90*deg); 
  
  new G4PVPlacement(
		    rotm2,              //rotation, position
                    G4ThreeVector(),         //at (0,0,0)
                    logicSource,             //its logical volume
                    "Source",                //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  */

  // 
  // Visualization attributes
  //
  
  logicShell->SetVisAttributes (G4VisAttributes::GetInvisible());
  logicDetector->SetVisAttributes (G4VisAttributes::GetInvisible());    

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl; 

  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  
  // declare crystal as a MultiFunctionalDetector scorer
  //  
  G4MultiFunctionalDetector* cryst = new G4MultiFunctionalDetector("crystal");
  G4SDManager::GetSDMpointer()->AddNewDetector(cryst);
  G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("edep");
  cryst->RegisterPrimitive(primitiv1);
  SetSensitiveDetector("CrystalLV",cryst);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
