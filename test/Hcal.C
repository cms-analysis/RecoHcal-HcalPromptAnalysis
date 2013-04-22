#define Hcal_cxx
#include "Hcal.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Hcal::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L Hcal.C
//      Root > Hcal t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   TH2F* hb[40];
   TH2F* he[40];   
   TH2F* ho[40];
   TH2F* hf[40]; 

//   TH2F* hb[40];
//   TH2F* he[40];   
//   TH2F* ho[40];
//   TH2F* hf[40]; 

   TH1F* hbp0=new TH1F("hbp0","hbp0",11,-0.5,10.5);
   TH1F* hbp1= new TH1F("hbp1","hbp1",11,-0.5,10.5); 

   TH1F* hbmax0= new TH1F("hbmax0","hbmax0",330,70.,200.);
   TH1F* hbmax1= new TH1F("hbmax1","hbmax1",330,70.,200.); 

   char myhist[60];
   for(Int_t i = 0; i<40; i++) {
     sprintf(myhist,"hb_%d",i);
     hb[i] = new TH2F(myhist,myhist,11,-0.5,10.5,1000, -0.5, 200.);
     sprintf(myhist,"he_%d",i);
     he[i] = new TH2F(myhist,myhist,11,-0.5,10.5,1000, -0.5, 200.);
     sprintf(myhist,"ho_%d",i);
     ho[i] = new TH2F(myhist,myhist,11,-0.5,10.5,1000, -0.5, 200.);
     sprintf(myhist,"hf_%d",i);
     hf[i] = new TH2F(myhist,myhist,11,-0.5,10.5,1000, -0.5, 200.);

   }

   Int_t maxhb=0;
   Int_t maxhe=0;
   Int_t maxho=0;
   Int_t maxhf=0;

   Int_t nev = 0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
// HB


        for(Int_t irbx=0; irbx<40; irbx++) {
        if( nev ==0) cout<<" RBX "<<irbx<<" "<<myCalEta[1][irbx]<<" "<<myCalPhi[1][irbx]<<endl;
          if(abs(myCalEta[1][irbx]) > 0) {for(Int_t j=0; j<10; j++) hb[irbx]->Fill((Float_t)j,hb_calibration1[irbx][j]-hb_calibration1[irbx][0]);
                                                  if(irbx==0) {hbp0->Fill(hb_calibration1[irbx][0]);hbmax0->Fill(hb_calibration1[irbx][2]);}
                                                  if(irbx==1) {hbp1->Fill(hb_calibration1[irbx][0]);hbmax1->Fill(hb_calibration1[irbx][2]);}
                                                  maxhb=irbx;}
          if(abs(myCalEta[2][irbx]) > 0) {for(Int_t j=0; j<10; j++) he[irbx]->Fill((Float_t)j,he_calibration1[irbx][j]-he_calibration1[irbx][0]);maxhe=irbx;}
          if(abs(myCalEta[3][irbx]) > 0) {for(Int_t j=0; j<10; j++) ho[irbx]->Fill((Float_t)j,ho_calibration1[irbx][j]-ho_calibration1[irbx][0]);maxho=irbx;}
        } // RBX
     nev++;
   } // event

   TFile f("led_calibration.root","recreate");
   hbp0->Write();
   hbp1->Write();
   hbmax0->Write();
   hbmax1->Write();    
   for(Int_t irbx=0; irbx<maxhb; irbx++) {
      hb[irbx]->Write();
   }

   for(Int_t irbx=0; irbx<maxhe; irbx++) {
      he[irbx]->Write();
   }

   for(Int_t irbx=0; irbx<maxho; irbx++) {
      ho[irbx]->Write();
   }

}
