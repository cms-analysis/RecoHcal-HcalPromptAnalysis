//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jan 29 16:30:48 2013 by ROOT version 5.34/04
// from TChain Hcal/
//////////////////////////////////////////////////////////

#ifndef Hcal_h
#define Hcal_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Hcal {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Nevent;
   Int_t           Run;
   Int_t           myCalEta[5][40];
   Int_t           myCalPhi[5][40];
   Float_t         hb_calibration0[40][15];
   Float_t         hb_calibration1[40][15];
   Float_t         he_calibration0[40][15];
   Float_t         he_calibration1[40][15];
   Float_t         hf_calibration0[40][15];
   Float_t         hf_calibration1[40][15];
   Float_t         ho_calibration0[40][15];
   Float_t         ho_calibration1[40][15];

   // List of branches
   TBranch        *b_Nevent;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_myCalEta;   //!
   TBranch        *b_myCalPhi;   //!
   TBranch        *b_hb_calibration0;   //!
   TBranch        *b_hb_calibration1;   //!
   TBranch        *b_he_calibration0;   //!
   TBranch        *b_he_calibration1;   //!
   TBranch        *b_hf_calibration0;   //!
   TBranch        *b_hf_calibration1;   //!
   TBranch        *b_ho_calibration0;   //!
   TBranch        *b_ho_calibration1;   //!

   Hcal(TTree *tree=0);
   virtual ~Hcal();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Hcal_cxx
Hcal::Hcal(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("Hcal",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("Hcal","");
      chain->Add("analysis_minbias_Full_211154.root/Hcal");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Hcal::~Hcal()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Hcal::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Hcal::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Hcal::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Nevent", &Nevent, &b_Nevent);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("myCalEta", myCalEta, &b_myCalEta);
   fChain->SetBranchAddress("myCalPhi", myCalPhi, &b_myCalPhi);
   fChain->SetBranchAddress("hb_calibration0", hb_calibration0, &b_hb_calibration0);
   fChain->SetBranchAddress("hb_calibration1", hb_calibration1, &b_hb_calibration1);
   fChain->SetBranchAddress("he_calibration0", he_calibration0, &b_he_calibration0);
   fChain->SetBranchAddress("he_calibration1", he_calibration1, &b_he_calibration1);
   fChain->SetBranchAddress("hf_calibration0", hf_calibration0, &b_hf_calibration0);
   fChain->SetBranchAddress("hf_calibration1", hf_calibration1, &b_hf_calibration1);
   fChain->SetBranchAddress("ho_calibration0", ho_calibration0, &b_ho_calibration0);
   fChain->SetBranchAddress("ho_calibration1", ho_calibration1, &b_ho_calibration1);
   Notify();
}

Bool_t Hcal::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Hcal::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Hcal::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Hcal_cxx
