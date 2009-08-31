#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#
#include "RecoHcal/HcalPromptAnalysis/interface/HcalZS.h"
#include "RecoHcal/HcalPromptAnalysis/interface/HcalChanMon.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(HcalZS);
DEFINE_ANOTHER_FWK_MODULE(HcalChanMon);
