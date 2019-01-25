#include "TDbrSbac.h"
#include "TLibCommon/TComTU.h"
#include "TLibEncoder/TEncEntropy.h"


Void TDbrSbac::setXmlWriter(TDbrXmlWriter* xmlWriter) {
  this->xmlWriter = xmlWriter;
}


TDbrXmlWriter* TDbrSbac::setXmlWriter() {
  return xmlWriter;
}


Void TDbrSbac::setCabacReader(TDbrBinCABAC* cabacReader) {
  this->cabacReader = cabacReader;
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
  xmlWriter->writeOpenTag("coeff");
  TDecSbac::parseCoeffNxN(rTu, compID);
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




/**
 * Gets the coded intra angle for a particular transform block
 */
Int getCodedIntraAngle(const TComTU &tu, const ComponentID component) {
  const TComDataCU& cu               = *tu.getCU();
  const UInt        uiAbsPartIdx     = tu.GetAbsPartIdxTU(component);
  const Bool        isLumaChannel    = isLuma(component);
  const Bool        isIntraPredicted = cu.isIntra(uiAbsPartIdx);

  // If the block isn't intra-coded, we can't get the intra mode
  if (!isIntraPredicted) {
    return -1;
  }

  // Get the coded intra mode
  Int intraAngle = cu.getIntraDir(toChannelType(component), uiAbsPartIdx);

  // If this is the luma channel, then we're done
  if (isLumaChannel) {
    return intraAngle;
  }

  // If we are borrowing from the luma channel, then get the luma coded mode
  if (intraAngle == DM_CHROMA_IDX) {
    const TComSlice& slice = *cu.getSlice();
    const TComSPS&   sps   = *slice.getSPS();
    const UInt partsPerMinCU =
      1 << (2 * (sps.getMaxTotalCUDepth() - sps.getLog2DiffMaxMinCodingBlockSize()));

    intraAngle = cu.getIntraDir(
      CHANNEL_TYPE_LUMA,
      getChromasCorrespondingPULumaIdx(uiAbsPartIdx, tu.GetChromaFormat(), partsPerMinCU)
    );
  }

  // If 4:2:2 chroma subsampling, adjust the intra angle
  if (tu.GetChromaFormat() == CHROMA_422) {
    intraAngle = g_chroma422IntraAngleMappingTable[intraAngle];
  }

  return intraAngle;
}




/**
 * Determines if a transform block is implicitly RDPCM-coded
 */
Bool isImplicitlyRDPCMCoded(const TComTU &tu, const ComponentID component) {
  const TComDataCU& cu = *tu.getCU();
  const UInt uiAbsPartIdx = tu.GetAbsPartIdxTU(component);
  const Bool isIntraPredicted = cu.isIntra(uiAbsPartIdx); 
  const Bool isTransformSkipped = cu.getTransformSkip(uiAbsPartIdx, component);

  if (isTransformSkipped && isIntraPredicted && cu.isRDPCMEnabled(uiAbsPartIdx)) {
    const Int intraMode = getCodedIntraAngle(tu, component);
    return (intraMode == HOR_IDX || intraMode == VER_IDX);
  } else {
    return false;
  }
}




Void TDbrSbac::codeCoeffNxN(TComTU &tu, TCoeff* pcCoef, const ComponentID compID) {
        TComDataCU&    cu           = *tu.getCU();
  const UInt           uiAbsPartIdx = tu.GetAbsPartIdxTU(compID);
  const TComRectangle& tuRect       = tu.getRect(compID);
  const UInt           tuWidth      = tuRect.width;
  const UInt           tuHeight     = tuRect.height;
  const TComSlice&     slice        = *cu.getSlice();
  const TComSPS&       sps          = *slice.getSPS();
  const TComPPS&       pps          = *slice.getPPS();


  DTRACE_CABAC_VL( g_nSymbolCounter++ )
  DTRACE_CABAC_T( "\tparseCoeffNxN()\teType=" )
  DTRACE_CABAC_V( compID )
  DTRACE_CABAC_T( "\twidth=" )
  DTRACE_CABAC_V( uiWidth )
  DTRACE_CABAC_T( "\theight=" )
  DTRACE_CABAC_V( uiHeight )
  DTRACE_CABAC_T( "\tdepth=" )
//  DTRACE_CABAC_V( rTu.GetTransformDepthTotalAdj(compID) )
  DTRACE_CABAC_V( rTu.GetTransformDepthTotal() )
  DTRACE_CABAC_T( "\tabspartidx=" )
  DTRACE_CABAC_V( uiAbsPartIdx )
  DTRACE_CABAC_T( "\ttoCU-X=" )
  DTRACE_CABAC_V( pcCU->getCUPelX() )
  DTRACE_CABAC_T( "\ttoCU-Y=" )
  DTRACE_CABAC_V( pcCU->getCUPelY() )
  DTRACE_CABAC_T( "\tCU-addr=" )
  DTRACE_CABAC_V(  pcCU->getCtuRsAddr() )
  DTRACE_CABAC_T( "\tinCU-X=" )
//  DTRACE_CABAC_V( g_auiRasterToPelX[ g_auiZscanToRaster[uiAbsPartIdx] ] )
  DTRACE_CABAC_V( g_auiRasterToPelX[ g_auiZscanToRaster[rTu.GetAbsPartIdxTU(compID)] ] )
  DTRACE_CABAC_T( "\tinCU-Y=" )
// DTRACE_CABAC_V( g_auiRasterToPelY[ g_auiZscanToRaster[uiAbsPartIdx] ] )
  DTRACE_CABAC_V( g_auiRasterToPelY[ g_auiZscanToRaster[rTu.GetAbsPartIdxTU(compID)] ] )
  DTRACE_CABAC_T( "\tpredmode=" )
  DTRACE_CABAC_V(  pcCU->getPredictionMode( uiAbsPartIdx ) )
  DTRACE_CABAC_T( "\n" )


  // Check TU size
  if (tuWidth > sps.getMaxTrSize()) {
    std::cerr << "ERROR: codeCoeffNxN was passed a TU with dimensions larger than the maximum allowed size" << std::endl;
    assert(false);
    exit(1);
  }


  // Count total number of significant coefficients
  const UInt totalNumSigCoeffs =
    TEncEntropy::countNonZeroCoeffs(pcCoef, tuWidth * tuHeight);
  if (totalNumSigCoeffs == 0) {
    std::cerr << "ERROR: codeCoeffNxN called for empty TU!" << std::endl;
    assert(false);
    exit(1);
  }


  // Prepare coding block parameters
  const UInt        tuWidthLog2            = g_aucConvertToBit[tuWidth] + 2;
  const UInt        tuHeightLog2           = g_aucConvertToBit[tuHeight] + 2;
  const ChannelType channelType            = toChannelType(compID);
  const Bool        isPrecisionExtended    = sps.getSpsRangeExtension().getExtendedPrecisionProcessingFlag();
  const Bool        alignCABACBeforeBypass = sps.getSpsRangeExtension().getCabacBypassAlignmentEnabledFlag();
  const Int         maxLog2TrDynamicRange  = sps.getMaxLog2TrDynamicRange(channelType);
        Bool        isSignHidingEnabled    = pps.getSignDataHidingEnabledFlag();


  // Code implicit RDPCM mode
  if (cu.getCUTransquantBypass(uiAbsPartIdx) || isImplicitlyRDPCMCoded(tu, compID)) {
    isSignHidingEnabled = false;
    if (!cu.isIntra(uiAbsPartIdx) && cu.isRDPCMEnabled(uiAbsPartIdx)) {
      codeExplicitRdpcmMode(tu, compID);
    }
  }


  // Code transform skip mode
  if (pps.getUseTransformSkip()) {
    codeTransformSkipFlags(tu, compID);
    if (cu.getTransformSkip(uiAbsPartIdx, compID) && !cu.isIntra(uiAbsPartIdx) && cu.isRDPCMEnabled(uiAbsPartIdx)) {
      //  This TU has coefficients and is transform skipped. Check whether is inter coded and if yes encode the explicit RDPCM mode
      codeExplicitRdpcmMode(tu, compID);

      //  Sign data hiding is force-disabled for horizontal and vertical explicit RDPCM modes
      if (cu.getExplicitRdpcmMode(compID, uiAbsPartIdx) != RDPCM_OFF) {
        isSignHidingEnabled = false;
      }
    }
  }


  // TODO: Define these variables
  const Bool  bUseGolombRiceParameterAdaptation = sps.getSpsRangeExtension().getPersistentRiceAdaptationEnabledFlag();
        UInt& currentGolombRiceStatistic        = m_golombRiceAdaptationStatistics[tu.getGolombRiceStatisticsIndex(compID)];


  // Get the scan iterator for this transform block
  TUEntropyCodingParameters codingParameters;
  getTUEntropyCodingParameters(codingParameters, tu, compID);


  // Tracks which 4x4 coefficient groups contain significant coefficients
  UInt isGroupSignificant[MLS_GRP_NUM];
  memset(isGroupSignificant, 0, sizeof(UInt) * MLS_GRP_NUM);

  // The index of the last significant coefficient in scan order
  Int lscScanOrderIndex = 0;
  
  // The index of the last significant coefficient in raster order
  Int lscRasterOrderIndex;

  // Scan through all coefficients to set up lscScanOrderIndex,
  //   lscRasterOrderIndex, and the isGroupSignificant array
  for (UInt coeffScanOrderIndex = 0, sigCoeffNum = 0; sigCoeffNum < totalNumSigCoeffs; coeffScanOrderIndex++) {
    const UInt coeffRasterOrderIndex = codingParameters.scan[coeffScanOrderIndex];

    if (pcCoef[coeffRasterOrderIndex] != 0) {
      UInt posY = coeffRasterOrderIndex >> tuWidthLog2;
      UInt posX = coeffRasterOrderIndex - (posY << tuWidthLog2);
      UInt cgIndex =
        codingParameters.widthInGroups * (posY >> MLS_CG_LOG2_HEIGHT) +
                                         (posX >> MLS_CG_LOG2_WIDTH);

      isGroupSignificant[cgIndex] = 1;
      sigCoeffNum++;
      lscScanOrderIndex   = coeffScanOrderIndex;
      lscRasterOrderIndex = coeffRasterOrderIndex;
    }
  }

  // First context group, containing DC coefficient, is always assumed to be
  //   significant
  isGroupSignificant[0] = 1;


  // Code position of last coefficient
  {
    Int posLastY = lscRasterOrderIndex >> tuWidthLog2;
    Int posLastX = lscRasterOrderIndex - (posLastY << tuWidthLog2);
    codeLastSignificantXY(
      posLastX,
      posLastY,
      tuWidth,
      tuHeight,
      compID,
      codingParameters.scanType
    );
  }


  // Prepare CABAC contexts
  ContextModel* const baseCoeffGroupContext =
    m_cCUSigCoeffGroupSCModel.get(0, channelType);
  ContextModel* const baseContext =
    m_cCUSigSCModel.get(0, 0) + getSignificanceMapContextOffset(compID);


  // The index of the group containing the last significant coefficient
  const Int lscgScanOrderIndex = lscScanOrderIndex >> MLS_CG_SIZE;

  // The index of the current coefficient in scan order
  Int coeffScanOrderIndex = lscScanOrderIndex;

  // Context state variable for signaling whether coefficient levels are greater
  //   than 1
  UInt gt1Context = 1;

  // Iterate over the 4x4 coefficient groups
  for (Int cgScanOrderIndex = lscgScanOrderIndex; cgScanOrderIndex >= 0; cgScanOrderIndex--) {
    UInt coeffSigns = 0;

    // Number of encountered significant coefficients in this coefficient group
    Int numSCsInCG = 0;

    // First significant coeffient index in the coefficient group
    Int firstSCInCGScanOrderIndex = 1 << MLS_CG_SIZE;

    // Last significant coefficient index in the coefficient group
    Int lastSCInCGScanOrderIndex = -1;

    // Array storing the absolute value coefficients
    Int  absCoeffs[1 << MLS_CG_SIZE];

    // Implicitly signal the last significant coefficient
    if (coeffScanOrderIndex == lscScanOrderIndex) {
      absCoeffs[0] = static_cast<Int>(abs(pcCoef[lscRasterOrderIndex]));
      coeffSigns = (pcCoef[lscRasterOrderIndex] < 0);
      firstSCInCGScanOrderIndex = coeffScanOrderIndex;
      lastSCInCGScanOrderIndex = coeffScanOrderIndex;
      numSCsInCG = 1;
      coeffScanOrderIndex--;
    }

    // Calculate the (x, y) location of the current coefficient group
    const Int cgRasterOrderIndex = codingParameters.scanCG[cgScanOrderIndex];
    const Int cgYPos = cgRasterOrderIndex / codingParameters.widthInGroups;
    const Int cgXPos = cgRasterOrderIndex - (cgYPos * codingParameters.widthInGroups);

    // Signal whether this coefficient group contains significant coefficients
    // Note: First and last coefficient groups are assumed to have significant
    //       coefficients and do not need to be signalled
    if (cgScanOrderIndex != lscgScanOrderIndex && cgScanOrderIndex != 0) {
      UInt isCGSig = (isGroupSignificant[cgRasterOrderIndex] ? 1 : 0);
      UInt sigCGContext = TComTrQuant::getSigCoeffGroupCtxInc(
        isGroupSignificant,
        cgXPos,
        cgYPos,
        codingParameters.widthInGroups,
        codingParameters.heightInGroups
      );
      m_pcBinIf->encodeBin(isCGSig, baseCoeffGroupContext[sigCGContext]);
    }

    // Signal whether each coefficient in the 4x4 group is significant
    //   (significant_coeff_flag)
    {
      Int firstCoeffInCGScanOrderIndex = cgScanOrderIndex << MLS_CG_SIZE;
      if (isGroupSignificant[cgRasterOrderIndex]) {
        const Int sigCoeffContext = TComTrQuant::calcPatternSigCtx(
          isGroupSignificant,
          cgXPos,
          cgYPos,
          codingParameters.widthInGroups,
          codingParameters.heightInGroups
        );

        // Loop through all coefficients in the 4x4 block
        for ( ; coeffScanOrderIndex >= firstCoeffInCGScanOrderIndex; coeffScanOrderIndex--) {
          UInt coeffRasterOrderIndex = codingParameters.scan[coeffScanOrderIndex];
          UInt isCoeffSigFlag = (pcCoef[coeffRasterOrderIndex] ? 1 : 0);

          // Encode significant coefficient flag
          // Note: Don't need to signal significance for the last coefficient in
          //   a coefficient block if no significant coefficients have been seen
          //   yet but we know one exists (except for DC coefficient)
          if (coeffScanOrderIndex != firstCoeffInCGScanOrderIndex || cgScanOrderIndex == 0 || numSCsInCG != 0) {
            UInt uiCtxSig = TComTrQuant::getSigCtxInc(
              sigCoeffContext,
              codingParameters,
              coeffScanOrderIndex,
              tuWidthLog2,
              tuHeightLog2,
              channelType
            );
            m_pcBinIf->encodeBin(isCoeffSigFlag, baseContext[uiCtxSig]);
          }

          // Store significant coefficients to encode during later scans
          if (isCoeffSigFlag) {
            absCoeffs[numSCsInCG] =
              static_cast<Int>(abs(pcCoef[coeffRasterOrderIndex]));
            numSCsInCG++;
            coeffSigns = (coeffSigns << 1) + (pcCoef[coeffRasterOrderIndex] < 0 ? 1 : 0);
            if (lastSCInCGScanOrderIndex == -1) {
              lastSCInCGScanOrderIndex = coeffScanOrderIndex;
            }
            firstSCInCGScanOrderIndex = coeffScanOrderIndex;
          }
        }
      } else {
        coeffScanOrderIndex = firstCoeffInCGScanOrderIndex - 1;
      }
    }

    // Nothing left to signal for this group if there there are no significant
    //   coefficients in it
    if (numSCsInCG == 0) {
      continue;
    }

    // Can only employ sign hiding if enough coefficients are in the block
    const Bool canHideSign =
      (lastSCInCGScanOrderIndex - firstSCInCGScanOrderIndex >= SBH_THRESHOLD);

    // Setup CABAC contexts
    const UInt contextSet = getContextSetIndex(compID, cgScanOrderIndex, gt1Context == 0);
    ContextModel* baseContextModel = m_cCUOneSCModel.get(0, 0) + (NUM_ONE_FLAG_CTX_PER_SET * contextSet);
    gt1Context = 1;

    // Set Golomb-Rice parameter
    UInt golombRiceParam            = currentGolombRiceStatistic / RExt__GOLOMB_RICE_INCREMENT_DIVISOR;
    Bool updateGolombRiceStatistics = bUseGolombRiceParameterAdaptation; // leave the statistics at 0 when not using the adaptation system

    // Number of coded greater-than-1 flags
    const Int numGT1FlagsInCG = min(numSCsInCG, C1FLAG_NUMBER);

    // True if there are > 8 significant coefficients in the group
    const Bool hasMoreThan8SigCoeffs = (numSCsInCG > C1FLAG_NUMBER);
    
    // The index of the first coefficient with a level greater than 1
    Int firstGT1CoeffIndex = -1;

    // Signal whether the first eight significant coefficients in the 4x4 group
    //   are greater than 1 (coeff_abs_level_greater1_flag)
    Bool hasMoreThan1GT1Coeff = false;
    for (Int i = 0; i < numGT1FlagsInCG; i++) {
      const Bool isCoeffGT1 = (absCoeffs[i] > 1);
      m_pcBinIf->encodeBin(isCoeffGT1 ? 1 : 0, baseContextModel[gt1Context]);
      if (isCoeffGT1) {
        gt1Context = 0;

        if (firstGT1CoeffIndex == -1) {
          firstGT1CoeffIndex = i;
        } else {
          hasMoreThan1GT1Coeff = true;
        }
      } else if (0 < gt1Context && gt1Context < 3) {
        gt1Context++;
      }
    }

    // Signal whether the first greater-than-1 coefficient in the 4x4 group is
    //   greater than 2 (coeff_abs_level_greater2_flag)
    Bool hasMoreThan0GT2Coeff = false;
    if (gt1Context == 0 && firstGT1CoeffIndex != -1) {
      baseContextModel = m_cCUAbsSCModel.get(0, 0) + (NUM_ABS_FLAG_CTX_PER_SET * contextSet);
      const Bool isCoeffGT2 = (absCoeffs[firstGT1CoeffIndex] > 2);
      m_pcBinIf->encodeBin(isCoeffGT2 ? 1 : 0, baseContextModel[0]);
      if (isCoeffGT2) {
        hasMoreThan0GT2Coeff = true;
      }
    }

    // True if this group contains remaining absolute level data
    const Bool hasRemainingAbsoluteLevels =
      (hasMoreThan8SigCoeffs || hasMoreThan1GT1Coeff || hasMoreThan0GT2Coeff);

    // Align bins before coding equiprobable bits
    if (hasRemainingAbsoluteLevels && alignCABACBeforeBypass) {
      m_pcBinIf->align();
    }

    // Signal sign bits (coeff_sign_flag)
    if (isSignHidingEnabled && canHideSign) {
      m_pcBinIf->encodeBinsEP(coeffSigns >> 1, numSCsInCG - 1);
    } else {
      m_pcBinIf->encodeBinsEP(coeffSigns, numSCsInCG);
    }

    // Signal remaining coefficient level data (coeff_abs_level_remaining)
    Int gt2CoeffAddin = 1;
    if (hasRemainingAbsoluteLevels) {
      for (Int sigCoeffIndex = 0; sigCoeffIndex < numSCsInCG; sigCoeffIndex++) {
        UInt baseLevel = (sigCoeffIndex < C1FLAG_NUMBER) ? (2 + gt2CoeffAddin) : 1;

        if (absCoeffs[sigCoeffIndex] >= baseLevel) {
          const UInt escapeCodeValue = absCoeffs[sigCoeffIndex] - baseLevel;

          // Write Golomb code
          xWriteCoefRemainExGolomb(
            escapeCodeValue,
            golombRiceParam,
            isPrecisionExtended,
            maxLog2TrDynamicRange
          );

          // Adjust Golomb-Rice parameter
          if (absCoeffs[sigCoeffIndex] > (3 << golombRiceParam)) {
            golombRiceParam =
              bUseGolombRiceParameterAdaptation ?
              golombRiceParam + 1 :
              std::min<UInt>(golombRiceParam + 1, 4);
          }

          // Update Golumb-Rice parameter statistics
          if (updateGolombRiceStatistics) {
            const UInt initialGolombRiceParameter = currentGolombRiceStatistic / RExt__GOLOMB_RICE_INCREMENT_DIVISOR;

            if (escapeCodeValue >= (3 << initialGolombRiceParameter)) {
              currentGolombRiceStatistic++;
            } else if (2*escapeCodeValue < (1 << initialGolombRiceParameter) && currentGolombRiceStatistic > 0) {
              currentGolombRiceStatistic--;
            }

            updateGolombRiceStatistics = false;
          }
        }

        // Only adjust baseLevel one time, for a single gt1 coefficient
        if (absCoeffs[sigCoeffIndex] >= 2) {
          gt2CoeffAddin = 0;
        }
      }
    }
  }
}
