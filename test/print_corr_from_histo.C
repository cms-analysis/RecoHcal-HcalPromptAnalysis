void print_corr_from_histo()
{
  gROOT->Reset();
  gStyle->SetPalette(1);
  gStyle                  -> SetOptFit(1111); 
  gStyle                  -> SetOptStat(111111); 


  char mychar[100];

  Int_t cancolor = 10; 
  TCanvas c2h("c2h","2-d options",10,10,800,600); 
  c2h.SetFillColor(cancolor);  
  gPad->SetDrawOption("e1p");
    
//
// Draw with error bars
//


  TH2F* hb[50];
  TH2F* he[50];
  TH2F* ho[50];
  TH2F* hf[50];
 
  TFile f("led_calibration.root");

    TIter nextHist(f.GetListOfKeys());
    TKey* histKey(0);
    Int_t i = 0;
    Int_t j = 0;
    Int_t k = 0;
    Int_t l = 0;
    
    while ((histKey=(TKey*)nextHist())) {

     TH1F* hrsp  = (TH1F*)histKey->ReadObj();  
     string histname(hrsp->GetName());

     cout<<histname<<endl; 

     if (histname.find("hb_")==0) {hb[i] = (TH2F*)histKey->ReadObj(); i++;}
     if (histname.find("he_")==0) {he[j] = (TH2F*)histKey->ReadObj();j++;}
     if (histname.find("ho_")==0) {ho[k] = (TH2F*)histKey->ReadObj(); k++;}
     if (histname.find("hf_")==0) {hf[l] = (TH2F*)histKey->ReadObj();l++;} 
    
    }


  TCanvas det("det","2-d options",10,10,800,600); 

   for(Int_t irbx=0; irbx<i; irbx++) {  
      sprintf(mychar,"GIFCALIB/hb_%d.gif",irbx);
      hb[irbx]->GetXaxis()->SetTitle("TS");
      hb[irbx]->GetYaxis()->SetTitle("fC");
      hb[irbx]->Draw("box");  det.Print(mychar);
   }

   for(Int_t irbx=0; irbx<j; irbx++) {
      sprintf(mychar,"GIFCALIB/he_%d.gif",irbx);
      he[irbx]->GetXaxis()->SetTitle("TS");
      he[irbx]->GetYaxis()->SetTitle("fC");
      he[irbx]->Draw("box");  det.Print(mychar);
   }

   for(Int_t irbx=0; irbx<k; irbx++) {
      sprintf(mychar,"GIFCALIB/ho_%d.gif",irbx);
      ho[irbx]->GetXaxis()->SetTitle("TS");
      ho[irbx]->GetYaxis()->SetTitle("fC");
      ho[irbx]->Draw("box");  det.Print(mychar);
   }
  
}
