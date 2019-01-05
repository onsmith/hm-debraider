#include "TDbrCavlc.h"


Void TDbrCavlc::setXmlWriter(TDbrXmlWriter* xmlWriter) {
  this->xmlWriter = xmlWriter;
}


TDbrXmlWriter* TDbrCavlc::setXmlWriter() {
  return xmlWriter;
}


Void TDbrCavlc::parseShortTermRefPicSet(TComSPS* sps, TComReferencePictureSet* rps, Int idx) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("short-term-ref-pic-set");
  TDecCavlc::parseShortTermRefPicSet(sps, rps, idx);
  xmlWriter->writeCloseTag("short-term-ref-pic-set");
}


Void TDbrCavlc::parseVPS(TComVPS* pcVPS) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("vps");
  TDecCavlc::parseVPS(pcVPS);
  xmlWriter->writeCloseTag("vps");
}


Void TDbrCavlc::parseSPS(TComSPS* pcSPS) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sps");
  TDecCavlc::parseSPS(pcSPS);
  xmlWriter->writeCloseTag("sps");
}


Void TDbrCavlc::parsePPS(TComPPS* pcPPS) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("pps");
  TDecCavlc::parsePPS(pcPPS);
  xmlWriter->writeCloseTag("pps");
}


Void TDbrCavlc::parseSliceHeader(TComSlice* pcSlice, ParameterSetManager* parameterSetManager, const Int prevTid0POC) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("slice-header");
  TDecCavlc::parseSliceHeader(pcSlice, parameterSetManager, prevTid0POC);
  xmlWriter->writeCloseTag("slice-header");
}


Void TDbrCavlc::parseTerminatingBit(UInt& ruilsLast) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("slice-terminating-bit");
  TDecCavlc::parseTerminatingBit(ruilsLast);
  xmlWriter->writeCloseTag("slice-terminating-bit");
}


Void TDbrCavlc::parseRemainingBytes(Bool noTrailingBytesExpected) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("slice-remaining-bytes");
  TDecCavlc::parseRemainingBytes(noTrailingBytesExpected);
  xmlWriter->writeCloseTag("slice-remaining-bytes");
}


Void TDbrCavlc::parseDeltaQP(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("delta-qp");
  TDecCavlc::parseDeltaQP(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("delta-qp");
}


Void TDbrCavlc::xReadCode(UInt length, UInt& val) {
  SyntaxElementParser::xReadCode(length, val);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("code", val, length);
}


Void TDbrCavlc::xReadUvlc(UInt& val) {
  SyntaxElementParser::xReadUvlc(val);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("uvlc", val);
}


Void TDbrCavlc::xReadSvlc(Int& val) {
  SyntaxElementParser::xReadSvlc(val);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("svlc", val);
}


Void TDbrCavlc::xReadFlag(UInt& val) {
  SyntaxElementParser::xReadFlag(val);
  assert(xmlWriter != nullptr);
  xmlWriter->writeValueTag("flag", val);
}


Void TDbrCavlc::xReadRbspTrailingBits() {
  xmlWriter->disableWriting();
  SyntaxElementParser::xReadRbspTrailingBits();
  xmlWriter->enableWriting();
  xmlWriter->writeValueTag("rsbp-trailing-bits", 1);
}
