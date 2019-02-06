#include "XmlBinEncoder.h"


TDbrXmlWriter* XmlBinEncoder::getXmlWriter() {
  return xmlWriter;
}


Void XmlBinEncoder::setXmlWriter(TDbrXmlWriter* writer) {
  xmlWriter = writer;
}


Void XmlBinEncoder::init(TComBitIf* pcTComBitIf) {
  // intentionally empty
}


Void XmlBinEncoder::uninit() {
  // intentionally empty
}


Void XmlBinEncoder::start() {
  // intentionally empty
}


Void XmlBinEncoder::finish() {
  // intentionally empty
}


Void XmlBinEncoder::copyState(const TEncBinIf* pcTEncBinIf) {
  // intentionally empty
}


Void XmlBinEncoder::flush() {
  // intentionally empty
}


Void XmlBinEncoder::resetBac() {
  // intentionally empty
}


Void XmlBinEncoder::encodePCMAlignBits() {
  // intentionally empty
}


Void XmlBinEncoder::xWritePCMCode(UInt uiCode, UInt uiLength) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("pcm-code", uiCode, uiLength);
}


Void XmlBinEncoder::resetBits() {
  // intentionally empty
}


UInt XmlBinEncoder::getNumWrittenBits() {
  assert(0);
  return 0;
}


Void XmlBinEncoder::encodeBin(UInt uiBin, ContextModel& rcCtxModel) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin", uiBin);
}


Void XmlBinEncoder::encodeBinEP(UInt uiBin) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin-ep", uiBin);
}


Void XmlBinEncoder::encodeBinsEP(UInt uiBins, Int numBins) {
  assert(xmlWriter != nullptr);
  if (numBins > 0) {
    xmlWriter->writeValueTag("bins-ep", uiBins, numBins);
  }
}


Void XmlBinEncoder::encodeBinTrm(UInt uiBin) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("bin-trm", uiBin);
}


Void XmlBinEncoder::align() {
  // intentionally empty
}


TEncBinCABAC* XmlBinEncoder::getTEncBinCABAC() {
  assert(0);
  return nullptr;
}


const TEncBinCABAC* XmlBinEncoder::getTEncBinCABAC() const {
  assert(0);
  return nullptr;
}
