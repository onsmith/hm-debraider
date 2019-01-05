#include "TDbrBinCABAC.h"


Void TDbrBinCABAC::setXmlWriter(TDbrXmlWriter* xmlWriter) {
  this->xmlWriter = xmlWriter;
}


TDbrXmlWriter* TDbrBinCABAC::setXmlWriter() {
  return xmlWriter;
}


Void TDbrBinCABAC::decodeBin(UInt& ruiBin, ContextModel& rcCtxModel) {
  TDecBinCABAC::decodeBin(ruiBin, rcCtxModel);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin", ruiBin);
}


Void TDbrBinCABAC::decodeBinEP(UInt& ruiBin) {
  TDecBinCABAC::decodeBinEP(ruiBin);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin-ep", ruiBin);
}


Void TDbrBinCABAC::decodeBinsEP(UInt& ruiBins, Int numBins) {
  TDecBinCABAC::decodeBinsEP(ruiBins, numBins);
  if (numBins > 0) {
    assert(xmlWriter != nullptr);
    xmlWriter->writeValueTag("bins-ep", ruiBins, numBins);
  }
}


Void TDbrBinCABAC::decodeBinTrm(UInt& ruiBin) {
  TDecBinCABAC::decodeBinTrm(ruiBin);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin-trm", ruiBin);
}


Void TDbrBinCABAC::xReadPCMCode(UInt uiLength, UInt& ruiCode) {
  TDecBinCABAC::xReadPCMCode(uiLength, ruiCode);
  if (uiLength > 0) {
    assert(xmlWriter != nullptr);
    xmlWriter->writeValueTag("pcm-code", ruiCode, uiLength);
  }
}
