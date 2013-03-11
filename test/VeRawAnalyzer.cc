// -*- C++ -*-
//
// Package:    VeRawAnalyzer
// Class:      VeRawAnalyzer
// 
/**\class VeRawAnalyzer VeRawAnalyzer.cc Hcal/VeRawAnalyzer/src/VeRawAnalyzer.cc
   
Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// $Id$
//

// system include files
#include <fstream> 
#include <iostream>
#include <cmath>
#include <iosfwd>
#include <bitset>
#include <memory>
//#include "TFile.h"
//#include "TH1F.h"
//#include "TH2F.h"
//#include "TTree.h"

using namespace std;
// user include files

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

using namespace edm;
// this is to retrieve HCAL digi's

#include "DataFormats/HcalDetId/interface/HcalElectronicsId.h"
#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDigi/interface/HcalQIESample.h"
#include "DataFormats/HcalDetId/interface/HcalCalibDetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
//#include "DataFormats/HcalDigi/interface/HcalUpgradeDataFrame.h"
#include "DataFormats/HcalDigi/interface/HBHEDataFrame.h"
#include "DataFormats/HcalDigi/interface/HFDataFrame.h"
#include "DataFormats/HcalDigi/interface/HODataFrame.h"
#include "DataFormats/HcalDigi/interface/HcalCalibrationEventTypes.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h" 


#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
//#include "CalibFormats/HcalObjects/interface/HcalCoderUpgrade.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h" 
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"

#include "EventFilter/HcalRawToDigi/interface/HcalDCCHeader.h" 

#include "CondFormats/DataRecord/interface/L1GtTriggerMaskAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskTechTrigRcd.h"

//to get to BX 
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h" 
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"

//#include "Geometry/Records/interface/CaloGeometryRecord.h"
//#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
//#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"


// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

// Private include files from Eric
#include "FedGet.hh" 

static const float adc2fC[128]={-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5, 10.5,11.5,12.5,
				13.5,15.,17.,19.,21.,23.,25.,27.,29.5,32.5,35.5,38.5,42.,46.,50.,54.5,59.5,
				64.5,59.5,64.5,69.5,74.5,79.5,84.5,89.5,94.5,99.5,104.5,109.5,114.5,119.5,
				124.5,129.5,137.,147.,157.,167.,177.,187.,197.,209.5,224.5,239.5,254.5,272.,
				292.,312.,334.5,359.5,384.5,359.5,384.5,409.5,434.5,459.5,484.5,509.5,534.5,
				559.5,584.5,609.5,634.5,659.5,684.5,709.5,747.,797.,847.,897.,947.,997.,
				1047.,1109.5,1184.5,1259.5,1334.5,1422.,1522.,1622.,1734.5,1859.5,1984.5,
				1859.5,1984.5,2109.5,2234.5,2359.5,2484.5,2609.5,2734.5,2859.5,2984.5,
				3109.5,3234.5,3359.5,3484.5,3609.5,3797.,4047.,4297.,4547.,4797.,5047.,
				5297.,5609.5,5984.5,6359.5,6734.5,7172.,7672.,8172.,8734.5,9359.5,9984.5};		   		   

class VeRawAnalyzer : public edm::EDAnalyzer 
{
public:
  explicit VeRawAnalyzer(const edm::ParameterSet&);
  ~VeRawAnalyzer();
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
private:
  
  ////////////////////////////////////
  double dR(double eta1, double phi1, double eta2, double phi2);
  double phi12(double phi1, double en1, double phi2, double en2);
  double dPhiWsign(double phi1,double phi2);  
  ////////////////////////////////////
  std::string  fOutputFileName;
  
  edm::InputTag inputTag_;
  
  edm::ESHandle<HcalDbService> conditions;
  const HcalQIEShape* shape;
  
  int verbosity;
  double ratioHBMin_;
  double ratioHBMax_;
  double ratioHEMin_;
  double ratioHEMax_;

  double rmsHBMin_;
  double rmsHBMax_;
  double rmsHEMin_;
  double rmsHEMax_;

  double calibratioHBMin_;
  double calibratioHEMin_;
  double calibratioHOMin_;
  double calibratioHFMin_;

  int nevent;
  int nnnnnn;
  int counter;
  TH1F* h_errorGeneral;
  TH1F* h_error1;
  TH1F* h_error2;
  TH1F* h_error3;
  TH1F* h_amplError;
  TH1F* h_amplFine;
  
  TH1F* h_errorGeneral_HB;
  TH1F* h_error1_HB;
  TH1F* h_error2_HB;
  TH1F* h_error3_HB;
  TH1F* h_error4_HB;
  TH1F* h_error5_HB;
  TH1F* h_error6_HB;
  TH1F* h_error7_HB;
  TH1F* h_amplError_HB;
  TH1F* h_amplFine_HB;
  TH2F* h_mapDepth1Error_HB;
  TH2F* h_mapDepth2Error_HB;
  TH2F* h_mapDepth3Error_HB;
  TH1F* h_fiber0_HB;
  TH1F* h_fiber1_HB;
  TH1F* h_fiber2_HB;
  TH1F* h_repetedcapid_HB;

  TH1F* h_errorGeneral_HE;
  TH1F* h_error1_HE;
  TH1F* h_error2_HE;
  TH1F* h_error3_HE;
  TH1F* h_error4_HE;
  TH1F* h_error5_HE;
  TH1F* h_error6_HE;
  TH1F* h_error7_HE;
  TH1F* h_amplError_HE;
  TH1F* h_amplFine_HE;
  TH2F* h_mapDepth1Error_HE;
  TH2F* h_mapDepth2Error_HE;
  TH2F* h_mapDepth3Error_HE;
  TH1F* h_fiber0_HE;
  TH1F* h_fiber1_HE;
  TH1F* h_fiber2_HE;
  TH1F* h_repetedcapid_HE;

  TH1F* h_Amplitude_HB;
  TH2F* h_mapDepth1Amplitude_HB;
  TH2F* h_mapDepth2Amplitude_HB;
  TH2F* h_mapDepth3Amplitude_HB;
  TH2F* h_mapDepth1Amplitude225_HB;
  TH2F* h_mapDepth2Amplitude225_HB;
  TH2F* h_mapDepth3Amplitude225_HB;
  TH1F* h_Ampl_HB;
  TH2F* h_mapDepth1Ampl047_HB;
  TH2F* h_mapDepth2Ampl047_HB;
  TH2F* h_mapDepth3Ampl047_HB;
  TH2F* h_mapDepth1Ampl_HB;
  TH2F* h_mapDepth2Ampl_HB;
  TH2F* h_mapDepth3Ampl_HB;
  TH2F* h_mapDepth1AmplE34_HB;
  TH2F* h_mapDepth2AmplE34_HB;
  TH2F* h_mapDepth3AmplE34_HB;
  TH2F* h_mapDepth1_HB;
  TH2F* h_mapDepth2_HB;
  TH2F* h_mapDepth3_HB;

  TH1F* h_nbadchannels_depth1_HB;
  TH1F* h_runnbadchannels_depth1_HB;
  TH1F* h_badrate_depth1_HB;
  TH1F* h_runbadrate_depth1_HB;
  TH1F* h_runbadrate0_depth1_HB;

  TH1F* h_nbadchannels_depth2_HB;
  TH1F* h_runnbadchannels_depth2_HB;
  TH1F* h_badrate_depth2_HB;
  TH1F* h_runbadrate_depth2_HB;
  TH1F* h_runbadrate0_depth2_HB;

  TH1F* h_nbadchannels_depth1_HE;
  TH1F* h_runnbadchannels_depth1_HE;
  TH1F* h_badrate_depth1_HE;
  TH1F* h_runbadrate_depth1_HE;
  TH1F* h_runbadrate0_depth1_HE;

  TH1F* h_nbadchannels_depth2_HE;
  TH1F* h_runnbadchannels_depth2_HE;
  TH1F* h_badrate_depth2_HE;
  TH1F* h_runbadrate_depth2_HE;
  TH1F* h_runbadrate0_depth2_HE;

  TH1F* h_nbadchannels_depth3_HE;
  TH1F* h_runnbadchannels_depth3_HE;
  TH1F* h_badrate_depth3_HE;
  TH1F* h_runbadrate_depth3_HE;
  TH1F* h_runbadrate0_depth3_HE;

  TH1F* h_Amplitude_HE;
  TH2F* h_mapDepth1Amplitude_HE;
  TH2F* h_mapDepth2Amplitude_HE;
  TH2F* h_mapDepth3Amplitude_HE;
  TH2F* h_mapDepth1Amplitude225_HE;
  TH2F* h_mapDepth2Amplitude225_HE;
  TH2F* h_mapDepth3Amplitude225_HE;
  TH1F* h_Ampl_HE;
  TH2F* h_mapDepth1Ampl047_HE;
  TH2F* h_mapDepth2Ampl047_HE;
  TH2F* h_mapDepth3Ampl047_HE;
  TH2F* h_mapDepth1Ampl_HE;
  TH2F* h_mapDepth2Ampl_HE;
  TH2F* h_mapDepth3Ampl_HE;
  TH2F* h_mapDepth1AmplE34_HE;
  TH2F* h_mapDepth2AmplE34_HE;
  TH2F* h_mapDepth3AmplE34_HE;
  TH2F* h_mapDepth1_HE;
  TH2F* h_mapDepth2_HE;
  TH2F* h_mapDepth3_HE;

  TH1F* h_GetRMSOverNormalizedSignal_HB;
  TH1F* h_GetRMSOverNormalizedSignal_HE;
  TH1F* h_GetRMSOverNormalizedSignal_HO;
  TH1F* h_GetRMSOverNormalizedSignal_HF;
  TH2F* h_NormalizedSignal3D_HB;
  TH2F* h_NormalizedSignal3D0_HB;
  TH2F* h_NormalizedSignal3D_HE;
  TH2F* h_NormalizedSignal3D0_HE;
  TH2F* h_NormalizedSignal3D_HO;
  TH2F* h_NormalizedSignal3D0_HO;
  TH2F* h_NormalizedSignal3D_HF;
  TH2F* h_NormalizedSignal3D0_HF;

  TH1F* h_GetRMSOverNormalizedSignal3_HB;
  TH1F* h_GetRMSOverNormalizedSignal3_HE;
  TH1F* h_GetRMSOverNormalizedSignal3_HO;
  TH1F* h_GetRMSOverNormalizedSignal3_HF;

  TH1F* h_RatioCalib_HB;
  TH2F* h_mapRatioCalib047_HB;
  TH2F* h_mapRatioCalib_HB;
  TH2F* h_map_HB;
  TH1F* h_RatioCalib_HE;
  TH2F* h_mapRatioCalib047_HE;
  TH2F* h_mapRatioCalib_HE;
  TH2F* h_map_HE;
  TH1F* h_RatioCalib_HO;
  TH2F* h_mapRatioCalib047_HO;
  TH2F* h_mapRatioCalib_HO;
  TH2F* h_map_HO;
  TH1F* h_RatioCalib_HF;
  TH2F* h_mapRatioCalib047_HF;
  TH2F* h_mapRatioCalib_HF;
  TH2F* h_map_HF;

  // for ROOT
  // Readouts
  
  float hb15[2][2][72][10];
  float hb16[2][2][72][10];
  float he16[2][1][72][10];
  float hb[29][1][72][10];
  float he[26][2][72][10]; 
  
  // Calibration channels (corresponded to 1 RBX,counted 1-35).
  
  float hb_calibration0[40][15];
  float hb_calibration1[40][15];
  float he_calibration0[40][15];
  float he_calibration1[40][15];
  float hf_calibration0[40][15];
  float hf_calibration1[40][15];
  float ho_calibration0[40][15];
  float ho_calibration1[40][15];
  
  int myCalEta[5][40];
  int myCalPhi[5][40];
  
  double calib0[4][82][72];
  double signal[4][82][72];
  double calib3[4][82][72];
  double signal3[4][82][72];
  double calib2[4][82][72];
  int badchannels[4][4][82][72];
  int badchannels0[4][4][82][72];


  int Nevent;
  int Run;
  int run0 ;
  int runcounter ;
  
  TTree*    myTree;
  TFile*    hOutputFile;
  ///////////////////////////////////////// 
  // Get RBX number from 1-35 for Calibration box
  int getRBX(int& i, int& j, int& k);
  void fillDigiErrors(HBHEDigiCollection::const_iterator& digiItr);
  void fillDigiAmplitude(HBHEDigiCollection::const_iterator& digiItr);

  int local_event;
  int eta,phi,depth,nTS,cap_num;
  int numOfTS;
  float TS_data[10];
  float TS_cal[10];
  int numOfLaserEv;
  
  int lumi;
};


VeRawAnalyzer::VeRawAnalyzer(const edm::ParameterSet& iConfig)
{
  verbosity   =  iConfig.getUntrackedParameter<int>("Verbosity");
  ratioHBMin_      = iConfig.getParameter<double>("ratioHBMin");//
  ratioHBMax_      = iConfig.getParameter<double>("ratioHBMax");//
  ratioHEMin_      = iConfig.getParameter<double>("ratioHEMin");//
  ratioHEMax_      = iConfig.getParameter<double>("ratioHEMax");//
  rmsHBMin_      = iConfig.getParameter<double>("rmsHBMin");//
  rmsHBMax_      = iConfig.getParameter<double>("rmsHBMax");//
  rmsHEMin_      = iConfig.getParameter<double>("rmsHEMin");//
  rmsHEMax_      = iConfig.getParameter<double>("rmsHEMax");//
  calibratioHBMin_ = iConfig.getParameter<double>("calibratioHBMin");//
  calibratioHEMin_ = iConfig.getParameter<double>("calibratioHEMin");//
  calibratioHOMin_ = iConfig.getParameter<double>("calibratioHOMin");//
  calibratioHFMin_ = iConfig.getParameter<double>("calibratioHFMin");//

  //
  fOutputFileName = iConfig.getUntrackedParameter<std::string>("HistOutFile"); 
  // inputTag_ = iConfig.getUntrackedParameter<edm::InputTag>("DigiCollectionLabel");
  //  allowMissingInputs_=iConfig.getUntrackedParameter<bool>("AllowMissingInputs",false);
  
  
  //
  lumi=0;
  numOfLaserEv=0;
  local_event=0;
  numOfTS=10;
  run0=-1 ;
  runcounter=0 ;
}

VeRawAnalyzer::~VeRawAnalyzer()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ method called to for each event  ------------
void VeRawAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  if(verbosity > 0) std::cout<<" Start analyze "<<std::endl;    

  nevent++;
  Run=iEvent.id().run();
  Nevent=iEvent.id().event(); 
  lumi=iEvent.luminosityBlock();

  //////
  //  ++eventcounter;
  if(run0 != Run) {
    ++runcounter;
    cout  <<  " New Run =  "  << Run <<  " runcounter =  "  << runcounter <<  endl;
    run0 = Run;
    //    eventcounter = 0;
  }
  //////



  /*  

  bool LaserEvent, LaserRaddam;
  std::vector<edm::Handle<FEDRawDataCollection> >::const_iterator rawdata; 
  std::vector<edm::Handle<FEDRawDataCollection> > rawdata1;
  
  iEvent.getManyByType(rawdata1);
  
  if(rawdata1.size()==0) {
    cout<<" No Raw collection is found "<<endl;
  } else {
    int flag = -1;
    for(std::vector<edm::Handle<FEDRawDataCollection> >::const_iterator it = rawdata1.begin();it != rawdata1.end(); it++) {
      
      // std::cout<<" product name "<< (*it).provenance()->moduleName()<<
      //              " "<<(*it).provenance()->moduleLabel()<<
      //              std::endl;
      if((*it).provenance()->moduleLabel() == "hltHcalCalibrationRaw") {
	flag = 1;
	rawdata = it;
      }
    }//for
  }
*/
  /*
  // Abort Gap laser 
  LaserEvent=false;
  LaserRaddam=false;
  int hc_RADDAM=2, hc_HBHEHPD=3, hc_HOHPD=4, hc_HFPMT=5;
  edm::Handle<FEDRawDataCollection> frd; 
  iEvent.getByLabel("hltHcalCalibrationRaw",frd); 
  
  if(!frd.isValid()) {
  cout<<" No hltHcalCalibrationRaw collection is found "<<endl;
  } else {
  //iEvent.getByType(frd);
  //checking FEDs for calibration information       
  for (int i=FEDNumbering::MINHCALFEDID;i<=FEDNumbering::MAXHCALFEDID; i++) {
  const FEDRawData& fedData = frd->FEDData(i); 
  if ( fedData.size() < 24 ) continue ;         
  int value = ((const HcalDCCHeader*)(fedData.data()))->getCalibType() ;
  //cout<< value<<"\n";            
  if(value==hc_HBHEHPD || value==hc_HOHPD || value==hc_HFPMT) LaserEvent=true; 
  if(value==hc_RADDAM){
  LaserEvent=true; 
  LaserRaddam=true; 
  }
  } 
  if(LaserEvent==true)
  numOfLaserEv++; 
  }
  std::cout<<" Before GCT trigger "<<std::endl;
  */
 
  ///////////////////////////////////////////////////
  // get conditions

  iSetup.get<HcalDbRecord>().get(conditions);
  shape = conditions->getHcalShape (); //generic
    
  /////////////////////////////////////////////////// over DigiCollections:
  for(int k1 = 0; k1<4; k1++) {
    for(int k2 = 0; k2<82; k2++) {
      for(int k3 = 0; k3<72; k3++) {
	signal[k1][k2][k3] = 0.;
	calib0[k1][k2][k3] = 0.;
	signal3[k1][k2][k3] = 0.;
	calib3[k1][k2][k3] = 0.;
	calib2[k1][k2][k3] = 0.;
	for(int k0 = 0; k0<4; k0++) {
	  badchannels[k0][k1][k2][k3] = 0;
	  badchannels0[k0][k1][k2][k3] = 0;
	}  
      }  
    }  
  }  
  /////////////////////////////////////////////// HFDigiCollection
  edm::Handle<HFDigiCollection> hf;
  iEvent.getByType(hf);
  
  if(!hf.isValid()) {
    cout<<" No HFDigiCollection collection is found "<<endl;
  } else {
    for(HFDigiCollection::const_iterator digi=hf->begin();digi!=hf->end();digi++){
      eta=digi->id().ieta(); phi=digi->id().iphi(); depth=digi->id().depth(); nTS=digi->size();
      int iphi  = phi;
      int ieta  = eta;
      if(ieta > 0) ieta -= 1;
      if (verbosity == -13) std::cout<<"******************   HFDigiCollection nTS= "<<nTS<<std::endl; 
      if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	TS_data[i]=adc2fC[digi->sample(i).adc()];
	signal[3][ieta+41][iphi] += TS_data[i];
	if(i>1&&i<6) signal3[3][ieta+41][iphi] += TS_data[i];
	if (verbosity == -13) {
	  cout<< "HF phi= " << phi << " eta= " << eta << endl;
	  cout<< "HF iphi= " << iphi << " ieta= " << ieta << endl;
	  cout<< "HF TS0= " << adc2fC[digi->sample(0).adc()] 
	      << " TS1= " << adc2fC[digi->sample(1).adc()] 
	      << " TS2= " << adc2fC[digi->sample(2).adc()] 
	      << " TS3= " << adc2fC[digi->sample(3).adc()] 
	      << " TS4= " << adc2fC[digi->sample(4).adc()] 
	      << " TS5= " << adc2fC[digi->sample(5).adc()] << endl;
	}
      }
      if(eta == 29&&phi==1 && verbosity > 0) std::cout<<" 29 eta HF "<<depth<<std::endl;    
      /*
	for(int i=0; i<10; i++) TS_data[i]=0;
	for(int i=0;i<nTS;i++) TS_data[i]=adc2fC[digi->sample(i).adc()];
	//for(int i=nTS;i<numOfTS;i++) TS_data[i]=0;
	cap_num= digi->sample(0).capid();
	//if(Noise_data_status)
	{
	TS_shape(3);
	//Noise_data(3); 
	}  
      */
    }   
  }

  /////////////////////////////////////////////// HBHEDigiCollection
  edm::Handle<HBHEDigiCollection> hbhe; 
  iEvent.getByType(hbhe);
  if(!hbhe.isValid()) {
    cout<<" No HBHEDigiCollection collection is found "<<endl;
  } else {
    
    unsigned int N =  hbhe->size();
    if (verbosity == -22) std::cout << " HBHEDigiCollection size : " << N << std::endl;

    for(HBHEDigiCollection::const_iterator digi=hbhe->begin();digi!=hbhe->end();digi++)
      {
	eta=digi->id().ieta(); phi=digi->id().iphi(); depth=digi->id().depth(); nTS=digi->size();      

////////////////////////////////////////////////////////////  for zerrors.C script:
        fillDigiErrors(digi); 
////////////////////////////////////////////////////////////  for zratio34.C,zrms.C & zdifampl.C scripts:
        fillDigiAmplitude(digi); 
////////////////////////////////////////////////////////////////  for zRunRatio34.C script:
	//////////// k0(sub): =0 HB; =1 HE; =2 HO; =2 HF;
	//////////// k1(depth+1): = 1 - 4 ;
	for(int k0 = 0; k0<4; k0++) {
	  for(int k1 = 0; k1<4; k1++) {
	    //////////
	    int aaa = 0;	
	    int bbb = 0;	
	    int nbadchannels = 0;	
	    for(int k2 = 0; k2<82; k2++) {
	      for(int k3 = 0; k3<72; k3++) {
		aaa += badchannels[k0][k1][k2][k3];
		if(badchannels[k0][k1][k2][k3] !=0) {
		  bbb += badchannels0[k0][k1][k2][k3];
		  ++nbadchannels;
		}
	      }//k3
	    }//k2
	    double badrate = 0.;	
	    if(bbb != 0 ) badrate = aaa/bbb;
	    //////////
	    //HB
	    if(k0 == 0) {
	      if(k1 == 0) { 
		if(nbadchannels != 0 ) h_nbadchannels_depth1_HB->Fill( double(nbadchannels) );
		h_runnbadchannels_depth1_HB->Fill( double(runcounter) , double(nbadchannels) );
		if(badrate != 0 ) h_badrate_depth1_HB->Fill( badrate );
		h_runbadrate_depth1_HB->Fill( double(runcounter) , badrate );
		h_runbadrate0_depth1_HB->Fill( double(runcounter) , 1. );
	      }
	      if(k1 == 1) { 
		if(nbadchannels != 0 ) h_nbadchannels_depth2_HB->Fill( double(nbadchannels) );
		h_runnbadchannels_depth2_HB->Fill( double(runcounter) , double(nbadchannels) );
		if(badrate != 0 ) h_badrate_depth2_HB->Fill( badrate );
		h_runbadrate_depth2_HB->Fill( double(runcounter) , badrate );
		h_runbadrate0_depth2_HB->Fill( double(runcounter) , 1. );
	      }
	    }
	    //HE
	    if(k0 == 1) {
	      if(k1 == 0) { 
		if(nbadchannels != 0 ) h_nbadchannels_depth1_HE->Fill( double(nbadchannels) );
		h_runnbadchannels_depth1_HE->Fill( double(runcounter) , double(nbadchannels) );
		if(badrate != 0 ) h_badrate_depth1_HE->Fill( badrate );
		h_runbadrate_depth1_HE->Fill( double(runcounter) , badrate );
		h_runbadrate0_depth1_HE->Fill( double(runcounter) , 1. );
	      }
	      if(k1 == 1) { 
		if(nbadchannels != 0 ) h_nbadchannels_depth2_HE->Fill( double(nbadchannels) );
		h_runnbadchannels_depth2_HE->Fill( double(runcounter) , double(nbadchannels) );
		if(badrate != 0 ) h_badrate_depth2_HE->Fill( badrate );
		h_runbadrate_depth2_HE->Fill( double(runcounter) , badrate );
		h_runbadrate0_depth2_HE->Fill( double(runcounter) , 1. );
	      }
	      if(k1 == 2) { 
		if(nbadchannels != 0 ) h_nbadchannels_depth3_HE->Fill( double(nbadchannels) );
		h_runnbadchannels_depth3_HE->Fill( double(runcounter) , double(nbadchannels) );
		if(badrate != 0 ) h_badrate_depth3_HE->Fill( badrate );
		h_runbadrate_depth3_HE->Fill( double(runcounter) , badrate );
		h_runbadrate0_depth3_HE->Fill( double(runcounter) , 1. );
	      }
	    }////
	    //////////
	  }//k1
	}//k0
	////////////
	////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////  for Olga's script:
	int iphi  = phi;
	int ieta  = eta;
	if(ieta > 0) ieta -= 1;
	

	if(digi->id().subdet()==HcalBarrel) 
	  {                 
	    if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	      TS_data[i]=adc2fC[digi->sample(i).adc()];
	      signal[0][ieta+41][iphi] += TS_data[i];
	      if(i>1&&i<6) signal3[0][ieta+41][iphi] += TS_data[i];
	      if(abs(eta) == 15 || abs(eta) == 16) {
		if(eta == 15) hb15[1][depth-1][phi-1][i] = TS_data[i];
		if(eta == 16) hb16[1][depth-1][phi-1][i] = TS_data[i];
		if(eta == -15) hb15[0][depth-1][phi-1][i] = TS_data[i];
		if(eta == -16) hb16[0][depth-1][phi-1][i] = TS_data[i];
	      } else {
		hb[eta+14][0][phi-1][i] = TS_data[i];
	      }
	    }
	    
	    if(eta == 16&&phi==1 && verbosity > 0 ) std::cout<<" 16 eta "<<depth<<" "<<digi->id().subdet()<<std::endl;
	    if(eta == 15&&phi==1 && verbosity > 0 ) std::cout<<" 15 eta "<<depth<<" "<<digi->id().subdet()<<std::endl;
	  } 
	
	if(digi->id().subdet()==HcalEndcap)
	  {  
	    if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	      TS_data[i]=adc2fC[digi->sample(i).adc()];
	      signal[1][ieta+41][iphi] += TS_data[i];
	      if(i>1&&i<6) signal3[1][ieta+41][iphi] += TS_data[i];
	      if( abs(eta) == 16 ) {
		if(eta == 16) he16[1][0][phi-1][i] = TS_data[i];
		if(eta == -16) he16[0][0][phi-1][i] = TS_data[i];
	      } else {
		int jeta = abs(eta);
		if(eta>0) jeta = eta-4;
		if(eta<0) jeta = eta+29;
		if (verbosity == -22) std::cout<<" HE "<<eta<<" "<<jeta<<std::endl;
                he[jeta][depth-1][phi-1][i] = TS_data[i];
	      }
	    } 
	    if(eta == 29&&phi==1 && verbosity > 0 ) std::cout<<" 29 eta HE "<<depth<<std::endl;
	    if(eta == 16&&phi==1 && verbosity > 0 ) std::cout<<" 16 eta "<<depth<<" "<<digi->id().subdet()<<std::endl;
	  }// HcalEndcap                                                                    
	
      }// for
  }//hbhe.isValid
  
  ///////////////////////////////////////////////////   HODigiCollection
  edm::Handle<HODigiCollection> ho; 
  iEvent.getByType(ho);
  
  if(!ho.isValid()) {
    cout<<" No HO collection is found "<<endl;
  } else {
    
    for(HODigiCollection::const_iterator digi=ho->begin();digi!=ho->end();digi++){
      eta=digi->id().ieta(); phi=digi->id().iphi(); depth=digi->id().depth(); nTS=digi->size();{  
	int iphi  = phi;
	int ieta  = eta;
	if(ieta > 0) ieta -= 1;
	if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	  TS_data[i]=adc2fC[digi->sample(i).adc()];
	  signal[2][ieta+41][iphi] += TS_data[i];
	  if(i>1&&i<6) signal3[2][ieta+41][iphi] += TS_data[i];
	}
      }                
    }   
  }        
  
  
  /////////////////////////////////////////////////// HcalCalibDigiCollection
  for(int k1 = 0; k1<40; k1++) {
    for(int k2 = 0; k2<5; k2++ ) {
      myCalEta[k2][k1] = 0.;
      myCalPhi[k2][k1] = 0.;
    }
    for(int k2 = 0; k2<15; k2++) {  
      
      hb_calibration0[k1][k2] = 0.;
      hb_calibration1[k1][k2] = 0.;    
      
      he_calibration0[k1][k2] = 0.;
      he_calibration1[k1][k2] = 0.;
      
      ho_calibration0[k1][k2] = 0.;
      ho_calibration1[k1][k2] = 0.;
      
      hf_calibration0[k1][k2] = 0.;
      hf_calibration1[k1][k2] = 0.;
      
    } // k2
  } // k1 
  edm::Handle<HcalCalibDigiCollection> calib;
  iEvent.getByType(calib);
  
  if(!calib.isValid()) {
    cout<<" No HcalCalibDigiCollection collection is found "<<endl;
  } else {
    //          std::cout<<" Collection HcalCalibDigiCollection "<<std::endl;
    for(HcalCalibDigiCollection::const_iterator digi=calib->begin();digi!=calib->end();digi++){
      int cal_det=digi->id().hcalSubdet(); // 1-HB,2-HE,3-HO,4-HF
      int cal_phi=digi->id().iphi();
      int cal_eta=digi->id().ieta();
      int cal_cbox=digi->id().cboxChannel();
      /////////////////////////////////////////////     
      if(cal_det>0 && cal_det<5 && cal_cbox == 0 ) {
	int iphi  = cal_phi-1;
	int ieta  = cal_eta;
	if(ieta > 0) ieta -= 1;
	nTS=digi->size();
	if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	  calib0[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	  // TS:3-6
//	  if(i>1&&i<6) calib3[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	  // TS:2-6
	  if(i>0&&i<6) calib3[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
//
	  // TS:3-4   ,   cal_det 1-HB,2-HE
//	  if(i>1&&i<4 && cal_det<3) calib2[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	  // TS:2-6   ,   cal_det 1-HB,2-HE
	  if(i>0&&i<6 && cal_det<3) calib2[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
//
	  // TS:3-6   ,   cal_det 3-HO,4-HF
//	  if(i>1&&i<6 && cal_det>2) calib2[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	  // TS:2-6   ,   cal_det 3-HO
	  if(i>0&&i<6 && cal_det==3) calib2[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	  // TS:1-6   ,   cal_det 4-HF
	  if(i> -1 &&i<6 && cal_det==4) calib2[cal_det-1][ieta+41][iphi] += adc2fC[digi->sample(i).adc()&0xff];
	}
//
	if (verbosity == -11) {
	  cout<<"*******  det= " << cal_det << " cbox = " << cal_cbox << endl;
	  cout<< " phi= " << cal_phi << " eta= " << cal_eta << endl;
	  cout<< " iphi= " << iphi << " ieta= " << ieta << endl;
	  cout<< " TS0= " << adc2fC[digi->sample(0).adc()&0xff] 
	      << " TS1= " << adc2fC[digi->sample(1).adc()&0xff] 
	      << " TS2= " << adc2fC[digi->sample(2).adc()&0xff] 
	      << " TS3= " << adc2fC[digi->sample(3).adc()&0xff] 
	      << " TS4= " << adc2fC[digi->sample(4).adc()&0xff] 
	      << " TS5= " << adc2fC[digi->sample(5).adc()&0xff] << endl;
	}
      }// if(cal_det>0 && cal_det<5
      /////////////////////////////////////////////     
      if (cal_cbox<2){
	nTS=digi->size();
	if(nTS<=numOfTS) for(int i=0;i<nTS;i++) {
	  TS_cal[i]=adc2fC[digi->sample(i).adc()&0xff];   
	  int cal_RBX = getRBX(cal_det,cal_eta,cal_phi);
	  if( cal_det == 0 ) continue;   // HcalEmpty - what it is? skip
	  
	  myCalEta[cal_det][cal_RBX] = cal_eta;
	  myCalPhi[cal_det][cal_RBX] = cal_phi;
	  
	  if(cal_det == 1 ) {  
	    if(cal_cbox == 0)  hb_calibration0[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	    if(cal_cbox == 1)  hb_calibration1[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	  }
	  if(cal_det == 2 ) {
	    if(cal_cbox == 0)  he_calibration0[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	    if(cal_cbox == 1)  he_calibration1[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	  }
	  if(cal_det == 3 ) {
	    if(cal_cbox == 0) ho_calibration0[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	    if(cal_cbox == 1) ho_calibration1[cal_RBX][i] = adc2fC[digi->sample(i).adc()&0xff];
	  }
	}
      }
      
      if(local_event==5){
	if (verbosity == -22) {
	cout<<cal_det<<" "<<cal_cbox<<" "<<cal_phi<<" "<<cal_eta<<" "<<digi->id().cboxChannelString();
	for(int i=0; i<numOfTS; i++) cout<<" "<<TS_cal[i]<<" ";
	cout<<endl;
	}
      } 
    }
  }
////////////////////////////////////////////////////////////////  for zcalib.C and zgain.C scripts:
  for(int k1 = 0; k1<4; k1++) {
    for(int k2 = 0; k2<82; k2++) {
      for(int k3 = 0; k3<72; k3++) {

//if(k1==3) cout<<"signal[k1][k2][k3]= "<<signal[k1][k2][k3]<<" calib0[k1][k2][k3]= "<<calib0[k1][k2][k3]<<endl;

	// return to real indexes in eta and phi ( k20 and k30)
	int k20=k2-41;
	if(k2>0 || k2==0) k20=k2+1;
	int k30=k3+1; 

	// find calibration indexes in eta and phi ( kk2 and kk3) 
	int kk2=0, kk3=0;
	if(k1==0 || k1==1) {
	  if(k20>0 ) kk2=1; else kk2=-1;
	  if(k30==71 ||k30==72 || k30==1 || k30==2) kk3=71; else kk3=((k30-3)/4)*4+3;
	}
	else if(k1==2){
	  if(abs(k20)<=4){
	    kk2=0;
	    if(k30==71||k30==72||k30==1||k30==2||k30==3||k30==4) kk3=71; else kk3=((k30-5)/6)*6+5;
	  }
	  else{
	    if(abs(k20)>4  && abs(k20)<=10)  kk2=1;
	    if(abs(k20)>10 && abs(k20)<=15)  kk2=2;
	    if(k20<0) kk2=-kk2;
	    if(k30==71 ||k30==72 || (k30>=1 && k30<=10)) kk3=71; else kk3=((k30-11)/12)*12+11;
	  }
	}
	else if(k1==3){
	  if(k20>0) kk2=1; else kk2=-1;
	  if(k30>=1  && k30<=18) kk3=1;
	  if(k30>=19 && k30<=36) kk3=19;
	  if(k30>=37 && k30<=54) kk3=37;
	  if(k30>=55 && k30<=72) kk3=55;
	}
	// return to indexes in massiv
	int kkk2  = kk2+41;
	if(kk2 > 0) kkk2 -= 1;  
	int kkk3  = kk3;
	kkk3 -=1;

////////////////////////////////////////////////////////////////  for zgain.C script:
	int k2plot = k2-41;
	double GetRMSOverNormalizedSignal =-1.;
	if(signal[k1][k2][k3]>0.&& calib0[k1][kkk2][kkk3]>0.) {
	  /////////// GetRMSOverNormalizedSignal 
	  GetRMSOverNormalizedSignal = signal[k1][k2][k3]/calib0[k1][kkk2][kkk3];
	  if(k1==0) {
	    h_GetRMSOverNormalizedSignal_HB->Fill(GetRMSOverNormalizedSignal,1.);
	    h_NormalizedSignal3D_HB->Fill(double(k2plot), double(k3), signal[k1][k2][k3]);
	    h_NormalizedSignal3D0_HB->Fill(double(k2plot), double(k3), 1.);    
	  }
	  if(k1==1) {
	    h_GetRMSOverNormalizedSignal_HE->Fill(GetRMSOverNormalizedSignal,1.);
	    h_NormalizedSignal3D_HE->Fill(double(k2plot), double(k3), signal[k1][k2][k3]);
	    h_NormalizedSignal3D0_HE->Fill(double(k2plot), double(k3), 1.);    
	  }
	  if(k1==2) {
	    h_GetRMSOverNormalizedSignal_HO->Fill(GetRMSOverNormalizedSignal,1.);
	    h_NormalizedSignal3D_HO->Fill(double(k2plot), double(k3), signal[k1][k2][k3]);
	    h_NormalizedSignal3D0_HO->Fill(double(k2plot), double(k3), 1.);    
	  }
	  if(k1==3) {
	    h_GetRMSOverNormalizedSignal_HF->Fill(GetRMSOverNormalizedSignal,1.);
	    h_NormalizedSignal3D_HF->Fill(double(k2plot), double(k3), signal[k1][k2][k3]);
	    h_NormalizedSignal3D0_HF->Fill(double(k2plot), double(k3), 1.);    
	  }
	} 
	GetRMSOverNormalizedSignal =-1.;
	if(signal3[k1][k2][k3]>0.&& calib3[k1][kkk2][kkk3]>0.) {
	  GetRMSOverNormalizedSignal = signal3[k1][k2][k3]/calib3[k1][kkk2][kkk3];
	  if(k1==0) h_GetRMSOverNormalizedSignal3_HB->Fill(GetRMSOverNormalizedSignal,1.);    
	  if(k1==1) h_GetRMSOverNormalizedSignal3_HE->Fill(GetRMSOverNormalizedSignal,1.);    
	  if(k1==2) h_GetRMSOverNormalizedSignal3_HO->Fill(GetRMSOverNormalizedSignal,1.);    
	  if(k1==3) h_GetRMSOverNormalizedSignal3_HF->Fill(GetRMSOverNormalizedSignal,1.);  
	}
	////////// 
////////////////////////////////////////////////////////////////  for zcalib.C script:

	double ratio =0.;
	if(calib0[k1][kkk2][kkk3]>0.) ratio = calib2[k1][kkk2][kkk3]/calib0[k1][kkk2][kkk3];
	if(k1==0) {
	  h_RatioCalib_HB->Fill(ratio,1.);
	  if(ratio < calibratioHBMin_ ) h_mapRatioCalib047_HB->Fill(double(k2plot), double(k3), 1.);
	  h_mapRatioCalib_HB->Fill(double(k2plot), double(k3), ratio);    
	  h_map_HB->Fill(double(k2plot), double(k3), 1.);    
	}
	if(k1==1) {
	  h_RatioCalib_HE->Fill(ratio,1.);
	  if(ratio < calibratioHEMin_ ) h_mapRatioCalib047_HE->Fill(double(k2plot), double(k3), 1.);
	  h_mapRatioCalib_HE->Fill(double(k2plot), double(k3), ratio);    
	  h_map_HE->Fill(double(k2plot), double(k3), 1.);    
	}
	if(k1==2) {
	  h_RatioCalib_HO->Fill(ratio,1.);
	  if(ratio < calibratioHOMin_ ) h_mapRatioCalib047_HO->Fill(double(k2plot), double(k3), 1.);
	  h_mapRatioCalib_HO->Fill(double(k2plot), double(k3), ratio);    
	  h_map_HO->Fill(double(k2plot), double(k3), 1.);    
	}
	if(k1==3) {
	  h_RatioCalib_HF->Fill(ratio,1.);
	  if(ratio < calibratioHFMin_ ) h_mapRatioCalib047_HF->Fill(double(k2plot), double(k3), 1.);
	  h_mapRatioCalib_HF->Fill(double(k2plot), double(k3), ratio);    
	  h_map_HF->Fill(double(k2plot), double(k3), 1.);    
	}
	////////// 
      } // k3 
    }  // k2
  }  // k1
  /////////////////////////////////////////////////// 
  myTree->Fill();
  
  if(++local_event%100==0)
    { 
      if (verbosity == -22) cout<<"run "<<Run<<" processing events "<<local_event<<" ok, "
				<<", lumi "<<lumi
				<<", numOfLaserEv "<<numOfLaserEv<<endl;          
    }                          
  /////////////////////////////////////////////////// 
}
// ------------ method called once each job just before starting event loop  -----------
void VeRawAnalyzer::beginJob()
{
  hOutputFile   = new TFile( fOutputFileName.c_str(), "RECREATE" ) ;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  nnnnnn = 0;
  nevent = 0; 
  counter = 0;
  //////////////////////////////////////////////////////////////////////////////////    book histoes
//  ha2 = new TH2F("ha2"," ", 82, -41., 41., 72, 0., 72.);

  h_errorGeneral = new TH1F("h_errorGeneral"," ", 5, 0., 5.);
  h_error1 = new TH1F("h_error1"," ", 5, 0., 5.);
  h_error2 = new TH1F("h_error2"," ", 5, 0., 5.);
  h_error3 = new TH1F("h_error3"," ", 5, 0., 5.);
  h_amplError = new TH1F("h_amplError"," ", 100, -2., 8.);
  h_amplFine = new TH1F("h_amplFine"," ", 100, -2., 8.);
  
  h_errorGeneral_HB = new TH1F("h_errorGeneral_HB"," ", 5, 0., 5.);
  h_error1_HB = new TH1F("h_error1_HB"," ", 5, 0., 5.);
  h_error2_HB = new TH1F("h_error2_HB"," ", 5, 0., 5.);
  h_error3_HB = new TH1F("h_error3_HB"," ", 5, 0., 5.);
  h_error4_HB = new TH1F("h_error4_HB"," ", 5, 0., 5.);
  h_error5_HB = new TH1F("h_error5_HB"," ", 5, 0., 5.);
  h_error6_HB = new TH1F("h_error6_HB"," ", 5, 0., 5.);
  h_error7_HB = new TH1F("h_error7_HB"," ", 5, 0., 5.);
  h_amplError_HB = new TH1F("h_amplError_HB"," ", 100, -2., 8.);
  h_amplFine_HB = new TH1F("h_amplFine_HB"," ", 100, -2., 8.);
  h_mapDepth1Error_HB = new TH2F("h_mapDepth1Error_HB"," ", 82, -41., 41., 72, 0., 72.);
  h_mapDepth2Error_HB = new TH2F("h_mapDepth2Error_HB"," ", 82, -41., 41., 72, 0., 72.);
  h_mapDepth3Error_HB = new TH2F("h_mapDepth3Error_HB"," ", 82, -41., 41., 72, 0., 72.);
  h_fiber0_HB = new TH1F("h_fiber0_HB"," ", 10, 0., 10.);
  h_fiber1_HB = new TH1F("h_fiber1_HB"," ", 10, 0., 10.);
  h_fiber2_HB = new TH1F("h_fiber2_HB"," ", 40, 0., 40.);
  h_repetedcapid_HB = new TH1F("h_repetedcapid_HB"," ", 5, 0., 5.);
  
  h_errorGeneral_HE = new TH1F("h_errorGeneral_HE"," ", 5, 0., 5.);
  h_error1_HE = new TH1F("h_error1_HE"," ", 5, 0., 5.);
  h_error2_HE = new TH1F("h_error2_HE"," ", 5, 0., 5.);
  h_error3_HE = new TH1F("h_error3_HE"," ", 5, 0., 5.);
  h_error4_HE = new TH1F("h_error4_HE"," ", 5, 0., 5.);
  h_error5_HE = new TH1F("h_error5_HE"," ", 5, 0., 5.);
  h_error6_HE = new TH1F("h_error6_HE"," ", 5, 0., 5.);
  h_error7_HE = new TH1F("h_error7_HE"," ", 5, 0., 5.);
  h_amplError_HE = new TH1F("h_amplError_HE"," ", 100, -2., 8.);
  h_amplFine_HE = new TH1F("h_amplFine_HE"," ", 100, -2., 8.);
  h_mapDepth1Error_HE = new TH2F("h_mapDepth1Error_HE"," ", 82, -41., 41., 72, 0., 72.);
  h_mapDepth2Error_HE = new TH2F("h_mapDepth2Error_HE"," ", 82, -41., 41., 72, 0., 72.);
  h_mapDepth3Error_HE = new TH2F("h_mapDepth3Error_HE"," ", 82, -41., 41., 72, 0., 72.);
  h_fiber0_HE = new TH1F("h_fiber0_HE"," ", 10, 0., 10.);
  h_fiber1_HE = new TH1F("h_fiber1_HE"," ", 10, 0., 10.);
  h_fiber2_HE = new TH1F("h_fiber2_HE"," ", 40, 0., 40.);
  h_repetedcapid_HE = new TH1F("h_repetedcapid_HE"," ", 5, 0., 5.);




    ////////////////////////////////////////////////////////////////////////////////////////////////////
    h_Amplitude_HB = new TH1F("h_Amplitude_HB"," ", 100, 1.,4.);
    h_mapDepth1Amplitude225_HB = new TH2F("h_mapDepth1Amplitude225_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Amplitude225_HB = new TH2F("h_mapDepth2Amplitude225_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Amplitude225_HB = new TH2F("h_mapDepth3Amplitude225_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1Amplitude_HB = new TH2F("h_mapDepth1Amplitude_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Amplitude_HB = new TH2F("h_mapDepth2Amplitude_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Amplitude_HB = new TH2F("h_mapDepth3Amplitude_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_Ampl_HB = new TH1F("h_Ampl_HB"," ", 100, 0.,1.);
    h_mapDepth1Ampl047_HB = new TH2F("h_mapDepth1Ampl047_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Ampl047_HB = new TH2F("h_mapDepth2Ampl047_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Ampl047_HB = new TH2F("h_mapDepth3Ampl047_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1Ampl_HB = new TH2F("h_mapDepth1Ampl_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Ampl_HB = new TH2F("h_mapDepth2Ampl_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Ampl_HB = new TH2F("h_mapDepth3Ampl_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1AmplE34_HB = new TH2F("h_mapDepth1AmplE34_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2AmplE34_HB = new TH2F("h_mapDepth2AmplE34_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3AmplE34_HB = new TH2F("h_mapDepth3AmplE34_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1_HB = new TH2F("h_mapDepth1_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2_HB = new TH2F("h_mapDepth2_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3_HB = new TH2F("h_mapDepth3_HB"," ", 82, -41., 41., 72, 0., 72.);
    
    h_Amplitude_HE = new TH1F("h_Amplitude_HE"," ", 100, 1.,4.);
    h_mapDepth1Amplitude225_HE = new TH2F("h_mapDepth1Amplitude225_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Amplitude225_HE = new TH2F("h_mapDepth2Amplitude225_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Amplitude225_HE = new TH2F("h_mapDepth3Amplitude225_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1Amplitude_HE = new TH2F("h_mapDepth1Amplitude_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Amplitude_HE = new TH2F("h_mapDepth2Amplitude_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Amplitude_HE = new TH2F("h_mapDepth3Amplitude_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_Ampl_HE = new TH1F("h_Ampl_HE"," ", 100, 0.,1.);
    h_mapDepth1Ampl047_HE = new TH2F("h_mapDepth1Ampl047_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Ampl047_HE = new TH2F("h_mapDepth2Ampl047_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Ampl047_HE = new TH2F("h_mapDepth3Ampl047_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1Ampl_HE = new TH2F("h_mapDepth1Ampl_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2Ampl_HE = new TH2F("h_mapDepth2Ampl_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3Ampl_HE = new TH2F("h_mapDepth3Ampl_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1AmplE34_HE = new TH2F("h_mapDepth1AmplE34_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2AmplE34_HE = new TH2F("h_mapDepth2AmplE34_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3AmplE34_HE = new TH2F("h_mapDepth3AmplE34_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth1_HE = new TH2F("h_mapDepth1_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth2_HE = new TH2F("h_mapDepth2_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapDepth3_HE = new TH2F("h_mapDepth3_HE"," ", 82, -41., 41., 72, 0., 72.);

    int bac= 50;
    float bac2=51.;
    h_nbadchannels_depth1_HB = new TH1F("h_nbadchannels_depth1_HB"," ",   100, 1.,-1.);
    h_runnbadchannels_depth1_HB = new TH1F("h_runnbadchannels_depth1_HB"," ",   bac, 1.,bac2);
    h_badrate_depth1_HB = new TH1F("h_badrate_depth1_HB"," ",   100, 0.,1.);
    h_runbadrate_depth1_HB = new TH1F("h_runbadrate_depth1_HB"," ",             bac, 1.,bac2);
    h_runbadrate0_depth1_HB = new TH1F("h_runbadrate0_depth1_HB"," ",           bac, 1.,bac2);

    h_nbadchannels_depth2_HB = new TH1F("h_nbadchannels_depth2_HB"," ",   100, 1.,-1.);
    h_runnbadchannels_depth2_HB = new TH1F("h_runnbadchannels_depth2_HB"," ",   bac, 1.,bac2);
    h_badrate_depth2_HB = new TH1F("h_badrate_depth2_HB"," ",   100, 0.,1.);
    h_runbadrate_depth2_HB = new TH1F("h_runbadrate_depth2_HB"," ",             bac, 1.,bac2);
    h_runbadrate0_depth2_HB = new TH1F("h_runbadrate0_depth2_HB"," ",           bac, 1.,bac2);

    h_nbadchannels_depth1_HE = new TH1F("h_nbadchannels_depth1_HE"," ",   100, 1.,-1.);
    h_runnbadchannels_depth1_HE = new TH1F("h_runnbadchannels_depth1_HE"," ",   bac, 1.,bac2);
    h_badrate_depth1_HE = new TH1F("h_badrate_depth1_HE"," ",   100, 0.,1.);
    h_runbadrate_depth1_HE = new TH1F("h_runbadrate_depth1_HE"," ",             bac, 1.,bac2);
    h_runbadrate0_depth1_HE = new TH1F("h_runbadrate0_depth1_HE"," ",           bac, 1.,bac2);

    h_nbadchannels_depth2_HE = new TH1F("h_nbadchannels_depth2_HE"," ",   100, 1.,-1.);
    h_runnbadchannels_depth2_HE = new TH1F("h_runnbadchannels_depth2_HE"," ",   bac, 1.,bac2);
    h_badrate_depth2_HE = new TH1F("h_badrate_depth2_HE"," ",   100, 0.,1.);
    h_runbadrate_depth2_HE = new TH1F("h_runbadrate_depth2_HE"," ",             bac, 1.,bac2);
    h_runbadrate0_depth2_HE = new TH1F("h_runbadrate0_depth2_HE"," ",           bac, 1.,bac2);

    h_nbadchannels_depth3_HE = new TH1F("h_nbadchannels_depth3_HE"," ",   100, 1.,-1.);
    h_runnbadchannels_depth3_HE = new TH1F("h_runnbadchannels_depth3_HE"," ",   bac, 1.,bac2);
    h_badrate_depth3_HE = new TH1F("h_badrate_depth3_HE"," ",   100, 0.,1.);
    h_runbadrate_depth3_HE = new TH1F("h_runbadrate_depth3_HE"," ",             bac, 1.,bac2);
    h_runbadrate0_depth3_HE = new TH1F("h_runbadrate0_depth3_HE"," ",           bac, 1.,bac2);
  ////////////////////////////////////////////////////////////////////////////////////////////////////
    float m7=2;
    float m8=3;
    float m9=5;
    h_GetRMSOverNormalizedSignal_HB = new TH1F("h_GetRMSOverNormalizedSignal_HB"," ",   100, 0.,m7);
    h_GetRMSOverNormalizedSignal_HE = new TH1F("h_GetRMSOverNormalizedSignal_HE"," ",   100, 0.,m7);
    h_GetRMSOverNormalizedSignal_HO = new TH1F("h_GetRMSOverNormalizedSignal_HO"," ",   100, 0.,m8);
    h_GetRMSOverNormalizedSignal_HF = new TH1F("h_GetRMSOverNormalizedSignal_HF"," ",   100, 0.,m9);
    h_NormalizedSignal3D_HB = new TH2F("h_NormalizedSignal3D_HB"," ",   82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D0_HB = new TH2F("h_NormalizedSignal3D0_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D_HE = new TH2F("h_NormalizedSignal3D_HE"," ",   82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D0_HE = new TH2F("h_NormalizedSignal3D0_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D_HO = new TH2F("h_NormalizedSignal3D_HO"," ",   82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D0_HO = new TH2F("h_NormalizedSignal3D0_HO"," ", 82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D_HF = new TH2F("h_NormalizedSignal3D_HF"," ",   82, -41., 41., 72, 0., 72.);
    h_NormalizedSignal3D0_HF = new TH2F("h_NormalizedSignal3D0_HF"," ", 82, -41., 41., 72, 0., 72.);
    h_GetRMSOverNormalizedSignal3_HB = new TH1F("h_GetRMSOverNormalizedSignal3_HB"," ", 100, 0.,m7);
    h_GetRMSOverNormalizedSignal3_HE = new TH1F("h_GetRMSOverNormalizedSignal3_HE"," ", 100, 0.,m7);
    h_GetRMSOverNormalizedSignal3_HO = new TH1F("h_GetRMSOverNormalizedSignal3_HO"," ", 100, 0.,m8);
    h_GetRMSOverNormalizedSignal3_HF = new TH1F("h_GetRMSOverNormalizedSignal3_HF"," ", 100, 0.,m9);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
    h_RatioCalib_HB       = new TH1F("h_RatioCalib_HB"," ",      100, 0.,1.);
    h_mapRatioCalib047_HB = new TH2F("h_mapRatioCalib047_HB"," ", 82, -41., 41., 72, 0., 72.);
    h_mapRatioCalib_HB    = new TH2F("h_mapRatioCalib_HB"," ",    82, -41., 41., 72, 0., 72.);
    h_map_HB              = new TH2F("h_map_HB"," ",              82, -41., 41., 72, 0., 72.);
    h_RatioCalib_HE       = new TH1F("h_RatioCalib_HE"," ",      100, 0.,1.);
    h_mapRatioCalib047_HE = new TH2F("h_mapRatioCalib047_HE"," ", 82, -41., 41., 72, 0., 72.);
    h_mapRatioCalib_HE    = new TH2F("h_mapRatioCalib_HE"," ",    82, -41., 41., 72, 0., 72.);
    h_map_HE              = new TH2F("h_map_HE"," ",              82, -41., 41., 72, 0., 72.);
    h_RatioCalib_HO       = new TH1F("h_RatioCalib_HO"," ",      100, 0.,1.);
    h_mapRatioCalib047_HO = new TH2F("h_mapRatioCalib047_HO"," ", 82, -41., 41., 72, 0., 72.);
    h_mapRatioCalib_HO    = new TH2F("h_mapRatioCalib_HO"," ",    82, -41., 41., 72, 0., 72.);
    h_map_HO              = new TH2F("h_map_HO"," ",              82, -41., 41., 72, 0., 72.);
    h_RatioCalib_HF       = new TH1F("h_RatioCalib_HF"," ",      100, 0.,1.);
    h_mapRatioCalib047_HF = new TH2F("h_mapRatioCalib047_HF"," ", 82, -41., 41., 72, 0., 72.);
    h_mapRatioCalib_HF    = new TH2F("h_mapRatioCalib_HF"," ",    82, -41., 41., 72, 0., 72.);
    h_map_HF              = new TH2F("h_map_HF"," ",              82, -41., 41., 72, 0., 72.);
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  myTree = new TTree("Hcal","Hcal Tree");
  myTree->Branch("Nevent",  &Nevent, "Nevent/I");
  myTree->Branch("Run",  &Run, "Run/I");
 
// Calibration box
 
  myTree->Branch("myCalEta", myCalEta, "myCalEta[5][40]/I");
  myTree->Branch("myCalPhi", myCalPhi, "myCalPhi[5][40]/I");
  
  myTree->Branch("hb_calibration0", hb_calibration0, "hb_calibration0[40][15]/F");
  myTree->Branch("hb_calibration1", hb_calibration1, "hb_calibration1[40][15]/F");
  
  myTree->Branch("he_calibration0", he_calibration0, "he_calibration0[40][15]/F");
  myTree->Branch("he_calibration1", he_calibration1, "he_calibration1[40][15]/F");
  
  myTree->Branch("hf_calibration0", hf_calibration0, "hf_calibration0[40][15]/F");
  myTree->Branch("hf_calibration1", hf_calibration1, "hf_calibration1[40][15]/F");
  
  myTree->Branch("ho_calibration0", ho_calibration0, "ho_calibration0[40][15]/F");
  myTree->Branch("ho_calibration1", ho_calibration1, "ho_calibration1[40][15]/F");

// HBHE readouts

  myTree->Branch("hb15", hb15, "hb15[2][2][72][10]/F");
  myTree->Branch("hb16", hb16, "hb16[2][2][72][10]/F");
  myTree->Branch("he16", he16, "he16[2][1][72][10]/F");
  myTree->Branch("hb", hb, "hb[29][1][72][10]/F");
  myTree->Branch("he", he, "he[26][2][72][10]/F");
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ method called for each event  ------------
void VeRawAnalyzer::fillDigiErrors(HBHEDigiCollection::const_iterator& digiItr)
{

    CaloSamples tool;  // TS
    
    nnnnnn++;    
    if (verbosity == -22) std::cout << "**************   in loop over Digis   counter =     " << nnnnnn << std::endl;
   
    HcalDetId cell(digiItr->id()); 
    int depth = cell.depth();
    int iphi  = cell.iphi()-1;
    int ieta  = cell.ieta();
    if(ieta > 0) ieta -= 1;
    int sub   = cell.subdet(); // 1-HB, 2-HE (HFDigiCollection: 4-HF)
    if (verbosity == -22) std::cout << std::endl << nnnnnn << " DIGI ->  "  
				 << "sub, ieta, iphi, depth = "  
				 <<  sub << ", " << ieta << ", " << iphi << ", " << depth 
				 << std::endl;
    
    // !!!!!!
    int errorGeneral =  0;
    int error1 =  0;
    int error2 =  0;
    int error3 =  0;
    int error4 =  0;
    int error5 =  0;
    int error6 =  0;
    int error7 =  0;
    // !!!!!!
    bool anycapid   =  true;
    bool anyer      =  false;
    bool anydv      =  true;
    // for help:
    int firstcapid  = 0;
    int sumcapid    = 0;
    int lastcapid=0, capid=0;
    int ERRORfiber = -10;
    int ERRORfiberChan = -10;
    int ERRORfiberAndChan = -10;
    int repetedcapid = 0;


    if (verbosity == -22) std::cout <<" Size of Digi "<<(*digiItr).size()<<std::endl;

    ///////////////////////////////////////    
    for  (int ii=0;ii<(*digiItr).size();ii++) {
      capid = (*digiItr)[ii].capid(); // capId (0-3, sequential)
      bool er    = (*digiItr)[ii].er();   // error
      bool dv    = (*digiItr)[ii].dv();  // valid data
      int fiber    = (*digiItr)[ii].fiber();  // get the fiber number
      int fiberChan    = (*digiItr)[ii].fiberChan();  // get the fiber channel number
      int fiberAndChan    = (*digiItr)[ii].fiberAndChan();  // get the id channel
      if (verbosity == -22) std::cout <<" fiber =  "  <<  fiber << std::endl;
      if (verbosity == -22) std::cout <<" fiberChan =  "  <<  fiberChan << std::endl;
      if (verbosity == -22) std::cout <<" fiberAndChan =  "  <<  fiberAndChan << std::endl;
      
      if(ii!=0 && ((lastcapid+1)%4)!=capid) { 
	anycapid =  false;    
	if (verbosity == -22) std::cout <<" capid=false  digiItr =    "  <<  *digiItr << std::endl;
	ERRORfiber = fiber;
	ERRORfiberChan = fiberChan;
	ERRORfiberAndChan = fiberAndChan;
	if( capid != lastcapid ){} else{ repetedcapid =  1;}
      }
      lastcapid=capid;  
   
      if(ii == 0) firstcapid = capid;
      sumcapid += capid;
      
      if (verbosity == -22) std::cout <<" capid =    "  <<  capid << std::endl;
      if(er) { 
	anyer =  true;    
	if (verbosity == -22) std::cout <<" er=true   digiItr =   "  <<  *digiItr << std::endl;
	ERRORfiber = fiber;
	ERRORfiberChan = fiberChan;
	ERRORfiberAndChan = fiberAndChan;
      }
      if(!dv) { 
	anydv =  false;    
	if (verbosity == -22) std::cout <<" dv=false  digiItr =    "  <<  *digiItr << std::endl;
	ERRORfiber = fiber;
	ERRORfiberChan = fiberChan;
	ERRORfiberAndChan = fiberAndChan;
      }
      
    }// for

    if (verbosity == -22) std::cout <<" Digi is treated "<<std::endl;

    ///////////////////////////////////////    
    if( firstcapid==0 && !anycapid) errorGeneral = 1; 
    if( firstcapid==1 && !anycapid) errorGeneral = 2; 
    if( firstcapid==2 && !anycapid) errorGeneral = 3; 
    if( firstcapid==3 && !anycapid) errorGeneral = 4; 
    if( !anycapid )                     error1 = 1; 
    if( anyer )                         error2 = 1; 
    if( !anydv )                        error3 = 1; 
    
    if( !anycapid && anyer)                     error4 = 1; 
    if( !anycapid && !anydv)                    error5 = 1; 
    if( !anycapid && anyer && !anydv)           error6 = 1; 
    if( anyer && !anydv)                        error7 = 1; 
    ///////////////////////////////////////Energy
    // Energy:    
//    int firstSample  = 1;  // RECO window parameters
//    int samplesToAdd = 9;
    int firstSample  = 2;  // RECO window parameters
    int samplesToAdd = 2;

    HcalCalibrations calib = conditions->getHcalCalibrations(cell);
    const HcalQIECoder* channelCoder = conditions->getHcalCoder(cell);
    HcalCoderDb coder (*channelCoder, *shape);
    coder.adc2fC(*digiItr,tool);
    if (verbosity == -22) std::cout << "coder done..." << std::endl;
    double ampl = 0.;
    for (int ii=0; ii<tool.size(); ii++) {  
      int capid = ((*digiItr)[ii]).capid();
      double ta = (tool[ii]-calib.pedestal(capid)); // pedestal subtraction
      ta*= calib.respcorrgain(capid) ;   // fC --> GeV
      if (ii >= firstSample && ii < firstSample+samplesToAdd )  ampl+=ta;  
    }
    if (verbosity == -22) std::cout << std::endl << "*** E = " << ampl 
				 << "   ACD -> fC -> (gain ="
				 << calib.respcorrgain(0) << ") GeV (ped.subtracted)" 
				 << std::endl;
    
    ///////////////////////////////////////Digis
    // Digis:
    // HB
    if ( sub == 1 ) {
      h_errorGeneral_HB->Fill(double(errorGeneral),1.);    
      h_error1_HB->Fill(double(error1),1.);    
      h_error2_HB->Fill(double(error2),1.);    
      h_error3_HB->Fill(double(error3),1.);    
      h_error4_HB->Fill(double(error4),1.);    
      h_error5_HB->Fill(double(error5),1.);    
      h_error6_HB->Fill(double(error6),1.);    
      h_error7_HB->Fill(double(error7),1.); 
      h_repetedcapid_HB->Fill(double(repetedcapid),1.); 
   
      if(error1 !=0 || error2 !=0 || error3 !=0 ) {
	h_amplError_HB->Fill(ampl,1.);
	if(depth==1) h_mapDepth1Error_HB->Fill(double(ieta), double(iphi));    
	if(depth==2) h_mapDepth2Error_HB->Fill(double(ieta), double(iphi));    
	if(depth==3) h_mapDepth3Error_HB->Fill(double(ieta), double(iphi));    
	h_fiber0_HB->Fill(double(ERRORfiber), 1. );    
	h_fiber1_HB->Fill(double(ERRORfiberChan), 1. );    
	h_fiber2_HB->Fill(double(ERRORfiberAndChan), 1. );    
      }
      else { h_amplFine_HB->Fill(ampl,1.);}
    }
    // HE
    if ( sub == 2 ) {
      h_errorGeneral_HE->Fill(double(errorGeneral),1.);    
      h_error1_HE->Fill(double(error1),1.);    
      h_error2_HE->Fill(double(error2),1.);    
      h_error3_HE->Fill(double(error3),1.);    
      h_error4_HE->Fill(double(error4),1.);    
      h_error5_HE->Fill(double(error5),1.);    
      h_error6_HE->Fill(double(error6),1.);    
      h_error7_HE->Fill(double(error7),1.);    
      h_repetedcapid_HE->Fill(double(repetedcapid),1.); 

      if(error1 !=0 || error2 !=0 || error3 !=0 ) {
	h_amplError_HE->Fill(ampl,1.);
	if(depth==1) h_mapDepth1Error_HE->Fill(double(ieta), double(iphi));    
	if(depth==2) h_mapDepth2Error_HE->Fill(double(ieta), double(iphi));    
	if(depth==3) h_mapDepth3Error_HE->Fill(double(ieta), double(iphi));    
	h_fiber0_HE->Fill(double(ERRORfiber), 1. );    
	h_fiber1_HE->Fill(double(ERRORfiberChan), 1. );    
	h_fiber2_HE->Fill(double(ERRORfiberAndChan), 1. );    
      }
      else { h_amplFine_HE->Fill(ampl,1.);}
    }
    //    ha2->Fill(double(ieta), double(iphi));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VeRawAnalyzer::fillDigiAmplitude(HBHEDigiCollection::const_iterator& digiItr)
{
    CaloSamples tool;  // TS
    
    HcalDetId cell(digiItr->id()); 
    int depth = cell.depth();
//    int iphi0  = cell.iphi();// 1-72
    int iphi  = cell.iphi()-1;// 0-71
//    int ieta0  = cell.ieta();//-41 +41 !=0
    int ieta  = cell.ieta();
    if(ieta > 0) ieta -= 1;//-41 +41
    int sub   = cell.subdet(); // 1-HB, 2-HE (HFDigiCollection: 4-HF)
    badchannels0[sub-1][depth-1][ieta+41][iphi] += 1;// 0-82 ; 0-71
    if (verbosity == -22) std::cout << std::endl << " fillDigiAmplitude     DIGI ->  "  << "sub, ieta, iphi, depth = "  <<  sub << ", " << ieta << ", " << iphi << ", " << depth << std::endl;
    // !!!!!!
    if (verbosity == -22) std::cout <<" fillDigiAmplitude     Size of Digi "<<(*digiItr).size()<<std::endl;
    ///////////////////////////////////////Energy
    // Energy:    
    int firstSample  = 2;  // RECO window parameters
    int samplesToAdd = 2;
    HcalCalibrations calib = conditions->getHcalCalibrations(cell);
    const HcalQIECoder* channelCoder = conditions->getHcalCoder(cell);
    HcalCoderDb coder (*channelCoder, *shape);
    coder.adc2fC(*digiItr,tool);
    if (verbosity == -22) std::cout << "fillDigiAmplitude    coder done..." << std::endl;
    double amplitude = 0.;
    double amplallTS = 0.;
    double ampl = 0.;
    double timew = 0.;

    for (int ii=0; ii<tool.size(); ii++) {  
      //      double ampldefault = adc2fC[digiItr->sample(ii).adc()];
      double ampldefault = tool[ii];
      double ampltool = tool[ii];
//      if (verbosity == -22) std::cout << "fillDigiAmplitude    ampldefault = " << ampldefault << " ampltool = " << ampltool << std::endl;
      int capid = ((*digiItr)[ii]).capid();
      double pedestal = calib.pedestal(capid);
      double ta = (ampltool-pedestal); // pedestal subtraction
      double calibrespcorrgain = calib.respcorrgain(capid) ;   // fC --> GeV
      if (verbosity == -22) std::cout << "fillDigiAmplitude    pedestal = " << pedestal << " calibrespcorrgain = " << calibrespcorrgain << std::endl;
      ta*= calibrespcorrgain;

      amplitude+=ampldefault;// fC
      amplallTS+=ta;// GeV
      if (ii >= firstSample && ii < firstSample+samplesToAdd )  ampl+=ta; // GeV 

      if (verbosity == -22) std::cout << "fillDigiAmplitude    amplitude = " << amplitude << " ampl = " << ampl << std::endl;
      timew += (ii+1)*ampldefault;
    }//for
    if (verbosity == -22) std::cout << std::endl << "*** E = " << ampl << "   ACD -> fC -> (gain ="<< calib.respcorrgain(0) << ") GeV (ped.subtracted)" << std::endl;

    double ratio = 0.;
    if(amplallTS != 0.) ratio = ampl/amplallTS;
    if (verbosity == -22) std::cout << std::endl << "*** ratio = " << ratio << "   ampl ="<< ampl << "   amplallTS ="<< amplallTS << std::endl;

    double aveamplitude = 0.;
    if(amplitude !=0)  aveamplitude = timew/amplitude;
    double rmsamp = 0.;
    for (int ii=0; ii<tool.size(); ii++) {  
      double ampldefault = tool[ii];
      double aaaaaa = (ii+1)-aveamplitude;
      double aaaaaa2 = aaaaaa*aaaaaa;
      rmsamp+=aaaaaa2*ampldefault;// fC
    }//for
    double rmsamplitude = 0.;
    if(amplitude !=0)  rmsamplitude = sqrt( rmsamp/amplitude );

    ///////////////////////////////////////Digis
    // HB
    if ( sub == 1 ) {
      
      h_Amplitude_HB->Fill(rmsamplitude,1.);
      //	if(rmsamplitude > 2.25 ) {
      //	if(rmsamplitude < -1. || rmsamplitude > 2.025 ) {
      if(rmsamplitude < rmsHBMin_ || rmsamplitude > rmsHBMax_ ) {
	if (verbosity == -54) std::cout << std::endl << "***BAD HB channels from shape RMS:  "  <<" ieta= " << ieta <<" iphi= " << iphi << std::endl;
	if(depth==1) h_mapDepth1Amplitude225_HB->Fill(double(ieta), double(iphi), 1.);
	if(depth==2) h_mapDepth2Amplitude225_HB->Fill(double(ieta), double(iphi), 1.);
	if(depth==3) h_mapDepth3Amplitude225_HB->Fill(double(ieta), double(iphi), 1.);
      } 
      if(depth==1) h_mapDepth1Amplitude_HB->Fill(double(ieta), double(iphi), rmsamplitude);    
      if(depth==2) h_mapDepth2Amplitude_HB->Fill(double(ieta), double(iphi), rmsamplitude);    
      if(depth==3) h_mapDepth3Amplitude_HB->Fill(double(ieta), double(iphi), rmsamplitude);  
      
      h_Ampl_HB->Fill(ratio,1.);
      //	if(ratio < 0.4 || ratio > 0.7  ) {
      if(ratio < ratioHBMin_ || ratio > ratioHBMax_  ) {
	if (verbosity == -53) std::cout << std::endl << "***BAD HB channels from shape Ratio:  " <<" ieta= " << ieta <<" iphi= " << iphi << std::endl;
	if(depth==1) h_mapDepth1Ampl047_HB->Fill(double(ieta), double(iphi), 1.);
	if(depth==2) h_mapDepth2Ampl047_HB->Fill(double(ieta), double(iphi), 1.);
	if(depth==3) h_mapDepth3Ampl047_HB->Fill(double(ieta), double(iphi), 1.);
	// //
	badchannels[sub-1][depth-1][ieta+41][iphi] += 1;// 0-82 ; 0-71
	// //
      }
      
      if(depth==1) h_mapDepth1Ampl_HB->Fill(double(ieta), double(iphi), ratio);    
      if(depth==2) h_mapDepth2Ampl_HB->Fill(double(ieta), double(iphi), ratio);    
      if(depth==3) h_mapDepth3Ampl_HB->Fill(double(ieta), double(iphi), ratio);  
      
      if(depth==1) h_mapDepth1AmplE34_HB->Fill(double(ieta), double(iphi), ampl);    
      if(depth==2) h_mapDepth2AmplE34_HB->Fill(double(ieta), double(iphi), ampl);    
      if(depth==3) h_mapDepth3AmplE34_HB->Fill(double(ieta), double(iphi), ampl);  
      
      if(depth==1) h_mapDepth1_HB->Fill(double(ieta), double(iphi), 1.);    
      if(depth==2) h_mapDepth2_HB->Fill(double(ieta), double(iphi), 1.);    
      if(depth==3) h_mapDepth3_HB->Fill(double(ieta), double(iphi), 1.);    
    }//if ( sub == 1 )
    // HE
    if ( sub == 2 ) {
      
      h_Amplitude_HE->Fill(rmsamplitude,1.);
      //	if(rmsamplitude > 2.25 ) {
      //	if(rmsamplitude < -1. || rmsamplitude > 2.225 ) {
      if(rmsamplitude < rmsHEMin_ || rmsamplitude > rmsHEMax_ ) {
	if (verbosity == -54) std::cout << std::endl << "***BAD HE channels from shape RMS:  " <<" ieta= " << ieta <<" iphi= " << iphi << std::endl;
	if(depth==1) h_mapDepth1Amplitude225_HE->Fill(double(ieta), double(iphi), 1.);
	if(depth==2) h_mapDepth2Amplitude225_HE->Fill(double(ieta), double(iphi), 1.);
	if(depth==3) h_mapDepth3Amplitude225_HE->Fill(double(ieta), double(iphi), 1.);
      } 
      if(depth==1) h_mapDepth1Amplitude_HE->Fill(double(ieta), double(iphi), rmsamplitude);    
      if(depth==2) h_mapDepth2Amplitude_HE->Fill(double(ieta), double(iphi), rmsamplitude);    
      if(depth==3) h_mapDepth3Amplitude_HE->Fill(double(ieta), double(iphi), rmsamplitude);  
      
      h_Ampl_HE->Fill(ratio,1.);
      //	if(ratio < 0.3 || ratio > 0.8  ) {
      if(ratio < ratioHEMin_ || ratio > ratioHEMax_  ) {
	if (verbosity == -53) std::cout << std::endl << "***BAD HE channels from shape Ratio:  " <<" ieta= " << ieta <<" iphi= " << iphi << std::endl;
	if(depth==1) h_mapDepth1Ampl047_HE->Fill(double(ieta), double(iphi), 1.);
	if(depth==2) h_mapDepth2Ampl047_HE->Fill(double(ieta), double(iphi), 1.);
	if(depth==3) h_mapDepth3Ampl047_HE->Fill(double(ieta), double(iphi), 1.);
	// //
	badchannels[sub-1][depth-1][ieta+41][iphi] += 1;// 0-82 ; 0-71
	// //
      }
      if(depth==1) h_mapDepth1Ampl_HE->Fill(double(ieta), double(iphi), ratio);    
      if(depth==2) h_mapDepth2Ampl_HE->Fill(double(ieta), double(iphi), ratio);    
      if(depth==3) h_mapDepth3Ampl_HE->Fill(double(ieta), double(iphi), ratio);  
      
      if(depth==1) h_mapDepth1AmplE34_HE->Fill(double(ieta), double(iphi), ampl);    
      if(depth==2) h_mapDepth2AmplE34_HE->Fill(double(ieta), double(iphi), ampl);    
      if(depth==3) h_mapDepth3AmplE34_HE->Fill(double(ieta), double(iphi), ampl);  
      
      if(depth==1) h_mapDepth1_HE->Fill(double(ieta), double(iphi), 1.);    
      if(depth==2) h_mapDepth2_HE->Fill(double(ieta), double(iphi), 1.);    
      if(depth==3) h_mapDepth3_HE->Fill(double(ieta), double(iphi), 1.);    
    }//if ( sub == 2 )
    //    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ method called to for each event  ------------
int VeRawAnalyzer::getRBX(int& kdet, int& keta, int& kphi){
  
  // Find the correspondance between ieta, iphi and RBX
  
  int cal_RBX=0;
  // HBHE
  if(kdet==1 || kdet==2) {
    if(kphi==71) cal_RBX=0;
    else cal_RBX=(kphi+1)/4;
    cal_RBX=cal_RBX+18*(keta+1)/2;
  }
  // HF
  if(kdet == 4 ){
    cal_RBX = (int)(kphi/18)+1;
  }
  // HO
  if(kdet==3){
    if(keta==-2){
      if(kphi==71) cal_RBX=0;
      else cal_RBX=kphi/12+1;
    }
    if(keta==-1){
      if(kphi==71) cal_RBX=6;
      else cal_RBX=kphi/12+1+6;
    }
    if(keta==0){
      if(kphi==71) cal_RBX=12;
      else cal_RBX=kphi/6+1+12;
    }
    if(keta==1){
      if(kphi==71) cal_RBX=24;
      else cal_RBX=kphi/12+1+24;
    }
    if(keta==2){
      if(kphi==71) cal_RBX=30;
      else cal_RBX=kphi/12+1+30;
    }
  }
  return cal_RBX;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ method called to for each event  ------------
void VeRawAnalyzer::endJob(){   
  
  hOutputFile->SetCompressionLevel(2);
  
  hOutputFile->Write();   
  hOutputFile->cd();
  myTree->Write();
  ///////////////////////////////////////////////////////////////////////////////////////////////
  std::cout << "===== full number of events =  " << nevent << std::endl;
  std::cout << "===== full number of events*HBHEdigis =  " << nnnnnn << std::endl;
  std::cout << "===== full number of events*HFdigis =  " << counter << std::endl;
  
  std::cout << "===== Start writing user histograms =====" << std::endl;
  
  ///////////////////////////////////////////->Write();
  h_errorGeneral->Write();
  h_error1->Write();
  h_error2->Write();
  h_error3->Write();
  h_amplError->Write();
  h_amplFine->Write();

  h_errorGeneral_HB->Write();
  h_error1_HB->Write();
  h_error2_HB->Write();
  h_error3_HB->Write();
  h_error4_HB->Write();
  h_error5_HB->Write();
  h_error6_HB->Write();
  h_error7_HB->Write();
  h_amplError_HB->Write();
  h_amplFine_HB->Write();
  h_mapDepth1Error_HB->Write();
  h_mapDepth2Error_HB->Write();
  h_mapDepth3Error_HB->Write();
  h_fiber0_HB->Write();
  h_fiber1_HB->Write();
  h_fiber2_HB->Write();
  h_repetedcapid_HB->Write();

  h_errorGeneral_HE->Write();
  h_error1_HE->Write();
  h_error2_HE->Write();
  h_error3_HE->Write();
  h_error4_HE->Write();
  h_error5_HE->Write();
  h_error6_HE->Write();
  h_error7_HE->Write();
  h_amplError_HE->Write();
  h_amplFine_HE->Write();
  h_mapDepth1Error_HE->Write();
  h_mapDepth2Error_HE->Write();
  h_mapDepth3Error_HE->Write();
  h_fiber0_HE->Write();
  h_fiber1_HE->Write();
  h_fiber2_HE->Write();
  h_repetedcapid_HE->Write();

    ///////////////////////
    h_Amplitude_HB->Write();
    h_mapDepth1Amplitude225_HB->Write();
    h_mapDepth2Amplitude225_HB->Write();
    h_mapDepth3Amplitude225_HB->Write();
    h_mapDepth1Amplitude_HB->Write();
    h_mapDepth2Amplitude_HB->Write();
    h_mapDepth3Amplitude_HB->Write();
    h_Ampl_HB->Write();
    h_mapDepth1Ampl047_HB->Write();
    h_mapDepth2Ampl047_HB->Write();
    h_mapDepth3Ampl047_HB->Write();
    h_mapDepth1Ampl_HB->Write();
    h_mapDepth2Ampl_HB->Write();
    h_mapDepth3Ampl_HB->Write();
    h_mapDepth1AmplE34_HB->Write();
    h_mapDepth2AmplE34_HB->Write();
    h_mapDepth3AmplE34_HB->Write();
    h_mapDepth1_HB->Write();
    h_mapDepth2_HB->Write();
    h_mapDepth3_HB->Write();
    
    h_Amplitude_HE->Write();
    h_mapDepth1Amplitude225_HE->Write();
    h_mapDepth2Amplitude225_HE->Write();
    h_mapDepth3Amplitude225_HE->Write();
    h_mapDepth1Amplitude_HE->Write();
    h_mapDepth2Amplitude_HE->Write();
    h_mapDepth3Amplitude_HE->Write();
    h_Ampl_HE->Write();
    h_mapDepth1Ampl047_HE->Write();
    h_mapDepth2Ampl047_HE->Write();
    h_mapDepth3Ampl047_HE->Write();
    h_mapDepth1Ampl_HE->Write();
    h_mapDepth2Ampl_HE->Write();
    h_mapDepth3Ampl_HE->Write();
    h_mapDepth1AmplE34_HE->Write();
    h_mapDepth2AmplE34_HE->Write();
    h_mapDepth3AmplE34_HE->Write();
    h_mapDepth1_HE->Write();
    h_mapDepth2_HE->Write();
    h_mapDepth3_HE->Write();
    
    ///////////////////////
    h_GetRMSOverNormalizedSignal_HB->Write();
    h_GetRMSOverNormalizedSignal_HE->Write();
    h_GetRMSOverNormalizedSignal_HO->Write();
    h_GetRMSOverNormalizedSignal_HF->Write();
    h_NormalizedSignal3D_HB->Write();
    h_NormalizedSignal3D0_HB->Write();
    h_NormalizedSignal3D_HE->Write();
    h_NormalizedSignal3D0_HE->Write();
    h_NormalizedSignal3D_HO->Write();
    h_NormalizedSignal3D0_HO->Write();
    h_NormalizedSignal3D_HF->Write();
    h_NormalizedSignal3D0_HF->Write();

    h_GetRMSOverNormalizedSignal3_HB->Write();
    h_GetRMSOverNormalizedSignal3_HE->Write();
    h_GetRMSOverNormalizedSignal3_HO->Write();
    h_GetRMSOverNormalizedSignal3_HF->Write();

    h_nbadchannels_depth1_HB->Write();
    h_runnbadchannels_depth1_HB->Write();
    h_badrate_depth1_HB->Write();
    h_runbadrate_depth1_HB->Write();
    h_runbadrate0_depth1_HB->Write();

    h_nbadchannels_depth2_HB->Write();
    h_runnbadchannels_depth2_HB->Write();
    h_badrate_depth2_HB->Write();
    h_runbadrate_depth2_HB->Write();
    h_runbadrate0_depth2_HB->Write();

    h_nbadchannels_depth1_HE->Write();
    h_runnbadchannels_depth1_HE->Write();
    h_badrate_depth1_HE->Write();
    h_runbadrate_depth1_HE->Write();
    h_runbadrate0_depth1_HE->Write();

    h_nbadchannels_depth2_HE->Write();
    h_runnbadchannels_depth2_HE->Write();
    h_badrate_depth2_HE->Write();
    h_runbadrate_depth2_HE->Write();
    h_runbadrate0_depth2_HE->Write();

    h_nbadchannels_depth3_HE->Write();
    h_runnbadchannels_depth3_HE->Write();
    h_badrate_depth3_HE->Write();
    h_runbadrate_depth3_HE->Write();
    h_runbadrate0_depth3_HE->Write();
    ///////////////////////

    h_RatioCalib_HB->Write();
    h_mapRatioCalib047_HB->Write();
    h_mapRatioCalib_HB->Write();
    h_map_HB->Write();
    h_RatioCalib_HE->Write();
    h_mapRatioCalib047_HE->Write();
    h_mapRatioCalib_HE->Write();
    h_map_HE->Write();
    h_RatioCalib_HO->Write();
    h_mapRatioCalib047_HO->Write();
    h_mapRatioCalib_HO->Write();
    h_map_HO->Write();
    h_RatioCalib_HF->Write();
    h_mapRatioCalib047_HF->Write();
    h_mapRatioCalib_HF->Write();
    h_map_HF->Write();
    ///////////////////////
    ///////////////////////
  hOutputFile->Close() ;
  std::cout << "===== Finish writing user histograms =====" << std::endl;
    ///////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SERVICE FUNCTIONS --------------------------------------------------------

double VeRawAnalyzer::dR(double eta1, double phi1, double eta2, double phi2) { 
  double PI = 3.1415926535898;
  double deltaphi= phi1 - phi2;
  if( phi2 > phi1 ) { deltaphi= phi2 - phi1;}
  if(deltaphi > PI) { deltaphi = 2.*PI - deltaphi;}
  double deltaeta = eta2 - eta1;
  double tmp = sqrt(deltaeta* deltaeta + deltaphi*deltaphi);
  return tmp;
}

double VeRawAnalyzer::phi12(double phi1, double en1, double phi2, double en2) {
  // weighted mean value of phi1 and phi2
  
  double tmp;
  double PI = 3.1415926535898;
  double a1 = phi1; double a2  = phi2;

  if( a1 > 0.5*PI  && a2 < 0.) a2 += 2*PI;
  if( a2 > 0.5*PI  && a1 < 0.) a1 += 2*PI; 
  tmp = (a1 * en1 + a2 * en2)/(en1 + en2);
  if(tmp > PI) tmp -= 2.*PI; 
 
  return tmp;

}

double VeRawAnalyzer::dPhiWsign(double phi1, double phi2) {
  // clockwise      phi2 w.r.t phi1 means "+" phi distance
  // anti-clockwise phi2 w.r.t phi1 means "-" phi distance 

  double PI = 3.1415926535898;
  double a1 = phi1; double a2  = phi2;
  double tmp =  a2 - a1;
  if( a1*a2 < 0.) {
    if(a1 > 0.5 * PI)  tmp += 2.*PI;
    if(a2 > 0.5 * PI)  tmp -= 2.*PI;
  }
  return tmp;

}

//define this as a plug-in
DEFINE_FWK_MODULE(VeRawAnalyzer);
