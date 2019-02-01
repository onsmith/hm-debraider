#pragma once

#include "TDbrXmlWriter.h"

#include "TLibEncoder/TEncBinCoder.h"


class XmlBinEncoder : public TEncBinIf {
protected:
  TDbrXmlWriter* xmlWriter;


public:
  Void setXmlWriter(TDbrXmlWriter* writer);

  Void init(TComBitIf* pcTComBitIf);
  Void uninit();

  Void start();
  Void finish();
  Void copyState(const TEncBinIf* pcTEncBinIf);
  Void flush();

  Void resetBac();
  Void encodePCMAlignBits();
  Void xWritePCMCode(UInt uiCode, UInt uiLength);

  Void resetBits();
  UInt getNumWrittenBits();

  Void encodeBin(UInt uiBin, ContextModel& rcCtxModel);
  Void encodeBinEP(UInt uiBin);
  Void encodeBinsEP(UInt uiBins, Int numBins);
  Void encodeBinTrm(UInt uiBin);

  Void align();

        TEncBinCABAC* getTEncBinCABAC();
  const TEncBinCABAC* getTEncBinCABAC() const;
};
