#ifndef RecoHcal_HcalPromptAnalysis_H
#define RecoHcal_HcalPromptAnalysis_H

#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/OrphanHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"

// calorimeter info
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"


#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"

#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"


#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"

#include <boost/regex.hpp>

#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "Utilities/Timing/interface/TimerStack.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include <math.h>
#include <iostream>
#include <map>
#include <iomanip>
#include <fstream>
#include <vector>



class HcalPromptAnalysis : public edm::EDAnalyzer {
 public:
   HcalPromptAnalysis(const edm::ParameterSet& iConfig);
   virtual ~HcalPromptAnalysis();
   void analyze (const edm::Event& iEvent, const edm::EventSetup& iSetup);

 protected:

 private:
   TrackDetectorAssociator trackAssociator_;
   bool useEcal_;
   bool useHcal_;
   bool useMuon_;
   bool useOldMuonMatching_;
   TFile* file_;
   int nTracks_;
   int totalevents;
   int goodevents;
   TH1F *hcalEnergy;
   TH1F *hcalEnergytop;
   TH1F *hcalEnergybot;
   TH1F *hcalEnergyveto;
   //TH1F *hcalEnergyplus;
   //TH1F *hcalEnergyminus;
   TH1F *timingh;
   TH1F *ADCs;
   TH1F *energyone;
   TH1F *sector;
   //TH2F *hcalphi;
   TProfile *profile;
   TProfile *profiletop;
   TProfile *profilebot;
   TH1F *hotimingh;
   TProfile *hoprofile;
   TH1F *hcalEnergyplus;
   TH1F *hcalEnergyminus;
   TH1F *hcalEnergyplus_prime;
   TH1F *hcalEnergyminus_prime;
   TH1F *hcalEnergyplus_doubleprime;
   TH1F *hcalEnergyminus_doubleprime;
   TH1F *hcalphi;
   TH1F *hcaleta;
   TH2F *hcaletaphi;
   TH2F *phicorr;
   TH2F *etacorr;
   TH2F *phicorr2;
   TH1F *crossedtowers;
   TProfile *pedprofile;
      	TH1F* h_rechit_energy;	
      	TH2F* h_eta_phi_HBHE;
      	TH1F* h_eta_HBHE;
      	TH1F* h_phi_HBHE;	
      	TH1F* h_digitime;
      	TH1F* h_nHitDT;
      	TH1F* h_calo_tower;
      	TH1F* h_rechit_energy_outphi;	
      	TH1F* h_jet_pt;
      	TH1F* h_jet_energy;
	TH1F* h_jet_eta;
	TH1F* h_jet_phi;
	TH2F* h_jet_eta_phi;
	TH2F* h_jet_eta_phi_l;
	TH1F* h_DeltaR_DT_Jet;
	TH1F* h_DeltaPhi_DT_Jet;
	TH1F* h_DeltaR_Jet_RecHit;
   TH1F *capid[4];
   //counters
   int nEventsKept;

   //edm::InputTag inputRecoTrackColl_;
   TrackAssociatorParameters parameters_;
};

#endif
