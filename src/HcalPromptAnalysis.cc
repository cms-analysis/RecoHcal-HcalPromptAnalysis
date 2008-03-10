#include "../interface/HcalPromptAnalysis.h"

// system include files
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
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"


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
#include "CondFormats/HcalObjects/interface/HcalQIECoder.h"
#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"

#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"


#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"

#include <boost/regex.hpp>
#include <cmath>

#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "Utilities/Timing/interface/TimerStack.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

HcalPromptAnalysis::HcalPromptAnalysis(const edm::ParameterSet& iConfig)
{
   file_ = new TFile( iConfig.getParameter<std::string>("outputfile").c_str(), "recreate");
   ADCs = new TH1F("ADCs","ADCs",21,-0.5,20.5);
   energyone = new TH1F("energyone","Energy in channel",210,-1.,6.);
   hcalEnergy = new TH1F("hcalEnergy","Max Energy in HB tower crossed by CosmicMuon track",210,-1.,6.);
   hcalEnergyplus = new TH1F("hcalEnergyplus","Max Energy in HB+ tower crossed by CosmicMuon track",210,-1.,6.);
   hcalEnergyminus = new TH1F("hcalEnergyminus","Max Energy in HB- tower crossed by CosmicMuon track",210,-1.,6.);
   hcalEnergytop = new TH1F("hcalEnergytop","Max Energy in HB tower crossed by muon (top)",210,-1.,6.);
   hcalEnergybot = new TH1F("hcalEnergybot","Max Energy in HB tower crossed by muon (bot)",210,-1.,6.);
   hcalEnergyveto = new TH1F("hcalEnergyveto","Energy in all HB towers not crossed by CosmicMuon track",210,-1.,6.);
//   hcalEnergyminus = new TH1F("hcalEnergyminus","Energy HB-",210,-1.,6.);
//   hcalEnergyplus = new TH1F("hcalEnergyplus","Energy HB+",210,-1.,6.);
   timingh = new TH1F("timingh","HB Timing (signal > 10 ADC)",100,0,10);
   profile = new TProfile("profile","HB Average signal (signals > 10 ADC)",10,-0.5,9.5,0,40);
   pedprofile = new TProfile("pedprofile","HB Average signal (ped)",10,-0.5,9.5,0,40);
   profiletop = new TProfile("profiletop","HB Average signal top (signals > 10 ADC)",10,-0.5,9.5,0,40);
   profilebot = new TProfile("profilebot","HB Average signal bot (signals > 10 ADC)",10,-0.5,9.5,0,40);
   hotimingh = new TH1F("hotimingh","HO Timing (signal > 10 ADC)",100,0,10);
   hoprofile = new TProfile("hoprofile","HO Average signal (signals > 10 ADC)",10,-0.5,9.5,0,40);
   sector = new TH1F("sector","DT sector for all segments",21,-0.5,20.5);
   hcalphi = new TH1F("hcalphi","Hcal iphi of muon intersected towers",72,.5,72.5);
   hcaleta = new TH1F("hcaleta","Hcal ieta of muon intersected towers",33,-16.5,16.5);
   hcaletaphi = new TH2F("hcaletaphi","Hcal iphi v. ieta of muon intersected tracks",33,-16.5,16.5,72,.5,72.5);
   phicorr = new TH2F("phicorr","Phi (HCAL) v. Phi (DTs)",1000,-4,4,72,-3.12,3.12);
   phicorr2 = new TH2F("phicorr2","Phi (HCAL) v. Phi (DTs)",1000,-4,4,72,-3.12,3.12);
   etacorr = new TH2F("etacorr","Eta (HCAL) v. Eta (DTs)",1000,-1.5,1.5,1000,-1.5,1.5);
   crossedtowers = new TH1F("crossedtowers","Number of HCAL towers crossed",11,-0.5,10.5);
   for (int i=0; i<4; i++){
     capid[i] = new TH1F("capid","ped for capid",8,-0.5,9.5);
   }
   nEventsKept = 0;
   //inputRecoTrackColl_ = iConfig.getParameter<edm::InputTag>("inputRecoTrackColl");
   // TrackAssociator parameters
   edm::ParameterSet parameters = iConfig.getParameter<edm::ParameterSet>("TrackAssociatorParameters");
   parameters_.loadParameters( parameters );
   trackAssociator_.useDefaultPropagator();
   goodevents = 0;
   totalevents = 0;
}

HcalPromptAnalysis::~HcalPromptAnalysis(){
   file_->cd();
   hcalEnergy->Write();
   //hcalEnergytop->Write();
   //hcalEnergybot->Write();
   //hcalEnergyveto->Write();
   //hcalEnergyminus->Write();
   //hcalEnergyplus->Write();
   //timingh->Write();
   profile->Write();
   pedprofile->Write();
   //hotimingh->Write();
   hoprofile->Write();
   //ADCs->Write();
   //energyone->Write();
   //sector->Write();
   //hcalphi->Write();
   hcalEnergyplus->Write();
   hcalEnergyminus->Write();
   profiletop->Write();
   profilebot->Write();
   hcalphi->Write();
   hcaleta->Write();
   hcaletaphi->Write();
   phicorr->Write();
   phicorr2->Write();
   etacorr->Write();
   crossedtowers->Write();
   for (int i=0; i<4; i++){
     capid[i]->Write();
   }
   file_->Close();
   std::cout << "Number of Events:  " << totalevents << std::endl;
   std::cout << "Number of Events kept:  " << goodevents << std::endl;
}

void HcalPromptAnalysis::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   totalevents++;
   // calo geometry
   edm::ESHandle<CaloGeometry> geometry;
   iSetup.get<IdealGeometryRecord>().get(geometry);
   edm::ESHandle<HcalDbService> cond;
   iSetup.get<HcalDbRecord>().get(cond);
   // muon geometry
   edm::ESHandle<DTGeometry> dtGeom;
   iSetup.get<MuonGeometryRecord>().get(dtGeom);

   // get reco tracks
   Handle<reco::TrackCollection> recoTracks;
   iEvent.getByLabel("cosmicMuons", recoTracks);

   Handle<HBHEDigiCollection> hbhe; iEvent.getByLabel("hcalZeroSuppressedDigis",hbhe);
   Handle<HODigiCollection> ho;     iEvent.getByLabel("hcalZeroSuppressedDigis",ho);
   Handle<HFDigiCollection> hf;     iEvent.getByLabel("hcalZeroSuppressedDigis",hf);

   // Get the rechit collection from the event
   edm::Handle<DTRecSegment4DCollection> all4DSegments;
   iEvent.getByLabel("dt4DSegments", all4DSegments);


   int nTracks = recoTracks->size();
   std::cout << "# of tracks: " << nTracks << std::endl;
   // Loop over tracks
   HcalDetId crossedID;
   float dtphi = -99;
   float dteta = -99;
   int nHcalcrossed = 0;
   for(reco::TrackCollection::const_iterator rT = recoTracks->begin(); rT != recoTracks->end(); ++rT){
     TrackDetMatchInfo info = trackAssociator_.associate(iEvent, iSetup, *rT, parameters_);
     DetId centerId;
     nHcalcrossed = info.crossedHcalRecHits.size();
     crossedtowers->Fill(nHcalcrossed);
     if (nHcalcrossed == 1) goodevents++;
     //std::cout << "nHcalcrossed: " << nHcalcrossed << std::endl;
     centerId = info.findMaxDeposition(TrackDetMatchInfo::HcalRecHits);
     DTChamberId outerid = DTChamberId((*rT).outerDetId());
     DTChamberId innerid = DTChamberId((*rT).innerDetId());
     if (nTracks == 1) crossedID = HcalDetId(centerId);
     double X = (*rT).outerX();
     double Y = (*rT).outerY();
     dtphi = atan2(Y,X);
     //print some of the track info
     //std::cout << "Number of rechits: " << (*rT).recHitsSize() << std::endl;
     //std::cout << "Innermost DetID: " << DTChamberId((*rT).outerDetId()) << std::endl;
     //std::cout << "Outermost DetID: " << DTChamberId((*rT).innerDetId()) << std::endl;
     //if (nTracks == 1) std::cout << "Hcal Crossed ID: " << crossedID << std::endl;
   }

   // DT segments
   DTRecSegment4DCollection::const_iterator segment;
   float DTPhitop = -99;
   float DTPhibot = -99;
   int nSeg = all4DSegments->size();
   bool keepthisevent = false;
   int nSegtop = 0;
   int nSegbot = 0;
   if (nSeg > 4){
     float avePhitop = 0;
     float avePhibot = 0;
     for (segment = all4DSegments->begin(); segment != all4DSegments->end(); ++segment){
       DTChamberId segId = (*segment).chamberId();
       const DTChamber* dtchamber = dtGeom->chamber(segId);
       LocalPoint dtlp = (*segment).localPosition();
       GlobalPoint dtgp = dtchamber->toGlobal(dtlp);
//       float dtphi = dtgp.phi();
//       float dteta = dtgp.eta();
//       int dtstation = segId.station();
//       int dtwheel = segId.wheel();
       int dtsector = segId.sector();
       sector->Fill(dtsector);
       if (dtsector == 3 || dtsector == 4 || dtsector == 5 || dtsector == 13) {avePhitop = avePhitop + dtphi; nSegtop++;}
       if (dtsector == 9 || dtsector == 10 || dtsector == 11 || dtsector == 14) {avePhibot = avePhibot + dtphi; nSegbot++;}
       //std::cout << "Segment Found in wheel = " << dtwheel << ", dtstation = " << dtstation << ", sector = " << dtsector << std::endl;
     }
     if (nSegtop > 0 && nSegbot > 0){
       avePhitop = avePhitop/nSegtop;
       avePhibot = avePhibot/nSegbot;
       DTPhitop = avePhitop;
       DTPhibot = avePhibot;
       std::cout << "DT global phi  ->  Top: " << avePhitop << "   Bot: " << avePhibot << std::endl;
       float rotPhitop = avePhitop - 3.12;
       float phidiff = fabs(rotPhitop - avePhibot);
       std::cout << "Diff - pi : " << phidiff << std::endl;
       if (phidiff < 0.087){ keepthisevent = true; nEventsKept++; }
     }
   }

  edm::Handle<HBHERecHitCollection> hbherh;  iEvent.getByLabel("hbhereco",hbherh);
  edm::Handle<HORecHitCollection> horh;  iEvent.getByLabel("horeco",horh);
  edm::Handle<HFRecHitCollection> hfrh;  iEvent.getByLabel("hfreco",hfrh);

  HBHERecHitCollection::const_iterator hbheit;
  HORecHitCollection::const_iterator hoit;
  HFRecHitCollection::const_iterator hfit;

  // Hcal Digis
  int adcs[10];
  float fC[10];
  int maxadc = 0;
  for (HBHEDigiCollection::const_iterator j=hbhe->begin(); j!=hbhe->end(); j++){
    const HBHEDataFrame digi = (const HBHEDataFrame)(*j);
    HcalDetId id = digi.id();
    const HcalQIEShape* shape = cond->getHcalShape();
    const HcalQIECoder* coder = cond->getHcalCoder(id);
    int ieta = id.ieta();
    int iphi = id.iphi();
//    int idepth = id.depth();   
    GlobalPoint caloGP = geometry->getPosition(id);
//    float phi = caloGP.phi();
//    float eta = caloGP.eta();
    if (id.subdet() == 1){
      for (int TS = 0; TS < 10 && TS < digi.size(); TS++){
        adcs[TS] = digi[TS].adc(); 
        fC[TS] = coder->charge(*shape,digi[TS].adc(),digi[TS].capid());
        if (digi[TS].adc() > maxadc) maxadc = digi[TS].adc();
      }
      if (ieta == -1 && iphi == 13){
        for (int i = 0; i < 10; i++){
          pedprofile->Fill(i,fC[i]);
          capid[digi[i].capid()]->Fill(fC[i]);
        }
      }
      if (maxadc > 7){
        for (int i = 0; i < 10; i++){
          profile->Fill(i,adcs[i]);
        }
        if (iphi == 18 || iphi == 19 || iphi == 17 || iphi == 20){
          for (int i = 0; i < 10; i++){
            profiletop->Fill(i,adcs[i]);
          }
        }
        if (iphi == 54 || iphi == 55 || iphi == 53 || iphi == 56){
          for (int i = 0; i < 10; i++){
            profilebot->Fill(i,adcs[i]);
          }
        }
      }
    }
    maxadc = 0;
  }

if (nSegtop > 0 && nSegbot > 0){

  // Hcal rechits
  float energy = 0;
  for (hbheit  = hbherh->begin(); hbheit != hbherh->end(); hbheit++) {
    HcalDetId idrh = hbheit->id();
    if (crossedID == idrh && nHcalcrossed == 1){
      GlobalPoint hcalGP = geometry->getPosition(idrh);
      hcalphi->Fill(idrh.iphi());
      hcaleta->Fill(idrh.ieta());
      hcaletaphi->Fill(idrh.ieta(),idrh.iphi());
      phicorr->Fill(dtphi,hcalGP.phi());
      phicorr2->Fill(DTPhitop,hcalGP.phi());
      etacorr->Fill(dteta,hcalGP.eta());
      energy = 0;
      energy = hbheit->energy();
      if (nTracks == 1) hcalEnergy->Fill(energy);
      if (nTracks == 1 && idrh.ieta() < 0) hcalEnergyminus->Fill(energy);
      if (nTracks == 1 && idrh.ieta() > 0) hcalEnergyplus->Fill(energy);
    }
  }

}

  for (HODigiCollection::const_iterator j=ho->begin(); j!=ho->end(); j++){
    const HODataFrame digi = (const HODataFrame)(*j);
    HcalDetId id = digi.id();
//    int ieta = id.ieta();
//    int iphi = id.iphi();
//    int idepth = id.depth();
    for (int TS = 0; TS < 10 && TS < digi.size(); TS++){
      adcs[TS] = digi[TS].adc();
      if (digi[TS].adc() > maxadc) maxadc = digi[TS].adc();
    }
    if (maxadc > 10){
      for (int i = 0; i < 10; i++){
        hoprofile->Fill(i,adcs[i]);
      }
    }
    maxadc = 0;
  }

}

DEFINE_FWK_MODULE(HcalPromptAnalysis);
