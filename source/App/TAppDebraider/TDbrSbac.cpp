#include "TDbrSbac.h"
#include "TLibCommon/TComTU.h"
#include "TLibEncoder/TEncEntropy.h"


Void TDbrSbac::setXmlWriter(TDbrXmlWriter* xmlWriter) {
  this->xmlWriter = xmlWriter;
  coefficientEncoder.setXmlWriter(xmlWriter);
}


TDbrXmlWriter* TDbrSbac::getXmlWriter() {
  return xmlWriter;
}


Void TDbrSbac::setCabacReader(TDbrBinCABAC* cabacReader) {
  this->cabacReader = cabacReader;
}


Void TDbrSbac::setNumLayers(Int numLayers) {
  coefficientEncoder.setNumLayers(numLayers);
}

Int64 TDbrSbac::getNumLayeredBits() const {
  return coefficientEncoder.getNumLayeredBits();
}


Void TDbrSbac::parseTerminatingBit(UInt& ruilsLast) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("terminating-bit");
  TDecSbac::parseTerminatingBit(ruilsLast);
  xmlWriter->writeCloseTag("terminating-bit");
}


Void TDbrSbac::parseRemainingBytes(Bool noTrailingBytesExpected) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("remaining-bytes");
  TDecSbac::parseRemainingBytes(noTrailingBytesExpected);
  xmlWriter->writeCloseTag("remaining-bytes");
}


Void TDbrSbac::parseMVPIdx(Int& riMVPIdx) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("mv-pred-index");
  TDecSbac::parseMVPIdx(riMVPIdx);
  xmlWriter->writeCloseTag("mv-pred-index");
}


Void TDbrSbac::parseSkipFlag(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("skip-flag");
  TDecSbac::parseSkipFlag(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("skip-flag");
}


Void TDbrSbac::parseCUTransquantBypassFlag(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("tq-bypass-flag");
  TDecSbac::parseCUTransquantBypassFlag(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("tq-bypass-flag");
}


Void TDbrSbac::parseSplitFlag(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("split-flag");
  TDecSbac::parseSplitFlag(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("split-flag");
}


Void TDbrSbac::parseMergeFlag(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, UInt uiPUIdx) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("merge-flag");
  TDecSbac::parseMergeFlag(pcCU, uiAbsPartIdx, uiDepth, uiPUIdx);
  xmlWriter->writeCloseTag("merge-flag");
}


Void TDbrSbac::parseMergeIndex(TComDataCU* pcCU, UInt& ruiMergeIndex) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("merge-index");
  TDecSbac::parseMergeIndex(pcCU, ruiMergeIndex);
  xmlWriter->writeCloseTag("merge-index");
}


Void TDbrSbac::parsePartSize(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("part-size");
  TDecSbac::parsePartSize(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("part-size");
}


Void TDbrSbac::parsePredMode(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("pred-mode");
  TDecSbac::parsePredMode(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("pred-mode");
}


Void TDbrSbac::parseIntraDirLumaAng(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("intra-dir-luma");
  TDecSbac::parseIntraDirLumaAng(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("intra-dir-luma");
}


Void TDbrSbac::parseIntraDirChroma(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("intra-dir-chroma");
  TDecSbac::parseIntraDirChroma(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("intra-dir-chroma");
}


Void TDbrSbac::parseInterDir(TComDataCU* pcCU, UInt& ruiInterDir, UInt uiAbsPartIdx) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("inter-dir");
  TDecSbac::parseInterDir(pcCU, ruiInterDir, uiAbsPartIdx);
  xmlWriter->writeCloseTag("inter-dir");
}


Void TDbrSbac::parseRefFrmIdx(TComDataCU* pcCU, Int& riRefFrmIdx, RefPicList eRefList) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("reference-frame-index");
  TDecSbac::parseRefFrmIdx(pcCU, riRefFrmIdx, eRefList);
  xmlWriter->writeCloseTag("reference-frame-index");
}


Void TDbrSbac::parseMvd(TComDataCU* pcCU, UInt uiAbsPartAddr, UInt uiPartIdx, UInt uiDepth, RefPicList eRefList) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("mv-delta");
  TDecSbac::parseMvd(pcCU, uiAbsPartAddr, uiPartIdx, uiDepth, eRefList);
  xmlWriter->writeCloseTag("mv-delta");
}


Void TDbrSbac::parseCrossComponentPrediction(TComTU& rTu, ComponentID compID) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("cross-comp-pred");
  TDecSbac::parseCrossComponentPrediction(rTu, compID);
  xmlWriter->writeCloseTag("cross-comp-pred");
}


Void TDbrSbac::parseTransformSubdivFlag(UInt& ruiSubdivFlag, UInt uiLog2TransformBlockSize) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("tu-subdiv-flag");
  TDecSbac::parseTransformSubdivFlag(ruiSubdivFlag, uiLog2TransformBlockSize);
  xmlWriter->writeCloseTag("tu-subdiv-flag");
}


Void TDbrSbac::parseQtCbf(TComTU& rTu, const ComponentID compID, const Bool lowestLevel) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("qt-cbf");
  TDecSbac::parseQtCbf(rTu, compID, lowestLevel);
  xmlWriter->writeCloseTag("qt-cbf");
}


Void TDbrSbac::parseQtRootCbf(UInt uiAbsPartIdx, UInt& uiQtRootCbf) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("qt-root-cbf");
  TDecSbac::parseQtRootCbf(uiAbsPartIdx, uiQtRootCbf);
  xmlWriter->writeCloseTag("qt-root-cbf");
}


Void TDbrSbac::parseDeltaQP(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("delta-qp");
  TDecSbac::parseDeltaQP(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("delta-qp");
}


Void TDbrSbac::parseChromaQpAdjustment(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("chroma-qp-adj");
  TDecSbac::parseChromaQpAdjustment(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("chroma-qp-adj");
}


Void TDbrSbac::parseIPCMInfo(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("intra-pcm");
  TDecSbac::parseIPCMInfo(pcCU, uiAbsPartIdx, uiDepth);
  xmlWriter->writeCloseTag("intra-pcm");
}


Void TDbrSbac::parseCoeffNxN(TComTU& rTu, ComponentID compID) {
  assert(xmlWriter != nullptr);

  xmlWriter->disableWriting();
  TDecSbac::parseCoeffNxN(rTu, compID);
  xmlWriter->enableWriting();

  xmlWriter->writeOpenTag("coeff");
  coefficientEncoder.codeCoeffNxN(rTu, compID);
  xmlWriter->writeCloseTag("coeff");
}


Void TDbrSbac::parseTransformSkipFlags(TComTU& rTu, ComponentID component) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("tu-skip-flag");
  TDecSbac::parseTransformSkipFlags(rTu, component);
  xmlWriter->writeCloseTag("tu-skip-flag");
}


Void TDbrSbac::parseExplicitRdpcmMode(TComTU& rTu, ComponentID compID) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("rd-pcm");
  TDecSbac::parseExplicitRdpcmMode(rTu, compID);
  xmlWriter->writeCloseTag("rd-pcm");
}


Void TDbrSbac::parseSaoMaxUvlc(UInt& val, UInt maxSymbol) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-max-uvlc");
  TDecSbac::parseSaoMaxUvlc(val, maxSymbol);
  xmlWriter->writeCloseTag("sao-max-uvlc");
}


Void TDbrSbac::parseSaoMerge(UInt& ruiVal) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-merge");
  TDecSbac::parseSaoMerge(ruiVal);
  xmlWriter->writeCloseTag("sao-merge");
}


Void TDbrSbac::parseSaoTypeIdx(UInt& ruiVal) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-type-index");
  TDecSbac::parseSaoTypeIdx(ruiVal);
  xmlWriter->writeCloseTag("sao-type-index");
}


Void TDbrSbac::parseSaoUflc(UInt uiLength, UInt& ruiVal) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-uflc");
  TDecSbac::parseSaoUflc(uiLength, ruiVal);
  xmlWriter->writeCloseTag("sao-uflc");
}


Void TDbrSbac::parseSAOBlkParam(SAOBlkParam& saoBlkParam, Bool* sliceEnabled, Bool leftMergeAvail, Bool aboveMergeAvail, const BitDepths& bitDepths) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-block-params");
  TDecSbac::parseSAOBlkParam(saoBlkParam, sliceEnabled, leftMergeAvail, aboveMergeAvail, bitDepths);
  xmlWriter->writeCloseTag("sao-block-params");
}


Void TDbrSbac::parseSaoSign(UInt& val) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("sao-sign");
  TDecSbac::parseSaoSign(val);
  xmlWriter->writeCloseTag("sao-sign");
}


Void TDbrSbac::parseLastSignificantXY(UInt& uiPosLastX, UInt& uiPosLastY, Int width, Int height, ComponentID component, UInt uiScanIdx) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("last-sig-xy");
  TDecSbac::parseLastSignificantXY(uiPosLastX, uiPosLastY, width, height, component, uiScanIdx);
  xmlWriter->writeCloseTag("last-sig-xy");
}


Void TDbrSbac::parseScalingList(TComScalingList* scalingList) {
  assert(xmlWriter != nullptr);
  xmlWriter->writeOpenTag("scaling-list");
  TDecSbac::parseScalingList(scalingList);
  xmlWriter->writeCloseTag("scaling-list");
}


Void TDbrSbac::resetEntropy(TComSlice* pSlice) {
  TDecSbac::resetEntropy(pSlice);
  coefficientEncoder.resetEntropy(pSlice);
}
