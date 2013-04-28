// How to run:
//root -b -q -l RemoteMonitoringMAP.C+
//root -b -q -l 'RemoteMonitoringMAP.C+("test7runs.root","test207167.root")'
//root -b -q -l 'RemoteMonitoringMAP.C+("test7longrun.root","test204172.root")'
//root -b -q -l 'RemoteMonitoringMAP.C+("test205355.root","test204172.root")'

#include "LogEleMapdb.h"

#include <iostream>
#include <fstream>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TROOT.h"
#include <TMath.h>
#include "TStyle.h"
#include "TSystem.h"
#include "TLegend.h"
#include "TText.h"
#include "TAxis.h"
#include "TFile.h"
#include "TLine.h"
#include "TGraph.h"

using namespace std;

void RemoteMonitoringMAP(const char* fname = "test7runs.root", const char* refname = "test207167.root")
{
    gROOT->Reset();
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);  
    gStyle->SetOptTitle(1);  

//======================================================================
// Connect the input files, parameters and get the 2-d histogram in memory
//    TFile *hfile= new TFile("GlobalHist.root", "READ");
    string runnumber;
    runnumber = "LED "+ (string) fname;
    TFile *hfile= new TFile( fname, "READ");
    TFile *hreffile= new TFile(refname, "READ");
     //CUTS:    [test][subdetector]                                  ADC amplitude  Am      Width  for Wm             Ratio min cut for Rm       TS mean for TNm      TS max  for TXm
    double MIN_M[7][5]={{0., 0.,0.,0.,0.}, {0., 0.,0.,0.,0.},      {0, 70.,70.,40.,300.}, {0, 0.7, 0.7, 0.1, 0.1}, {0, 0.48, 0.48, 0.48, 0.48}, {0, 1.0, 1.0, 1.0, 1.0}, {0, 1.5, 1.5, 0.5, 0.5}};
    double MAX_M[7][5]={{0., 0.,0.,0.,0.}, {0., 0.,0.,0.,0.},   {0, 1E20,1E20,1E20,1E20}, {0, 2.4, 2.4, 2.5, 2.5}, {0, 0.92, 0.92, 1.02, 1.04}, {0, 5.2, 5.2, 4.5, 5.3}, {0, 6.5, 6.5, 6.5, 6.5}};                    
//======================================================================



//======================================================================
// Prepare histograms and plot them to .png files

 
  TCanvas *cHB = new TCanvas("cHB","cHB",1000,500);
  TCanvas *cHE = new TCanvas("cHE","cHE",1500,500);
  TCanvas *cONE = new TCanvas("cONE","cONE",500,500);
  
  char *str = (char*)alloca(10000);


  TH2F *Map_Ampl[10][5][5]; // 2D histogramm for test,subdet,depth
  TH2F *Map_SUB[5][5];      // 2d histogramm for subdet, depth
  TH1F *HistAmplDepth[10][5][5];    // 1d histogramm for test,subdet, depth
  TH1F *HistAmpl[10][5];    // 1d histogramm for test,subdet

  Map_SUB[1][1] = (TH2F*)hfile->Get("h_mapDepth1_HB");
  Map_SUB[1][2] = (TH2F*)hfile->Get("h_mapDepth2_HB");   
  Map_SUB[2][1] = (TH2F*)hfile->Get("h_mapDepth1_HE");
  Map_SUB[2][2] = (TH2F*)hfile->Get("h_mapDepth2_HE"); 
  Map_SUB[2][3] = (TH2F*)hfile->Get("h_mapDepth3_HE"); 
  Map_SUB[3][4] = (TH2F*)hfile->Get("h_mapDepth4_HO"); 
  Map_SUB[4][1] = (TH2F*)hfile->Get("h_mapDepth1_HF");
  Map_SUB[4][2] = (TH2F*)hfile->Get("h_mapDepth2_HF");  
   
//+++++++++++++++++++++++++++++  
//Test 1 (Cm) Rate of Cap ID errors   
//+++++++++++++++++++++++++++++ 
 
  Map_Ampl[1][1][1] = (TH2F*)hfile->Get("h_mapDepth1Error_HB");
  Map_Ampl[1][1][2] = (TH2F*)hfile->Get("h_mapDepth2Error_HB");   
  Map_Ampl[1][2][1] = (TH2F*)hfile->Get("h_mapDepth1Error_HE");
  Map_Ampl[1][2][2] = (TH2F*)hfile->Get("h_mapDepth2Error_HE"); 
  Map_Ampl[1][2][3] = (TH2F*)hfile->Get("h_mapDepth3Error_HE");    
  Map_Ampl[1][3][4] = (TH2F*)hfile->Get("h_mapDepth4Error_HO");
  Map_Ampl[1][4][1] = (TH2F*)hfile->Get("h_mapDepth1Error_HF");
  Map_Ampl[1][4][2] = (TH2F*)hfile->Get("h_mapDepth2Error_HF"); 
  
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);
       int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
       int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k);  
            Map_Ampl[1][sub][k]->Divide(Map_Ampl[1][sub][k],Map_SUB[sub][k], 1, 1, "B"); 
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogz();
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k);  
            Map_Ampl[1][sub][k]->SetTitle(str);
            Map_Ampl[1][sub][k]->SetXTitle("#eta \b");
            Map_Ampl[1][sub][k]->SetYTitle("#phi \b");
            Map_Ampl[1][sub][k]->SetZTitle("Rate \b");
            Map_Ampl[1][sub][k]->Draw("COLZ");
            Map_Ampl[1][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
            Map_Ampl[1][sub][k]->GetZaxis()->SetRangeUser(0.0001, 1.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();} 
       }//end depth
    
       if (sub==1) {cHB->Print("MapRateCapIDHB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("MapRateCapIDHE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MapRateCapIDHO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MapRateCapIDHF.png"); cHB->Clear();}    
  }// end sub  


//+++++++++++++++++++++++++++++  
//Test 2 (Am) ADC amplitude   
//+++++++++++++++++++++++++++++
 
  Map_Ampl[2][1][1] = (TH2F*)hfile->Get("h_mapDepth1ADCAmpl225_HB");
  Map_Ampl[2][1][2] = (TH2F*)hfile->Get("h_mapDepth2ADCAmpl225_HB");   
  Map_Ampl[2][2][1] = (TH2F*)hfile->Get("h_mapDepth1ADCAmpl225_HE");
  Map_Ampl[2][2][2] = (TH2F*)hfile->Get("h_mapDepth2ADCAmpl225_HE"); 
  Map_Ampl[2][2][3] = (TH2F*)hfile->Get("h_mapDepth3ADCAmpl225_HE"); 
  Map_Ampl[2][3][4] = (TH2F*)hfile->Get("h_mapDepth4ADCAmpl225_HO");
  Map_Ampl[2][4][1] = (TH2F*)hfile->Get("h_mapDepth1ADCAmpl225_HF");
  Map_Ampl[2][4][2] = (TH2F*)hfile->Get("h_mapDepth2ADCAmpl225_HF"); 
  
  HistAmpl[2][1] = (TH1F*)hfile->Get("h_ADCAmpl_HB");
  HistAmpl[2][2] = (TH1F*)hfile->Get("h_ADCAmpl_HE");
  HistAmpl[2][3] = (TH1F*)hfile->Get("h_ADCAmpl_HO");
  HistAmpl[2][4] = (TH1F*)hfile->Get("h_ADCAmpl_HF");
      

//+++++++++++++++++++++++++++++  
//Test 3 (Wm) Rate of RMS   
//+++++++++++++++++++++++++++++ 
 
  Map_Ampl[3][1][1] = (TH2F*)hfile->Get("h_mapDepth1Amplitude225_HB");
  Map_Ampl[3][1][2] = (TH2F*)hfile->Get("h_mapDepth2Amplitude225_HB");   
  Map_Ampl[3][2][1] = (TH2F*)hfile->Get("h_mapDepth1Amplitude225_HE");
  Map_Ampl[3][2][2] = (TH2F*)hfile->Get("h_mapDepth2Amplitude225_HE"); 
  Map_Ampl[3][2][3] = (TH2F*)hfile->Get("h_mapDepth3Amplitude225_HE"); 
  Map_Ampl[3][3][4] = (TH2F*)hfile->Get("h_mapDepth4Amplitude225_HO");
  Map_Ampl[3][4][1] = (TH2F*)hfile->Get("h_mapDepth1Amplitude225_HF");
  Map_Ampl[3][4][2] = (TH2F*)hfile->Get("h_mapDepth2Amplitude225_HF");   
  
  HistAmpl[3][1] = (TH1F*)hfile->Get("h_Amplitude_HB");
  HistAmpl[3][2] = (TH1F*)hfile->Get("h_Amplitude_HE");
  HistAmpl[3][3] = (TH1F*)hfile->Get("h_Amplitude_HO");
  HistAmpl[3][4] = (TH1F*)hfile->Get("h_Amplitude_HF");
  
  //+++++++++++++++++++++++++++++  
//Test 4 (Rm) Rate of ratio 4 near max TS/ All TS   
//+++++++++++++++++++++++++++++
 
  Map_Ampl[4][1][1] = (TH2F*)hfile->Get("h_mapDepth1Ampl047_HB");
  Map_Ampl[4][1][2] = (TH2F*)hfile->Get("h_mapDepth2Ampl047_HB");   
  Map_Ampl[4][2][1] = (TH2F*)hfile->Get("h_mapDepth1Ampl047_HE");
  Map_Ampl[4][2][2] = (TH2F*)hfile->Get("h_mapDepth2Ampl047_HE"); 
  Map_Ampl[4][2][3] = (TH2F*)hfile->Get("h_mapDepth3Ampl047_HE"); 
  Map_Ampl[4][3][4] = (TH2F*)hfile->Get("h_mapDepth4Ampl047_HO");
  Map_Ampl[4][4][1] = (TH2F*)hfile->Get("h_mapDepth1Ampl047_HF");
  Map_Ampl[4][4][2] = (TH2F*)hfile->Get("h_mapDepth2Ampl047_HF"); 
  
  
  HistAmpl[4][1] = (TH1F*)hfile->Get("h_Ampl_HB");
  HistAmpl[4][2] = (TH1F*)hfile->Get("h_Ampl_HE");
  HistAmpl[4][3] = (TH1F*)hfile->Get("h_Ampl_HO");
  HistAmpl[4][4] = (TH1F*)hfile->Get("h_Ampl_HF");  
  
//+++++++++++++++++++++++++++++  
//Test 5 (TNm) Mean position in 1-8 TS range   
//+++++++++++++++++++++++++++++
 
  Map_Ampl[5][1][1] = (TH2F*)hfile->Get("h_mapDepth1TSmeanA225_HB");
  Map_Ampl[5][1][2] = (TH2F*)hfile->Get("h_mapDepth2TSmeanA225_HB");   
  Map_Ampl[5][2][1] = (TH2F*)hfile->Get("h_mapDepth1TSmeanA225_HE");
  Map_Ampl[5][2][2] = (TH2F*)hfile->Get("h_mapDepth2TSmeanA225_HE"); 
  Map_Ampl[5][2][3] = (TH2F*)hfile->Get("h_mapDepth3TSmeanA225_HE"); 
  Map_Ampl[5][3][4] = (TH2F*)hfile->Get("h_mapDepth4TSmeanA225_HO");
  Map_Ampl[5][4][1] = (TH2F*)hfile->Get("h_mapDepth1TSmeanA225_HF");
  Map_Ampl[5][4][2] = (TH2F*)hfile->Get("h_mapDepth2TSmeanA225_HF"); 
  
  HistAmpl[5][1] = (TH1F*)hfile->Get("h_TSmeanA_HB");
  HistAmpl[5][2] = (TH1F*)hfile->Get("h_TSmeanA_HE");
  HistAmpl[5][3] = (TH1F*)hfile->Get("h_TSmeanA_HO");
  HistAmpl[5][4] = (TH1F*)hfile->Get("h_TSmeanA_HF");


//+++++++++++++++++++++++++++++  
//Test 6 (TXm) Maximum position in 1-8 TS range   
//+++++++++++++++++++++++++++++
 
  Map_Ampl[6][1][1] = (TH2F*)hfile->Get("h_mapDepth1TSmaxA225_HB");
  Map_Ampl[6][1][2] = (TH2F*)hfile->Get("h_mapDepth2TSmaxA225_HB");   
  Map_Ampl[6][2][1] = (TH2F*)hfile->Get("h_mapDepth1TSmaxA225_HE");
  Map_Ampl[6][2][2] = (TH2F*)hfile->Get("h_mapDepth2TSmaxA225_HE"); 
  Map_Ampl[6][2][3] = (TH2F*)hfile->Get("h_mapDepth3TSmaxA225_HE"); 
  Map_Ampl[6][3][4] = (TH2F*)hfile->Get("h_mapDepth4TSmaxA225_HO");
  Map_Ampl[6][4][1] = (TH2F*)hfile->Get("h_mapDepth1TSmaxA225_HF");
  Map_Ampl[6][4][2] = (TH2F*)hfile->Get("h_mapDepth2TSmaxA225_HF"); 
  
  HistAmpl[6][1] = (TH1F*)hfile->Get("h_TSmaxA_HB");
  HistAmpl[6][2] = (TH1F*)hfile->Get("h_TSmaxA_HE");
  HistAmpl[6][3] = (TH1F*)hfile->Get("h_TSmaxA_HO");
  HistAmpl[6][4] = (TH1F*)hfile->Get("h_TSmaxA_HF");
  


  

  for (int test=2;test<=6;test++) { //Test: 2-Am, 3-Wm, 4-Rm, 5-TNm, 6-TXm, 
      for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
          if (sub==1) cHB->Divide(2,1);
          if (sub==2) cHE->Divide(3,1);
          if (sub==3) cONE->Divide(1,1);
          if (sub==4) cHB->Divide(2,1);
          int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
          int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
          for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
              if (sub==1) cHB->cd(k); 
              if (sub==2) cHE->cd(k);
	      if (sub==3) cONE->cd(k-3);
	      if (sub==4) cHB->cd(k);  
              Map_Ampl[test][sub][k]->Divide(Map_Ampl[test][sub][k],Map_SUB[sub][k], 1, 1, "B"); 
              gPad->SetGridy();
              gPad->SetGridx();
              gPad->SetLogz();
	      if (sub==3) HistAmpl[test][sub]->SetTitleOffset(0.5,"Z");
              if (sub==1) sprintf(str,"HB, Depth%d \b", k);
              if (sub==2) sprintf(str,"HE, Depth%d \b", k);
              if (sub==3) sprintf(str,"HO, Depth%d \b", k);
              if (sub==4) sprintf(str,"HF, Depth%d \b", k);  
              Map_Ampl[test][sub][k]->SetTitle(str);
              Map_Ampl[test][sub][k]->SetXTitle("#eta \b");
              Map_Ampl[test][sub][k]->SetYTitle("#phi \b");
              Map_Ampl[test][sub][k]->SetZTitle("Rate \b");
              Map_Ampl[test][sub][k]->Draw("COLZ");
              Map_Ampl[test][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
              Map_Ampl[test][sub][k]->GetZaxis()->SetRangeUser(0.0001, 1.);
              if (sub==1) {cHB->Modified(); cHB->Update();} 
              if (sub==2) {cHE->Modified(); cHE->Update();}
              if (sub==3) {cONE->Modified();cONE->Update();}
              if (sub==4) {cHB->Modified(); cHB->Update();} 
          }//end depth    
          if (test==2){ 
	     if (sub==1) {cHB->Print("MapRateAmplHB.png"); cHB->Clear();} 
             if (sub==2) {cHE->Print("MapRateAmplHE.png"); cHE->Clear();}
             if (sub==3) {cONE->Print("MapRateAmplHO.png"); cONE->Clear();}
             if (sub==4) {cHB->Print("MapRateAmplHF.png"); cHB->Clear();} 
	  }   
	  if (test==3){ 
	     if (sub==1) {cHB->Print("MapRateRMSHB.png"); cHB->Clear();} 
             if (sub==2) {cHE->Print("MapRateRMSHE.png"); cHE->Clear();}
             if (sub==3) {cONE->Print("MapRateRMSHO.png"); cONE->Clear();}
             if (sub==4) {cHB->Print("MapRateRMSHF.png"); cHB->Clear();} 
	  }
	  if (test==4){ 
	     if (sub==1) {cHB->Print("MapRate43TStoAllTSHB.png"); cHB->Clear();} 
             if (sub==2) {cHE->Print("MapRate43TStoAllTSHE.png"); cHE->Clear();}
             if (sub==3) {cONE->Print("MapRate43TStoAllTSHO.png"); cONE->Clear();}
             if (sub==4) {cHB->Print("MapRate43TStoAllTSHF.png"); cHB->Clear();} 
	  }
	  if (test==5){ 
	     if (sub==1) {cHB->Print("MapRateMeanPosHB.png"); cHB->Clear();} 
             if (sub==2) {cHE->Print("MapRateMeanPosHE.png"); cHE->Clear();}
             if (sub==3) {cONE->Print("MapRateMeanPosHO.png"); cONE->Clear();}
             if (sub==4) {cHB->Print("MapRateMeanPosHF.png"); cHB->Clear();} 
	  }
          if (test==6){ 
	     if (sub==1) {cHB->Print("MapRateMaxPosHB.png"); cHB->Clear();} 
             if (sub==2) {cHE->Print("MapRateMaxPosHE.png"); cHE->Clear();}
             if (sub==3) {cONE->Print("MapRateMaxPosHO.png"); cONE->Clear();}
             if (sub==4) {cHB->Print("MapRateMaxPosHF.png"); cHB->Clear();} 
	  }
   
          cONE->Divide(1,1);
          cONE->cd(1);
          gPad->SetGridy();
          gPad->SetGridx(); 
          gPad->SetLogy();
          if (sub==1) HistAmpl[test][sub]->SetTitle("HB, All Depth");
          if (sub==2) HistAmpl[test][sub]->SetTitle("HE, All Depth");
          if (sub==3) HistAmpl[test][sub]->SetTitle("HO, All Depth");
          if (sub==4) HistAmpl[test][sub]->SetTitle("HF, All Depth");
          if (test==2) HistAmpl[test][sub]->SetXTitle("ADC Amlitude in each event & cell \b");
	  if (test==3) HistAmpl[test][sub]->SetXTitle("TS RMS in each event & cell \b");
	  if (test==4) HistAmpl[test][sub]->SetXTitle("Ratio in each event & cell \b");
	  if (test==5) HistAmpl[test][sub]->SetXTitle("Mean TS position in each event & cell \b");
	  if (test==6) HistAmpl[test][sub]->SetXTitle("Max TS position in each event & cell \b");
          HistAmpl[test][sub]->SetYTitle("Number of cell-events \b");
	  HistAmpl[test][sub]->SetLineColor(4);
          HistAmpl[test][sub]->SetLineWidth(2);
          HistAmpl[test][sub]->Draw("");
//        HistAmpl[test][sub]->GetYaxis()->SetRangeUser(1., 100.);
          if (test==2) {gPad->SetLogx(); HistAmpl[test][sub]->GetXaxis()->SetRangeUser(1., 10000.);}
	  if (test==3) HistAmpl[test][sub]->GetXaxis()->SetRangeUser(0., 5.);
	  if (test==4) HistAmpl[test][sub]->GetXaxis()->SetRangeUser(0., 1.);
	  if (test==5) HistAmpl[test][sub]->GetXaxis()->SetRangeUser(0., 9.);
          if (test==6) HistAmpl[test][sub]->GetXaxis()->SetRangeUser(0., 9.);
	  cONE->Modified(); 
          cONE->Update(); 
	  float min_x[] = {MIN_M[test][sub],MIN_M[test][sub]};
          float min_y[] = {0.,100000000.};
          TGraph* MIN = new TGraph(2, min_x, min_y);
          MIN->SetLineStyle(2);
          MIN->SetLineColor(2);
          MIN->SetLineWidth(2 + 100*100);
          MIN->SetFillStyle(3005);
          MIN->SetFillColor(2);
          MIN->Draw("L"); 
          float max_x[] = {MAX_M[test][sub],MAX_M[test][sub]};
          float max_y[] = {0.,100000000.};
          TGraph* MAX = new TGraph(2, max_x, max_y);
          MAX->SetLineStyle(2);
          MAX->SetLineColor(2);
          MAX->SetLineWidth(-2 - 100*100);
          MAX->SetFillStyle(3004);
          MAX->SetFillColor(2);
          MAX->Draw("L");          
	  if (test==2){ 
	     if (sub==1) {cONE->Print("HistAmplHB.png"); cONE->Clear();} 
             if (sub==2) {cONE->Print("HistAmplHE.png"); cONE->Clear();}
             if (sub==3) {cONE->Print("HistAmplHO.png"); cONE->Clear();}
             if (sub==4) {cONE->Print("HistAmplHF.png"); cONE->Clear();}  
	  } 
          if (test==3){ 
	     if (sub==1) {cONE->Print("HistRMSHB.png"); cONE->Clear();} 
             if (sub==2) {cONE->Print("HistRMSHE.png"); cONE->Clear();}
             if (sub==3) {cONE->Print("HistRMSHO.png"); cONE->Clear();}
             if (sub==4) {cONE->Print("HistRMSHF.png"); cONE->Clear();}  
	  } 
	  if (test==4){ 
	     if (sub==1) {cONE->Print("Hist43TStoAllTSHB.png"); cONE->Clear();} 
             if (sub==2) {cONE->Print("Hist43TStoAllTSHE.png"); cONE->Clear();}
             if (sub==3) {cONE->Print("Hist43TStoAllTSHO.png"); cONE->Clear();}
             if (sub==4) {cONE->Print("Hist43TStoAllTSHF.png"); cONE->Clear();}  
	  } 
	  if (test==5){ 
	     if (sub==1) {cONE->Print("HistMeanPosHB.png"); cONE->Clear();} 
             if (sub==2) {cONE->Print("HistMeanPosHE.png"); cONE->Clear();}
             if (sub==3) {cONE->Print("HistMeanPosHO.png"); cONE->Clear();}
             if (sub==4) {cONE->Print("HistMeanPosHF.png"); cONE->Clear();}  
	  }
	  if (test==6){ 
	     if (sub==1) {cONE->Print("HistMaxPosHB.png"); cONE->Clear();} 
             if (sub==2) {cONE->Print("HistMaxPosHE.png"); cONE->Clear();}
             if (sub==3) {cONE->Print("HistMaxPosHO.png"); cONE->Clear();}
             if (sub==4) {cONE->Print("HistMaxPosHF.png"); cONE->Clear();}  
	  } 	                  
      }// end sub      
  }//end test
  
//+++++++++++++++++++++++++++++  
//Test 7 (GS) Amplitude drift   
//+++++++++++++++++++++++++++++
 
  Map_Ampl[7][1][1] = (TH2F*)hfile->Get("h_mapDepth1AmplE34_HB");
  Map_Ampl[7][1][2] = (TH2F*)hfile->Get("h_mapDepth2AmplE34_HB");   
  Map_Ampl[7][2][1] = (TH2F*)hfile->Get("h_mapDepth1AmplE34_HE");
  Map_Ampl[7][2][2] = (TH2F*)hfile->Get("h_mapDepth2AmplE34_HE"); 
  Map_Ampl[7][2][3] = (TH2F*)hfile->Get("h_mapDepth3AmplE34_HE"); 
  Map_Ampl[7][3][4] = (TH2F*)hfile->Get("h_mapDepth4AmplE34_HO");
  Map_Ampl[7][4][1] = (TH2F*)hfile->Get("h_mapDepth1AmplE34_HF");
  Map_Ampl[7][4][2] = (TH2F*)hfile->Get("h_mapDepth2AmplE34_HF");   
  
  TH2F *Map_RefAmpl[7][5]; // 2D histogramm for subdet, depth
  TH2F *Map_RefSUB[7][5];  // 2d histogramm for subdet, depth

  Map_RefAmpl[1][1] = (TH2F*)hreffile->Get("h_mapDepth1AmplE34_HB");
  Map_RefAmpl[1][2] = (TH2F*)hreffile->Get("h_mapDepth2AmplE34_HB");   
  Map_RefAmpl[2][1] = (TH2F*)hreffile->Get("h_mapDepth1AmplE34_HE");
  Map_RefAmpl[2][2] = (TH2F*)hreffile->Get("h_mapDepth2AmplE34_HE"); 
  Map_RefAmpl[2][3] = (TH2F*)hreffile->Get("h_mapDepth3AmplE34_HE"); 
  Map_RefAmpl[3][4] = (TH2F*)hreffile->Get("h_mapDepth4AmplE34_HO");
  Map_RefAmpl[4][1] = (TH2F*)hreffile->Get("h_mapDepth1AmplE34_HF");
  Map_RefAmpl[4][2] = (TH2F*)hreffile->Get("h_mapDepth2AmplE34_HF");   

  Map_RefSUB[1][1] = (TH2F*)hreffile->Get("h_mapDepth1_HB");
  Map_RefSUB[1][2] = (TH2F*)hreffile->Get("h_mapDepth2_HB");   
  Map_RefSUB[2][1] = (TH2F*)hreffile->Get("h_mapDepth1_HE");
  Map_RefSUB[2][2] = (TH2F*)hreffile->Get("h_mapDepth2_HE"); 
  Map_RefSUB[2][3] = (TH2F*)hreffile->Get("h_mapDepth3_HE"); 
  Map_RefSUB[3][4] = (TH2F*)hreffile->Get("h_mapDepth4_HO");
  Map_RefSUB[4][1] = (TH2F*)hreffile->Get("h_mapDepth1_HF");
  Map_RefSUB[4][2] = (TH2F*)hreffile->Get("h_mapDepth2_HF");  
  
  HistAmplDepth[7][1][1] = new TH1F("diffAmpl_Depth1_HB","", 100, -10., 10.); 
  HistAmplDepth[7][1][2] = new TH1F("diffAmpl_Depth2_HB","", 100, -10., 10.); 
  HistAmplDepth[7][2][1] = new TH1F("diffAmpl_Depth1_HE","", 100, -10., 10.); 
  HistAmplDepth[7][2][2] = new TH1F("diffAmpl_Depth2_HE","", 100, -10., 10.);
  HistAmplDepth[7][2][3] = new TH1F("diffAmpl_Depth3_HE","", 100, -10., 10.); 
  HistAmplDepth[7][3][4] = new TH1F("diffAmpl_Depth4_HO","", 100, -10., 10.); 
  HistAmplDepth[7][4][1] = new TH1F("diffAmpl_Depth1_HF","", 100, -10., 10.);
  HistAmplDepth[7][4][2] = new TH1F("diffAmpl_Depth2_HF","", 100, -10., 10.);

  double porog[5] = {0., 2., 2., 2., 2.};
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);
       int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
       int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k);
	    Map_Ampl[7][sub][k]->Divide(Map_Ampl[7][sub][k], Map_SUB[sub][k], 1, 1, "B"); 
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogz();
            gStyle->SetTitleOffset(0.5, "Y");  
	    gStyle->SetTitleOffset(2.5, "Z");  
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k); 
            Map_Ampl[7][sub][k]->SetTitle(str);
            Map_Ampl[7][sub][k]->SetXTitle("#eta \b");
            Map_Ampl[7][sub][k]->SetYTitle("#phi \b");
            Map_Ampl[7][sub][k]->SetZTitle("Response \b");
            Map_Ampl[7][sub][k]->Draw("COLZ");
            Map_Ampl[7][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
//            Map_Ampl[7][sub][k]->GetZaxis()->SetRangeUser(1., 10.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();}             
       } //end depth 
       if (sub==1) {cHB->Print("MapRateAmpl1HB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("MapRateAmpl1HE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MapRateAmpl1HO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MapRateAmpl1HF.png"); cHB->Clear();}  
       
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);   
       
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k);
            Map_RefAmpl[sub][k]->Divide(Map_RefAmpl[sub][k], Map_RefSUB[sub][k], 1, 1, "B");
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogz();
            gStyle->SetTitleOffset(0.5, "Y");  
	    gStyle->SetTitleOffset(2.5, "Z");
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k); 
            Map_Ampl[7][sub][k]->SetTitle(str);
            Map_Ampl[7][sub][k]->SetXTitle("#eta \b");
            Map_Ampl[7][sub][k]->SetYTitle("#phi \b");
            Map_Ampl[7][sub][k]->SetZTitle("Response\b");
            Map_Ampl[7][sub][k]->Draw("COLZ");
            Map_Ampl[7][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
//            Map_Ampl[7][sub][k]->GetZaxis()->SetRangeUser(1., 10.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();}             
       }//end depth  
       if (sub==1) {cHB->Print("MapRateAmpl2HB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("MapRateAmpl2HE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MapRateAmpl2HO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MapRateAmpl2HF.png"); cHB->Clear();}   
         
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);   
          
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k);     
	    for (int x=1;x<=Map_Ampl[7][sub][k]->GetXaxis()->GetNbins();x++) {
                for (int y=1;y<=Map_Ampl[7][sub][k]->GetYaxis()->GetNbins(); y++) {
	            if (Map_Ampl[7][sub][k]->GetBinContent(x,y) !=0 && Map_RefAmpl[sub][k]->GetBinContent(x,y) !=0 ) {
	               double ccc1 =  Map_Ampl[7][sub][k]->GetBinContent(x,y)   -   Map_RefAmpl[sub][k]->GetBinContent(x,y)  ;
	               ccc1 = 100.*ccc1/Map_Ampl[7][sub][k]->GetBinContent(x,y);
	               HistAmplDepth[7][sub][k]->Fill(ccc1);
	               if(fabs(ccc1) > porog[sub])  Map_Ampl[7][sub][k]->SetBinContent(x,y,fabs(ccc1));
	               else  Map_Ampl[7][sub][k]->SetBinContent(x,y,0); 
	            }
                }//end y
            }//esnd x
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogz();
            gStyle->SetTitleOffset(0.5, "Y");  
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k); 
            Map_Ampl[7][sub][k]->SetTitle(str);
            Map_Ampl[7][sub][k]->SetXTitle("#eta \b");
            Map_Ampl[7][sub][k]->SetYTitle("#phi \b");
            Map_Ampl[7][sub][k]->SetZTitle("Rate \b");
            Map_Ampl[7][sub][k]->Draw("COLZ");
            Map_Ampl[7][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
            Map_Ampl[7][sub][k]->GetZaxis()->SetRangeUser(1., 10.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();} 
       }//end depth
    
       if (sub==1) {cHB->Print("MapRateAmplDriftHB.png"); cHB->Clear(); } 
       if (sub==2) {cHE->Print("MapRateAmplDriftHE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MapRateAmplDriftHO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MapRateAmplDriftHF.png"); cHB->Clear();} 
       
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);   
          
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k); 
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogy();
            gStyle->SetTitleOffset(2.5, "Y");  
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k);  
            HistAmplDepth[7][sub][k]->SetTitle(str);
            HistAmplDepth[7][sub][k]->SetYTitle("Number of cell-events \b");
            HistAmplDepth[7][sub][k]->SetXTitle("Per cent \b");
            HistAmplDepth[7][sub][k]->Draw();
//          HistAmplDepth[7][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
            HistAmplDepth[7][sub][k]->GetXaxis()->SetRangeUser(-10., 10.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();} 
       } // end depth    
       if (sub==1) {cHB->Print("HistAmplDriftDepthHB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("HistAmplDriftDepthHE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("HistAmplDriftDepthHO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("HistAmplDriftDepthHF.png"); cHB->Clear();}   
 } //end sub 
    
//+++++++++++++++++++++++++++++  
//Test 8 (Rc) Ratio of calibration channel   
//+++++++++++++++++++++++++++++ 
 
  Map_Ampl[8][1][1] = (TH2F*)hfile->Get("h_mapRatioCalib047_HB");
  Map_Ampl[8][1][2] = (TH2F*)hfile->Get("h_mapRatioCalib047_HB");   
  Map_Ampl[8][2][1] = (TH2F*)hfile->Get("h_mapRatioCalib047_HE");
  Map_Ampl[8][2][2] = (TH2F*)hfile->Get("h_mapRatioCalib047_HE"); 
  Map_Ampl[8][2][3] = (TH2F*)hfile->Get("h_mapRatioCalib047_HE"); 
  Map_Ampl[8][3][4] = (TH2F*)hfile->Get("h_mapRatioCalib047_HO");
  Map_Ampl[8][4][1] = (TH2F*)hfile->Get("h_mapRatioCalib047_HF");
  Map_Ampl[8][4][2] = (TH2F*)hfile->Get("h_mapRatioCalib047_HF");  
  
  TH2F *Map_Calib[5][5];  // 2d histogramm for subdet, depth
  
  Map_Calib[1][1] = (TH2F*)hfile->Get("h_map_HB");
  Map_Calib[1][2] = (TH2F*)hfile->Get("h_map_HB");   
  Map_Calib[2][1] = (TH2F*)hfile->Get("h_map_HE");
  Map_Calib[2][2] = (TH2F*)hfile->Get("h_map_HE"); 
  Map_Calib[2][3] = (TH2F*)hfile->Get("h_map_HE");    
  Map_Calib[3][4] = (TH2F*)hfile->Get("h_map_HO");
  Map_Calib[4][1] = (TH2F*)hfile->Get("h_map_HF");
  Map_Calib[4][2] = (TH2F*)hfile->Get("h_map_HF");
  
  HistAmpl[8][1] = (TH1F*)hfile->Get("h_RatioCalib_HB");
  HistAmpl[8][2] = (TH1F*)hfile->Get("h_RatioCalib_HE");
  HistAmpl[8][3] = (TH1F*)hfile->Get("h_RatioCalib_HO");
  HistAmpl[8][4] = (TH1F*)hfile->Get("h_RatioCalib_HF"); 
  
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);
       int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
       int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k);  
            Map_Ampl[8][sub][k]->Divide(Map_Ampl[8][sub][k],Map_Calib[sub][k], 1, 1, "B"); 
            gPad->SetGridy();
            gPad->SetGridx();
            gPad->SetLogz();
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k);  
            Map_Ampl[8][sub][k]->SetTitle(str);
            Map_Ampl[8][sub][k]->SetXTitle("#eta \b");
            Map_Ampl[8][sub][k]->SetYTitle("#phi \b");
            Map_Ampl[8][sub][k]->SetZTitle("Rate \b");
            Map_Ampl[8][sub][k]->Draw("COLZ");
//           Map_Ampl[8][sub][k]->GetYaxis()->SetRangeUser(0, 72.);
            Map_Ampl[8][sub][k]->GetZaxis()->SetRangeUser(0.0000001, 1.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();} 
       }//end depth
    
       if (sub==1) {cHB->Print("MapRateCalibHB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("MapRateCalibHE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MapRateCalibHO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MapRateCalibHF.png"); cHB->Clear();} 
       cONE->Divide(1,1);
       cONE->cd(1);
       gPad->SetGridy();
       gPad->SetGridx(); 
       gPad->SetLogy();
       HistAmpl[8][sub]->SetTitleOffset(1.3,"Y"); 
       if (sub==1) HistAmpl[8][sub]->SetTitle("HB, All Depth");
       if (sub==2) HistAmpl[8][sub]->SetTitle("HE, All Depth");
       if (sub==3) HistAmpl[8][sub]->SetTitle("HO, All Depth");
       if (sub==4) HistAmpl[8][sub]->SetTitle("HF, All Depth");
       HistAmpl[8][sub]->SetXTitle("Ratio in each event & cell \b");;
       HistAmpl[8][sub]->SetYTitle("Number of cell-events \b");
       HistAmpl[8][sub]->SetLineColor(4);
       HistAmpl[8][sub]->SetLineWidth(2);
       HistAmpl[8][sub]->Draw("");
//     HistAmpl[8][sub]->GetYaxis()->SetRangeUser(1., 100.);
       HistAmpl[8][sub]->GetXaxis()->SetRangeUser(0., 1.);
       cONE->Modified(); 
       cONE->Update();
       float min_x[] = {MIN_M[8][sub],MIN_M[8][sub]};
       float min_y[] = {0.,100000000.};
       TGraph* MIN = new TGraph(2, min_x, min_y);
       MIN->SetLineStyle(2);
       MIN->SetLineColor(2);
       MIN->SetLineWidth(2 + 100*100);
       MIN->SetFillStyle(3005);
       MIN->SetFillColor(2);
       MIN->Draw("L"); 
       float max_x[] = {MAX_M[8][sub],MAX_M[8][sub]};
       float max_y[] = {0.,100000000.};
       TGraph* MAX = new TGraph(2, max_x, max_y);
       MAX->SetLineStyle(2);
       MAX->SetLineColor(2);
       MAX->SetLineWidth(-2 - 100*100);
       MAX->SetFillStyle(3004);
       MAX->SetFillColor(2);
       MAX->Draw("L");                   
       if (sub==1) {cONE->Print("HistCalibHB.png"); cONE->Clear();} 
       if (sub==2) {cONE->Print("HistCalibHE.png"); cONE->Clear();}
       if (sub==3) {cONE->Print("HistCalibHO.png"); cONE->Clear();}
       if (sub==4) {cONE->Print("HistCalibHF.png"); cONE->Clear();}     
  }// end sub  
//======================================================================


//======================================================================
/// Prepare maps of good/bad channels:

    TH2F *Map_ALL = new TH2F("Map","Map", 82, -41, 40, 72, 0, 71);             
    int nx = Map_ALL->GetXaxis()->GetNbins();
    int ny = Map_ALL->GetYaxis()->GetNbins();
    int NBad = 0;
    int NWarn = 0;
    int Eta[3][10000]={0};
    int Phi[3][10000]={0};
    int Sub[3][10000]={0};
    int Depth[3][10000]={0};
    string Comment[3][10000]={""};
    string Text[9]={"","Am","Cm","Wm","Rm","TNm","TXm", "GS", "Rc"};
    int flag_W = 0;
    int flag_B = 0;
    
    for (int i=1;i<=nx;i++) {  //Eta
       for (int j=1;j<=ny;j++) {	// Phi  
          for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HO, 4-HF
	     int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
	     int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
	     for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
	         if (Map_SUB[sub][k]->GetBinContent(i,j)!=0) {Map_SUB[sub][k]->SetBinContent(i,j,0.5);Map_ALL->SetBinContent(i,j,0.5);}
	     }	
          }
       }
    }   	  

    for (int i=1;i<=nx;i++) {  //Eta
       for (int j=1;j<=ny;j++) {	// Phi  
          for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
	     int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
	     int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
	     for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
	        flag_W = 0;
	        flag_B = 0;		
	        for (int test=1;test<=6;test++) { //Test: 1-Wm, 2-Rm, etc
//Warning		
 
                   if ((Map_Ampl[test][sub][k]->GetBinContent(i,j) != 0.)&&(Map_Ampl[test][sub][k]->GetBinContent(i,j) < 0.001) )  {
	              if (Map_SUB[sub][k]->GetBinContent(i,j)!=1.) Map_SUB[sub][k]->SetBinContent(i,j,0.8);
		      if (Map_ALL->GetBinContent(i,j)!=1.) Map_ALL->SetBinContent(i,j,0.8);
		      if (flag_W == 0) {
		         NWarn +=1; 
		         Eta[1][NWarn]=i-41;
		         Phi[1][NWarn]=j-1;
			 Sub[1][NWarn]=sub;
		         Depth[1][NWarn]=k;
		         Comment[1][NWarn]=Text[test]; 
		      } 
		      else Comment[1][NWarn]+=", "+Text[test];
		      flag_W = 1;		      		 
//		      cout<<"Map_Ampl["<<test<<"]["<<sub<<"]["<<k<<"]->GetBinContent("<<i<<","<<j<<")= "<<Map_Ampl[test][sub][k]->GetBinContent(i,j)<<endl;
                   }
//Bad			     	                 
                   if (Map_Ampl[test][sub][k]->GetBinContent(i,j) >= 0.001 )  {
	              Map_ALL->SetBinContent(i,j,1.);
		      Map_SUB[sub][k]->SetBinContent(i,j,1.);
		      if (flag_B == 0) {
		         NBad +=1; 
		         Eta[2][NBad]=i-41;
		         Phi[2][NBad]=j-1;
			 Sub[2][NBad]=sub;
		         Depth[2][NBad]=k;
		         Comment[2][NBad]=Text[test];
		      } 
		      else Comment[2][NBad]+=", "+Text[test];
		      flag_B = 1;		 
//		      cout<<"Map_Ampl["<<test<<"]["<<sub<<"]["<<k<<"]->GetBinContent("<<i<<","<<j<<")= "<<Map_Ampl[test][sub][k]->GetBinContent(i,j)<<endl;
                   }
		} //end test
		 for (int test=7;test<=8;test++) { //Test: 1-2.E, 2-2.F, etc
//Warning	
	
                   if (Map_Ampl[test][sub][k]->GetBinContent(i,j) != 0.)  {
//	              if (Map_SUB[sub][k]->GetBinContent(i,j)!=1.0) Map_SUB[sub][k]->SetBinContent(i,j,0.8);
//		      if (Map_ALL->GetBinContent(i,j)!=1.) Map_ALL->SetBinContent(i,j,0.8);
		      if (flag_W == 0) {
		         NWarn +=1; 
		         Eta[1][NWarn]=i-41;
		         Phi[1][NWarn]=j-1;
			 Sub[1][NWarn]=sub;
		         Depth[1][NWarn]=k;
		         Comment[1][NWarn]=Text[test]; 
		      } 
		      else Comment[1][NWarn]+=", "+Text[test];
		      flag_W = 1;		      		 
//		      cout<<"Map_Ampl["<<test<<"]["<<sub<<"]["<<k<<"]->GetBinContent("<<i<<","<<j<<")= "<<Map_Ampl[test][sub][k]->GetBinContent(i,j)<<endl;
                  }
		 
		} //end test
             }//end Depth
	  }//end Sub 	  	  
       }//end Phi
    }//end Eta 
                   
// subdet maps
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
       if (sub==1) cHB->Divide(2,1);
       if (sub==2) cHE->Divide(3,1);
       if (sub==3) cONE->Divide(1,1);
       if (sub==4) cHB->Divide(2,1);
       int k_min[5]={0,1,1,4,1}; // minimum depth for each subdet
       int k_max[5]={0,2,3,4,2}; // maximum depth for each subdet	
       for (int k=k_min[sub];k<=k_max[sub];k++) {  //Depth 
            if (sub==1) cHB->cd(k); 
            if (sub==2) cHE->cd(k);
	    if (sub==3) cONE->cd(k-3);
	    if (sub==4) cHB->cd(k); 
            gPad->SetGridy();
            gPad->SetGridx();
//          gPad->SetLogz();
//          gStyle->SetTitleOffset(0.5, "Y");  
            if (sub==1) sprintf(str,"HB, Depth%d \b", k);
            if (sub==2) sprintf(str,"HE, Depth%d \b", k);
            if (sub==3) sprintf(str,"HO, Depth%d \b", k);
            if (sub==4) sprintf(str,"HF, Depth%d \b", k); 
            Map_SUB[sub][k]->SetTitle(str);
            Map_SUB[sub][k]->SetXTitle("#eta \b");
            Map_SUB[sub][k]->SetYTitle("#phi \b");
            Map_SUB[sub][k]->Draw("COL");
            Map_SUB[sub][k]->GetYaxis()->SetRangeUser(0, 72.);
            Map_SUB[sub][k]->GetZaxis()->SetRangeUser(0., 1.);
            if (sub==1) {cHB->Modified(); cHB->Update();} 
            if (sub==2) {cHE->Modified(); cHE->Update();}
            if (sub==3) {cONE->Modified();cONE->Update();}
            if (sub==4) {cHB->Modified(); cHB->Update();}      
       } //end depth   
       if (sub==1) {cHB->Print("MAPHB.png"); cHB->Clear();} 
       if (sub==2) {cHE->Print("MAPHE.png"); cHE->Clear();}
       if (sub==3) {cONE->Print("MAPHO.png"); cONE->Clear();}
       if (sub==4) {cHB->Print("MAPHF.png"); cHB->Clear();} 
  }
  
// ALL SubDet  
  gStyle->SetOptTitle(0);
  TCanvas *cmain = new TCanvas("cmain","MAP",800,800);
  gPad->SetGridy();
  gPad->SetGridx();
//   gPad->SetLogz();
  Map_ALL->SetTitleOffset(1.3,"Y");
  Map_ALL->SetXTitle("#eta \b");
  Map_ALL->SetYTitle("#phi \b");
  Map_ALL->Draw("COL");
  Map_ALL->GetYaxis()->SetRangeUser(0, 72.);
  Map_ALL->GetZaxis()->SetRangeUser(0, 1.);
  cmain->Modified(); 
  cmain->Update();   
  cmain->Print("MAP.png"); 
            
//======================================================================

//======================================================================
// Creating each test kind for each subdet html pages:
  std::string raw_class;  
  int ind = 0;
  
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
     ofstream htmlFileT,htmlFileC, htmlFileD;
     if (sub==1) {htmlFileT.open("HB_Tile.html"); htmlFileC.open("HB_Calib.html");htmlFileD.open("HB_Drift.html");}
     if (sub==2) {htmlFileT.open("HE_Tile.html"); htmlFileC.open("HE_Calib.html");htmlFileD.open("HE_Drift.html");}
     if (sub==3) {htmlFileT.open("HO_Tile.html"); htmlFileC.open("HO_Calib.html");htmlFileD.open("HO_Drift.html");}
     if (sub==4) {htmlFileT.open("HF_Tile.html"); htmlFileC.open("HF_Calib.html");htmlFileD.open("HF_Drift.html");}
 
// Megatile channels   
     htmlFileT << "</html><html xmlns=\"http://www.w3.org/1999/xhtml\">"<< std::endl;
     htmlFileT << "<head>"<< std::endl;
     htmlFileT << "<meta http-equiv=\"Content-Type\" content=\"text/html\"/>"<< std::endl;
     htmlFileT << "<title> Raw Data Analyser </title>"<< std::endl;
     htmlFileT << "<style type=\"text/css\">"<< std::endl;
     htmlFileT << " body,td{ background-color: #FFFFCC; font-family: arial, arial ce, helvetica; font-size: 12px; }"<< std::endl;
     htmlFileT << "   td.s0 { font-family: arial, arial ce, helvetica; }"<< std::endl;
     htmlFileT << "   td.s1 { font-family: arial, arial ce, helvetica; font-weight: bold; background-color: #FFC169; text-align: center;}"<< std::endl;
     htmlFileT << "   td.s2 { font-family: arial, arial ce, helvetica; background-color: #eeeeee; }"<< std::endl;
     htmlFileT << "   td.s3 { font-family: arial, arial ce, helvetica; background-color: #d0d0d0; }"<< std::endl;
     htmlFileT << "   td.s4 { font-family: arial, arial ce, helvetica; background-color: #FFC169; }"<< std::endl;
     htmlFileT << "</style>"<< std::endl;
     htmlFileT << "<body>"<< std::endl;

          
     if (sub==1) htmlFileT << "<h1> Tests of megatile channels for HB, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     if (sub==2) htmlFileT << "<h1> Tests of megatile channels for HE, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==3) htmlFileT << "<h1> Tests of megatile channels for HO, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==4) htmlFileT << "<h1> Tests of megatile channels for HF, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     htmlFileT << "<br>"<< std::endl;




// Test Cm
         
     htmlFileT << "<h2> 1.Cm test: CapID errors for each channel.</h3>"<< std::endl;
     htmlFileT << "<h3> 1.A. Rate of CapId failures in each channel for each depth.</h3>"<< std::endl;
     htmlFileT << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRateCapIDHB.png\" />" << std::endl;
     if (sub==2) htmlFileT << " <img src=\"MapRateCapIDHE.png\" />" << std::endl;
     if (sub==3) htmlFileT << " <img src=\"MapRateCapIDHO.png\" />" << std::endl;
     if (sub==4) htmlFileT << " <img src=\"MapRateCapIDHF.png\" />" << std::endl;
     htmlFileT << "<br>"<< std::endl;

// Am         
     htmlFileT << "<h2> 2. Am test: Mean ADC amplitude over all timeslices (TS) for each channel. </h3>"<< std::endl;
     htmlFileT << "<h3> 2.A. Combined mean ADC amplitude distribution over all events, channels and depths.</h3>"<< std::endl;
     htmlFileT << "<h4> Legend: Bins less "<< MIN_M[2][sub] <<" correpond to bad ADC amplitude </h4>"<< std::endl; 
     if (sub==1) htmlFileT << " <img src=\"HistAmplHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"HistAmplHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"HistAmplHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"HistAmplHF.png\" />" << std::endl;
     htmlFileT << "<br>"<< std::endl; 
     htmlFileT << "<h3> 2.B. Rate of bad ADC amplitude (<"<<MIN_M[2][sub]<<") in each channel for each depth. </h3>"<< std::endl;
     htmlFileT << "<h4> Channel legend: wthite - good, other colours - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRateAmplHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"MapRateAmplHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"MapRateAmplHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"MapRateAmplHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl;

// Test Wm        
     htmlFileT << "<h2> 3. Wm test: RMS (width) of ADC amplutude for each channel.</h3>"<< std::endl;
     htmlFileT << "<h3> 3.A. Combined RMS distribution over all events, channel and depth.</h3>"<< std::endl;
     htmlFileT << "<h4> Legend: Bins less "<< MIN_M[3][sub] <<" and more "<< MAX_M[3][sub] <<" correpond to bad RMS </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"HistRMSHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"HistRMSHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"HistRMSHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"HistRMSHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl; 
     htmlFileT << "<h3> 2.B. Rate of bad RMS (<"<<MIN_M[3][sub]<<",>"<<MAX_M[3][sub]<<") in each channel for each depth.</h3>"<< std::endl; 
     htmlFileT << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRateRMSHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"MapRateRMSHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"MapRateRMSHO.png\" />" << std::endl;
     if (sub==4) htmlFileT << " <img src=\"MapRateRMSHF.png\" />" << std::endl;
     htmlFileT << "<br>"<< std::endl;

// Rm       
     htmlFileT << "<h2> 4. Rm test: Ratio ADC value sum over four near maximum (-2, -1, max, +1) TS to ADC value sum over all TS for each channel. </h3>"<< std::endl;
     htmlFileT << "<h3> 4.A. Combined ratio distribution over all events, channels and depths.</h3>"<< std::endl;
     htmlFileT << "<h4> Legend: Bins less "<<MIN_M[4][sub]<<" and more "<<MAX_M[4][sub]<<" correpond to bad ratio </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"Hist43TStoAllTSHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"Hist43TStoAllTSHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"Hist43TStoAllTSHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"Hist43TStoAllTSHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl; 
     htmlFileT << "<h3> 3.B. Rate of bad ratio (<"<<MIN_M[4][sub] <<", >"<<MAX_M[4][sub]<<") in each channel for each depth.</h3>"<< std::endl;
     htmlFileT << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRate43TStoAllTSHB.png\" />" << std::endl;  
     if (sub==2) htmlFileT << " <img src=\"MapRate43TStoAllTSHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"MapRate43TStoAllTSHO.png\" />" << std::endl;  
     if (sub==4) htmlFileT << " <img src=\"MapRate43TStoAllTSHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl;
     
// TNm          
     htmlFileT << "<h2> 5. TNm test: Mean TS position for each channel.</h3>"<< std::endl;
     htmlFileT << "<h3> 5.A. Combined mean TS position distribution over all events, channels and depths.</h3>"<< std::endl;
     htmlFileT << "<h4> Legend: Bins less "<<MIN_M[5][sub]<<" and more "<<MAX_M[5][sub]<<" correpond to bad position </h4>"<< std::endl; 
     if (sub==1) htmlFileT << " <img src=\"HistMeanPosHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"HistMeanPosHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"HistMeanPosHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"HistMeanPosHF.png\" />" << std::endl;
     htmlFileT << "<br>"<< std::endl; 
     htmlFileT << "<h3> 5.B. Rate of bad TS position  (<"<<MIN_M[5][sub]<<", >"<<MAX_M[5][sub]<<") in each channel for each depth. </h3>"<< std::endl;
     htmlFileT << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRateMeanPosHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"MapRateMeanPosHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"MapRateMeanPosHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"MapRateMeanPosHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl;

// TXm         
     htmlFileT << "<h2> 6.TXm test: Maximum TS position for each channel.</h3>"<< std::endl;
     htmlFileT << "<h3> 6.A. Combined maximum TS position distribution over all events, channel and depth.</h3>"<< std::endl; 
     htmlFileT << "<h4> Legend: Bins less "<<MIN_M[6][sub]<<" and more "<<MAX_M[6][sub]<<" correpond to bad position </h4>"<< std::endl; 
     if (sub==1) htmlFileT << " <img src=\"HistMaxPosHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"HistMaxPosHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"HistMaxPosHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"HistMaxPosHF.png\" />" << std::endl;
     htmlFileT << "<br>"<< std::endl; 
     htmlFileT << "<h3> 6.B. Rate of bad TS position  (<"<<MIN_M[6][sub]<<", >"<<MAX_M[6][sub]<<") in each channel for each depth. </h3>"<< std::endl;
     htmlFileT << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileT << " <img src=\"MapRateMaxPosHB.png\" />" << std::endl; 
     if (sub==2) htmlFileT << " <img src=\"MapRateMaxPosHE.png\" />" << std::endl; 
     if (sub==3) htmlFileT << " <img src=\"MapRateMaxPosHO.png\" />" << std::endl; 
     if (sub==4) htmlFileT << " <img src=\"MapRateMaxPosHF.png\" />" << std::endl; 
     htmlFileT << "<br>"<< std::endl;      

 
     htmlFileT << "</body> " << std::endl;
     htmlFileT << "</html> " << std::endl;
     htmlFileT.close();  
 
 
//Calibration channels
     htmlFileC << "</html><html xmlns=\"http://www.w3.org/1999/xhtml\">"<< std::endl;
     htmlFileC << "<head>"<< std::endl;
     htmlFileC << "<meta http-equiv=\"Content-Type\" content=\"text/html\"/>"<< std::endl;
     htmlFileC << "<title> Raw Data Analyser </title>"<< std::endl;
     htmlFileC << "<style type=\"text/css\">"<< std::endl;
     htmlFileC << " body,td{ background-color: #FFFFCC; font-family: arial, arial ce, helvetica; font-size: 12px; }"<< std::endl;
     htmlFileC << "   td.s0 { font-family: arial, arial ce, helvetica; }"<< std::endl;
     htmlFileC << "   td.s1 { font-family: arial, arial ce, helvetica; font-weight: bold; background-color: #FFC169; text-align: center;}"<< std::endl;
     htmlFileC << "   td.s2 { font-family: arial, arial ce, helvetica; background-color: #eeeeee; }"<< std::endl;
     htmlFileC << "   td.s3 { font-family: arial, arial ce, helvetica; background-color: #d0d0d0; }"<< std::endl;
     htmlFileC << "   td.s4 { font-family: arial, arial ce, helvetica; background-color: #FFC169; }"<< std::endl;
     htmlFileC << "</style>"<< std::endl;
     htmlFileC << "<body>"<< std::endl;

     if (sub==1) htmlFileC << "<h1> Tests for calibration channels for HB, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     if (sub==2) htmlFileC << "<h1> Tests for calibration channels for HE, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==3) htmlFileC << "<h1> Tests for calibration channels for HO, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==4) htmlFileC << "<h1> Tests for calibration channels for HF, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     htmlFileC << "<br>"<< std::endl;

// Test Rc    
     htmlFileC << "<h2> 1. Rc test: Ratio ADC value sum over four near maximum (-2, max, +1) TS to ADC value sum over all TS for each calibration channel. </h3>"<< std::endl;
     htmlFileC << "<h3> 1.A. Combined ratio distribution over all events, channel and depth.</h3>"<< std::endl;
     htmlFileC << "<h4> Legend: Bins less 0.3 and more 0.9 correpond to bad ratio </h4>"<< std::endl;
     if (sub==1) htmlFileC << " <img src=\"HistCalibHB.png\" />" << std::endl; 
     if (sub==2) htmlFileC << " <img src=\"HistCalibHE.png\" />" << std::endl; 
     if (sub==3) htmlFileC << " <img src=\"HistCalibHO.png\" />" << std::endl; 
     if (sub==4) htmlFileC << " <img src=\"HistCalibHF.png\" />" << std::endl; 
     htmlFileC << "<br>"<< std::endl; 
     htmlFileC << "<h3> 1.B. Rate of bad ratio (<0.3, >0.9) in each channel for each depth.</h3>"<< std::endl;
     htmlFileC << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1) htmlFileC << " <img src=\"MapRateCalibHB.png\" />" << std::endl;  
     if (sub==2) htmlFileC << " <img src=\"MapRateCalibHE.png\" />" << std::endl; 
     if (sub==3) htmlFileC << " <img src=\"MapRateCalibHO.png\" />" << std::endl;  
     if (sub==4) htmlFileC << " <img src=\"MapRateCalibHF.png\" />" << std::endl; 
     htmlFileC << "<br>"<< std::endl;
    
     htmlFileC << "</body> " << std::endl;
     htmlFileC << "</html> " << std::endl;
     htmlFileC.close();  

//Response drift  
     htmlFileD << "</html><html xmlns=\"http://www.w3.org/1999/xhtml\">"<< std::endl;
     htmlFileD << "<head>"<< std::endl;
     htmlFileD << "<meta http-equiv=\"Content-Type\" content=\"text/html\"/>"<< std::endl;
     htmlFileD << "<title> Raw Data Analyser </title>"<< std::endl;
     htmlFileD << "<style type=\"text/css\">"<< std::endl;
     htmlFileD << " body,td{ background-color: #FFFFCC; font-family: arial, arial ce, helvetica; font-size: 12px; }"<< std::endl;
     htmlFileD << "   td.s0 { font-family: arial, arial ce, helvetica; }"<< std::endl;
     htmlFileD << "   td.s1 { font-family: arial, arial ce, helvetica; font-weight: bold; background-color: #FFC169; text-align: center;}"<< std::endl;
     htmlFileD << "   td.s2 { font-family: arial, arial ce, helvetica; background-color: #eeeeee; }"<< std::endl;
     htmlFileD << "   td.s3 { font-family: arial, arial ce, helvetica; background-color: #d0d0d0; }"<< std::endl;
     htmlFileD << "   td.s4 { font-family: arial, arial ce, helvetica; background-color: #FFC169; }"<< std::endl;
     htmlFileD << "</style>"<< std::endl;
     htmlFileD << "<body>"<< std::endl;

     if (sub==1) htmlFileD << "<h1> Response drift tests for HB, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     if (sub==2) htmlFileD << "<h1> Response drift tests for HE, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==3) htmlFileD << "<h1> Response drift tests for HO, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==4) htmlFileD << "<h1> Response drift tests for HF, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     htmlFileD << "<br>"<< std::endl;

// test GS     
     htmlFileD << "<h2> 1. GS test: Gain Stability </h3>"<< std::endl;
     htmlFileD << "<h3> 1.A. Averaged channel response, collected over all TS, for this run in each channel for each depth.</h3>"<< std::endl;
     htmlFileD << "<h4> Channel legend: colour means cooresponding value of mean response. </h4>"<< std::endl;
     if (sub==1)  htmlFileD << " <img src=\"MapRateAmpl1HB.png\" />" << std::endl; 
     if (sub==2)  htmlFileD << " <img src=\"MapRateAmpl1HE.png\" />" << std::endl;  
     if (sub==3)  htmlFileD << " <img src=\"MapRateAmpl1HO.png\" />" << std::endl;  
     if (sub==4)  htmlFileD << " <img src=\"MapRateAmpl1HF.png\" />" << std::endl;   
     htmlFileD << "<br>"<< std::endl; 
     htmlFileD << "<h3> 1.B.A. Averaged channel response, collected over all TS, for reper run in each channel for each depth.</h3>"<< std::endl;
     htmlFileD << "<h4> Channel legend: colour means cooresponding value of mean response. </h4>"<< std::endl;
     if (sub==1)  htmlFileD << " <img src=\"MapRateAmpl2HB.png\" />" << std::endl; 
     if (sub==2)  htmlFileD << " <img src=\"MapRateAmpl2HE.png\" />" << std::endl;  
     if (sub==3)  htmlFileD << " <img src=\"MapRateAmpl2HO.png\" />" << std::endl;  
     if (sub==4)  htmlFileD << " <img src=\"MapRateAmpl2HF.png\" />" << std::endl;   
     htmlFileD << "<br>"<< std::endl; 
     htmlFileD << "<h3> 1.C. Relative difference between this run and reper run distribution over all events, channels for each depth.</h3>"<< std::endl;
     htmlFileD << "<h4>  Legend: Bins less -2% and more +2% correpond to bad relative difference position </h4>"<< std::endl;
     if (sub==1)  htmlFileD << " <img src=\"HistAmplDriftDepthHB.png\" />" << std::endl; 
     if (sub==2)  htmlFileD << " <img src=\"HistAmplDriftDepthHE.png\" />" << std::endl; 
     if (sub==3)  htmlFileD << " <img src=\"HistAmplDriftDepthHO.png\" />" << std::endl; 
     if (sub==4)  htmlFileD << " <img src=\"HistAmplDriftDepthHF.png\" />" << std::endl;  
     htmlFileD << "<br>"<< std::endl; 
     htmlFileD << "<h3> 1.D. Rate of bad relative difference  (<-2, >+2) in each channel for each depth.</h3>"<< std::endl;
     htmlFileD << "<h4> Channel legend: wthite - good, other colour - bad. </h4>"<< std::endl;
     if (sub==1)  htmlFileD << " <img src=\"MapRateAmplDriftHB.png\" />" << std::endl; 
     if (sub==2)  htmlFileD << " <img src=\"MapRateAmplDriftHE.png\" />" << std::endl;  
     if (sub==3)  htmlFileD << " <img src=\"MapRateAmplDriftHO.png\" />" << std::endl;  
     if (sub==4)  htmlFileD << " <img src=\"MapRateAmplDriftHF.png\" />" << std::endl;        
     htmlFileD << "<br>"<< std::endl;    
 
     htmlFileD << "</body> " << std::endl;
     htmlFileD << "</html> " << std::endl;
     htmlFileD.close();  

  }// end sub

//======================================================================

//======================================================================
// Creating subdet  html pages:
  
  for (int sub=1;sub<=4;sub++) {  //Subdetector: 1-HB, 2-HE, 3-HF, 4-HO
     ofstream htmlFile;
     if (sub==1) htmlFile.open("HB.html");
     if (sub==2) htmlFile.open("HE.html");
     if (sub==3) htmlFile.open("HO.html");
     if (sub==4) htmlFile.open("HF.html");
     
     htmlFile << "</html><html xmlns=\"http://www.w3.org/1999/xhtml\">"<< std::endl;
     htmlFile << "<head>"<< std::endl;
     htmlFile << "<meta http-equiv=\"Content-Type\" content=\"text/html\"/>"<< std::endl;
     htmlFile << "<title> Raw Data Analyser </title>"<< std::endl;
     htmlFile << "<style type=\"text/css\">"<< std::endl;
     htmlFile << " body,td{ background-color: #FFFFCC; font-family: arial, arial ce, helvetica; font-size: 12px; }"<< std::endl;
     htmlFile << "   td.s0 { font-family: arial, arial ce, helvetica; }"<< std::endl;
     htmlFile << "   td.s1 { font-family: arial, arial ce, helvetica; font-weight: bold; background-color: #FFC169; text-align: center;}"<< std::endl;
     htmlFile << "   td.s2 { font-family: arial, arial ce, helvetica; background-color: #eeeeee; }"<< std::endl;
     htmlFile << "   td.s3 { font-family: arial, arial ce, helvetica; background-color: #d0d0d0; }"<< std::endl;
     htmlFile << "   td.s4 { font-family: arial, arial ce, helvetica; background-color: #FFC169; }"<< std::endl;
     htmlFile << "</style>"<< std::endl;
     htmlFile << "<body>"<< std::endl;
     if (sub==1) htmlFile << "<h1> HCAL BARREL, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     if (sub==2) htmlFile << "<h1> HCAL ENDCAP, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==3) htmlFile << "<h1> HCAL OUTER, RUN = "<< runnumber <<" </h1>"<< std::endl;
     if (sub==4) htmlFile << "<h1> HCAL FORWARD, RUN = "<< runnumber <<" </h1>"<< std::endl; 
     htmlFile << "<br>"<< std::endl;
     if (sub==1) htmlFile << "<h2> 1. Tests separately for HB</h2>"<< std::endl;   
     if (sub==2) htmlFile << "<h2> 1. Tests separately for HE</h2>"<< std::endl;
     if (sub==3) htmlFile << "<h2> 1. Tests separately for HO</h2>"<< std::endl;
     if (sub==4) htmlFile << "<h2> 1. Tests separately for HF</h2>"<< std::endl;
     htmlFile << "<table width=\"400\">"<< std::endl;
     htmlFile << "<tr>"<< std::endl;
     if (sub==1) {
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HB_Tile.html\">Megatile Channels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HB_Calib.html\">Calibration Cheannels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HB_Drift.html\">Response Drift</a></td>"<< std::endl;
     }
     if (sub==2) {
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HE_Tile.html\">Megatile Channels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HE_Calib.html\">Calibration Cheannels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HE_Drift.html\">Response Drift</a></td>"<< std::endl;
     } 
     if (sub==3) {
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HO_Tile.html\">Megatile Channels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HO_Calib.html\">Calibration Cheannels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HO_Drift.html\">Response Drift</a></td>"<< std::endl;
     } 
     if (sub==4) {
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HF_Tile.html\">Megatile Channels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HF_Calib.html\">Calibration Cheannels</a></td>"<< std::endl;
        htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HF_Drift.html\">Response Drift</a></td>"<< std::endl;
     }   
     htmlFile << "</tr>"<< std::endl;
     htmlFile << "</table>"<< std::endl;
     htmlFile << "<br>"<< std::endl;   
     if (sub==1) htmlFile << "<h2> 2.Status HB over all test </h2>"<< std::endl;
     if (sub==2) htmlFile << "<h2> 2.Status HE over all test </h2>"<< std::endl;
     if (sub==3) htmlFile << "<h2> 2.Status HO over all test </h2>"<< std::endl;
     if (sub==4) htmlFile << "<h2> 2.Status HF over all test </h2>"<< std::endl;
     htmlFile << "<h3> 2.A.Channel map for each Depth </h3>"<< std::endl;
     htmlFile << "<h4> Channel legend: green - good, yelloy - warning, red - bad, white - not applicable or out of range </h4>"<< std::endl;
     if (sub==1) htmlFile << " <img src=\"MAPHB.png\" />" << std::endl;      
     if (sub==2) htmlFile << " <img src=\"MAPHE.png\" />" << std::endl;      
     if (sub==3) htmlFile << " <img src=\"MAPHO.png\" />" << std::endl;      
     if (sub==4) htmlFile << " <img src=\"MAPHF.png\" />" << std::endl;      
     htmlFile << "<br>"<< std::endl;
     
     htmlFile << "<h3> 2.B.List of Bad channels </h3>"<< std::endl;
     htmlFile << "<table>"<< std::endl;     
     htmlFile << "<tr>";
     htmlFile << "<td class=\"s4\" align=\"center\">#</td>"    << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ETA</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PHI</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DEPTH</td>"<< std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RBX</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PIXEL</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">FIBER_CH</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">QIE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ADC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">CRATE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DCC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">SPIGOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_SLOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FPGA</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Cm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Am</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Wm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Rm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">TNm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">TXm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">GS</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Rc</td>"   << std::endl;
     htmlFile << "</tr>"   << std::endl;     
   
     for (int i=1;i<=NBad;i++) {
        if((ind%2)==1){
           raw_class="<td class=\"s2\" align=\"center\">";
        }else{
           raw_class="<td class=\"s3\" align=\"center\">";
        }
        const CellDB db;
        CellDB ce;
	if ((ce.size()>=1)&&(Sub[2][i]==sub)) {
           if (Sub[2][i]==1) {
	      ce = db.find("subdet", "HB").find("Eta", Eta[2][i]).find("Phi", Phi[2][i]).find("Depth", Depth[2][i]);
	      if (ce.size()==0) {cout<<"Error: No such HB, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HB, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	      }
	   if (Sub[2][i]==2) {
	      ce = db.find("subdet", "HE").find("Eta", Eta[2][i]).find("Phi", Phi[2][i]).find("Depth", Depth[2][i]);
	      if (ce.size()==0) {cout<<"Error: No such HE, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HE, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}	   
	      }
	   if (Sub[2][i]==3) {
	      ce = db.find("subdet", "HO").find("Eta", Eta[2][i]).find("Phi", Phi[2][i]).find("Depth", Depth[2][i]);
	      if (ce.size()==0) {cout<<"Error: No such HO, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HO, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}	   
	      }	   
	   if (Sub[2][i]==4) {
	      ce = db.find("subdet", "HF").find("Eta", Eta[2][i]).find("Phi", Phi[2][i]).find("Depth", Depth[2][i]);
	      if (ce.size()==0) {cout<<"Error: No such HF, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HF, Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}	   
	      }	
	   htmlFile << "<tr>"<< std::endl;
           htmlFile << "<td class=\"s4\" align=\"center\">" << ind+1 <<"</td>"<< std::endl;
           htmlFile << raw_class<< Eta[2][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Phi[2][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Depth[2][i] <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RBX <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RM <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Pixel <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RMfiber <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberCh <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].QIE <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].ADC<<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].VMECardID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].dccID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Spigot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberIndex <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrSlot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrTB <<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[1][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
           htmlFile << raw_class<< Map_Ampl[2][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[3][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[4][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[5][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[6][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[7][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;
           htmlFile << raw_class<< Map_Ampl[8][Sub[2][i]][Depth[2][i]]->GetBinContent(Eta[2][i]+41,Phi[2][i]+1)<<"</td>"<< std::endl;	 
	   htmlFile << "</tr>" << std::endl;

        ind+=1;
	}
     } 
     htmlFile << "</table>" << std::endl;
     htmlFile << "<br>"<< std::endl;
     
     htmlFile << "<h3> 2.C.List of Warning channels </h3>"<< std::endl;
     htmlFile << "<table>"<< std::endl;     
     
     htmlFile << "<tr>";
     htmlFile << "<td class=\"s4\" align=\"center\">#</td>"    << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ETA</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PHI</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DEPTH</td>"<< std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RBX</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PIXEL</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">FIBER_CH</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">QIE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ADC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">CRATE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DCC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">SPIGOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_SLOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FPGA</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Cm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Am</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Wm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Rm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">TNm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">TXm</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">GS</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Rc</td>"   << std::endl;
     htmlFile << "</tr>"   << std::endl;     
   
    for (int i=1;i<=NWarn;i++) {
        if((ind%2)==1){
           raw_class="<td class=\"s2\" align=\"center\">";
        }else{
           raw_class="<td class=\"s3\" align=\"center\">";
        }
        const CellDB db;
        CellDB ce;
	if ((ce.size()>=1)&&(Sub[1][i]==sub)) {
           if (Sub[1][i]==1) {
	      ce = db.find("subdet", "HB").find("Eta", Eta[1][i]).find("Phi", Phi[1][i]).find("Depth", Depth[1][i]);
	      if (ce.size()==0) {cout<<"Error: No such HB, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HB, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	      }
	   if (Sub[1][i]==2) {
	      ce = db.find("subdet", "HE").find("Eta", Eta[1][i]).find("Phi", Phi[1][i]).find("Depth", Depth[1][i]);
	      if (ce.size()==0) {cout<<"Error: No such HE, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HE, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}	   
	      }
	   if (Sub[1][i]==3) {
	      ce = db.find("subdet", "HO").find("Eta", Eta[1][i]).find("Phi", Phi[1][i]).find("Depth", Depth[1][i]);
	      if (ce.size()==0) {cout<<"Error: No such HO, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HO, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}	   
	      }	   
	   if (Sub[1][i]==4) {
	      ce = db.find("subdet", "HF").find("Eta", Eta[1][i]).find("Phi", Phi[1][i]).find("Depth", Depth[1][i]);
	      if (ce.size()==0) {cout<<"Error: No such HF, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	      else if (ce.size()>1) {cout<<"Warning: More than one line correspond to such HF, Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}	   
	      }	
           htmlFile << "<td class=\"s4\" align=\"center\">" << ind+1 <<"</td>"<< std::endl;
           htmlFile << raw_class<< Eta[1][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Phi[1][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Depth[1][i] <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RBX <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RM <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Pixel <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RMfiber <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberCh <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].QIE <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].ADC<<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].VMECardID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].dccID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Spigot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberIndex <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrSlot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrTB <<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[1][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
           htmlFile << raw_class<< Map_Ampl[2][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[3][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[4][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[5][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[6][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[7][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << raw_class<< Map_Ampl[8][Sub[1][i]][Depth[1][i]]->GetBinContent(Eta[1][i]+41,Phi[1][i]+1)<<"</td>"<< std::endl;
	   htmlFile << "</tr>" << std::endl;
           ind+=1;
	}
     } 
     htmlFile << "</table>" << std::endl;   
     htmlFile << "</body> " << std::endl;
     htmlFile << "</html> " << std::endl;
     htmlFile.close();
  }
      
//======================================================================




//======================================================================
// Creating main html file:   
     ofstream htmlFile;
     htmlFile.open("MAP.html");  
     htmlFile << "</html><html xmlns=\"http://www.w3.org/1999/xhtml\">"<< std::endl;
     htmlFile << "<head>"<< std::endl;
     htmlFile << "<meta http-equiv=\"Content-Type\" content=\"text/html\"/>"<< std::endl;
     htmlFile << "<title> Raw Data Analyser </title>"<< std::endl;
     htmlFile << "<style type=\"text/css\">"<< std::endl;
     htmlFile << " body,td{ background-color: #FFFFCC; font-family: arial, arial ce, helvetica; font-size: 12px; }"<< std::endl;
     htmlFile << "   td.s0 { font-family: arial, arial ce, helvetica; }"<< std::endl;
     htmlFile << "   td.s1 { font-family: arial, arial ce, helvetica; font-weight: bold; background-color: #FFC169; text-align: center;}"<< std::endl;
     htmlFile << "   td.s2 { font-family: arial, arial ce, helvetica; background-color: #eeeeee; }"<< std::endl;
     htmlFile << "   td.s3 { font-family: arial, arial ce, helvetica; background-color: #d0d0d0; }"<< std::endl;
     htmlFile << "   td.s4 { font-family: arial, arial ce, helvetica; background-color: #FFC169; }"<< std::endl;
     htmlFile << "</style>"<< std::endl;
     htmlFile << "<body>"<< std::endl;
     
     htmlFile << "<h1> Raw Data Analyser, RUN = "<< runnumber <<" </h1>"<< std::endl;
     htmlFile << "<br>"<< std::endl;
     
     htmlFile << "<h2> 1. Status of each subdetector separately </h2>"<< std::endl;   
     htmlFile << "<table width=\"400\">"<< std::endl;
     htmlFile << "<tr>"<< std::endl;
     htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HB.html\">HB</a></td>"<< std::endl;
     htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HE.html\">HE</a></td>"<< std::endl;
     htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HO.html\">HO</a></td>"<< std::endl;
     htmlFile << "  <td><a href=\"file:///afs/cern.ch/work/d/dtlisov/private/Monitoring/CMSSW_5_3_8/src/RecoHcal/HcalPromptAnalysis/test/HF.html\">HF</a></td>"<< std::endl;
     htmlFile << "</tr>"<< std::endl;
     htmlFile << "</table>"<< std::endl;
     htmlFile << "<br>"<< std::endl;    
    
    
     htmlFile << "<h2> 2. HCAL status over all test and subdetectors </h2>"<< std::endl;   
     htmlFile << "<h3> 2.A. Channel map </h3>"<< std::endl;
     htmlFile << "<h4> Channel legend: green - good, yelloy - warning, red - bad, white - not applicable or out of range </h4>"<< std::endl;
     htmlFile << " <img src=\"MAP.png\" />" << std::endl;      
     htmlFile << "<br>"<< std::endl;
     
     htmlFile << "<h3> 2.B. List of Bad channels </h3>"<< std::endl;
     htmlFile << "<table>"<< std::endl;     
     htmlFile << "<tr>";
     htmlFile << "<td class=\"s4\" align=\"center\">#</td>"    << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ETA</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PHI</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DEPTH</td>"<< std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RBX</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PIXEL</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">FIBER_CH</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">QIE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ADC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">CRATE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DCC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">SPIGOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_SLOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FPGA</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Comment</td>"   << std::endl;
     htmlFile << "</tr>"   << std::endl;     
   
     ind = 0;
     
     for (int i=1;i<=NBad;i++) {
        if((ind%2)==1){
           raw_class="<td class=\"s2\" align=\"center\">";
        }else{
           raw_class="<td class=\"s3\" align=\"center\">";
        }
        const CellDB db;
        const CellDB ce = db.find("Eta", Eta[2][i]).find("Phi", Phi[2][i]).find("Depth", Depth[2][i]);
             if (ce.size()==0) {cout<<"Error: No such Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
//	else if (ce.size()>1) { cout<<"Warning: More than one line correspond to such Eta="<< Eta[2][i] <<", Phi="<< Phi[2][i] <<", Depth="<< Depth[2][i] <<" in database"<<endl;}
	
	if (ce.size()>=1) {
	   htmlFile << "<tr>"<< std::endl;
           htmlFile << "<td class=\"s4\" align=\"center\">" << ind+1 <<"</td>"<< std::endl;
           htmlFile << raw_class<< Eta[2][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Phi[2][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Depth[2][i] <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RBX <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RM <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Pixel <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RMfiber <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberCh <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].QIE <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].ADC<<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].VMECardID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].dccID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Spigot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberIndex <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrSlot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrTB <<"</td>"<< std::endl;
           htmlFile << raw_class<< Comment[2][i]<<"</td>"<< std::endl;
	   htmlFile << "</tr>" << std::endl;

        ind+=1;
	}
     } 
     htmlFile << "</table>" << std::endl;
     htmlFile << "<br>"<< std::endl;
     
     htmlFile << "<h3> 2.C.List of Warning channels </h3>"<< std::endl;
     htmlFile << "<table>"<< std::endl;     
     htmlFile << "<tr>";
     htmlFile << "<td class=\"s4\" align=\"center\">#</td>"    << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ETA</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PHI</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DEPTH</td>"<< std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RBX</td>"  << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">PIXEL</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">RM_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">FIBER_CH</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">QIE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">ADC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">CRATE</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">DCC</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">SPIGOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FIBER</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_SLOT</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">HTR_FPGA</td>"   << std::endl;
     htmlFile << "<td class=\"s1\" align=\"center\">Comment</td>"   << std::endl;
     htmlFile << "</tr>"   << std::endl;     
   
     for (int i=1;i<=NWarn;i++) {
        if((ind%2)==1){
           raw_class="<td class=\"s2\" align=\"center\">";
        }else{
           raw_class="<td class=\"s3\" align=\"center\">";
        }
        const CellDB db;
        const CellDB ce = db.find("Eta", Eta[1][i]).find("Phi", Phi[1][i]).find("Depth", Depth[1][i]);
	    if (ce.size()==0) {cout<<"Error: No such Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
//	else if (ce.size()>1) { cout<<"Warning: More than one line correspond to such Eta="<< Eta[1][i] <<", Phi="<< Phi[1][i] <<", Depth="<< Depth[1][i] <<" in database"<<endl;}
	
	if (ce.size()>=1) {
	   htmlFile << "<tr>"<< std::endl;
           htmlFile << "<td class=\"s4\" align=\"center\">" << ind+1 <<"</td>"<< std::endl;
           htmlFile << raw_class<< Eta[1][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Phi[1][i]<<"</td>"<< std::endl;
           htmlFile << raw_class<< Depth[1][i] <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RBX <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RM <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Pixel <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].RMfiber <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberCh <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].QIE <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].ADC<<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].VMECardID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].dccID <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].Spigot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].FiberIndex <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrSlot <<"</td>"<< std::endl;
           htmlFile << raw_class<< ce[0].HtrTB <<"</td>"<< std::endl;
           htmlFile << raw_class<< Comment[1][i]<<"</td>"<< std::endl;
	   htmlFile << "</tr>" << std::endl;

           ind+=1;
	}
     } 
    
   
     htmlFile << "</table>" << std::endl;
     htmlFile << "</body> " << std::endl;
     htmlFile << "</html> " << std::endl; 
     htmlFile.close();  
//======================================================================
     
//======================================================================
// Close and delete all possible things:
    hfile->Close();    
//  hfile->Delete();
//  Exit Root
    gSystem->Exit(0);
//======================================================================

}


