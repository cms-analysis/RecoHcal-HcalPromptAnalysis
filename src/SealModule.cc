#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#
#include "RecoHcal/HcalPromptAnalysis/interface/HcalZS.h"
#include "RecoHcal/HcalPromptAnalysis/interface/HcalChanMon.h"
#include "RecoHcal/HcalPromptAnalysis/interface/HcalPFGAnalysisTemplate.h"
#include "RecoHcal/HcalPromptAnalysis/interface/JetTrigAnalyzer.h"
#include "RecoHcal/HcalPromptAnalysis/interface/AccessFEDinfo.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(HcalZS);
DEFINE_ANOTHER_FWK_MODULE(HcalChanMon);
DEFINE_ANOTHER_FWK_MODULE(HcalPFGAnalysisTemplate);
DEFINE_ANOTHER_FWK_MODULE(JetTrigAnalyzer);
DEFINE_ANOTHER_FWK_MODULE(AccessFEDinfo);
