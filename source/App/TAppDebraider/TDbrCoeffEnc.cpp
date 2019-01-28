#include "TDbrCoeffEnc.h"

#include "TLibCommon/TComSlice.h"
#include "TLibCommon/TComDataCu.h"
#include "TLibEncoder/TEncEntropy.h"



TDbrLayer::TDbrLayer() :
  numContextModels(0),
  m_cCUSigCoeffGroupSCModel (1, 2,                      NUM_SIG_CG_FLAG_CTX,         contextModels + numContextModels, numContextModels),
  m_cCUSigSCModel           (1, 1,                      NUM_SIG_FLAG_CTX,            contextModels + numContextModels, numContextModels),
  m_cCuCtxLastX             (1, NUM_CTX_LAST_FLAG_SETS, NUM_CTX_LAST_FLAG_XY,        contextModels + numContextModels, numContextModels),
  m_cCuCtxLastY             (1, NUM_CTX_LAST_FLAG_SETS, NUM_CTX_LAST_FLAG_XY,        contextModels + numContextModels, numContextModels),
  m_cCUOneSCModel           (1, 1,                      NUM_ONE_FLAG_CTX,            contextModels + numContextModels, numContextModels),
  m_cCUAbsSCModel           (1, 1,                      NUM_ABS_FLAG_CTX,            contextModels + numContextModels, numContextModels),
  m_cTransformSkipSCModel   (1, MAX_NUM_CHANNEL_TYPE,   NUM_TRANSFORMSKIP_FLAG_CTX,  contextModels + numContextModels, numContextModels),
  m_explicitRdpcmFlagSCModel(1, MAX_NUM_CHANNEL_TYPE,   NUM_EXPLICIT_RDPCM_FLAG_CTX, contextModels + numContextModels, numContextModels),
  m_explicitRdpcmDirSCModel (1, MAX_NUM_CHANNEL_TYPE,   NUM_EXPLICIT_RDPCM_DIR_CTX,  contextModels + numContextModels, numContextModels) {
  cabacEncoder.init(&bitstream);
}



Void TDbrLayer::encodeBin(UInt value, ContextModel& context) {
  cabacEncoder.encodeBin(value, context);
}



TDbrCoeffEnc::TDbrCoeffEnc(size_t numLayers) :
  layers(numLayers) {
}



size_t TDbrCoeffEnc::numLayers() const {
  return layers.size();
}



Void TDbrCoeffEnc::addLayer() {
  layers.push_back(TDbrLayer());
}



/**
 * Gets the coded intra angle for a particular transform block
 */
static Int getCodedIntraAngle(const TComTU &tu, const ComponentID component) {
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
static Bool isImplicitlyRDPCMCoded(const TComTU &tu, const ComponentID component) {
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



Void TDbrCoeffEnc::codeCoeffNxN(TComTU &tu, TCoeff* pcCoef, const ComponentID compID) {
        TComDataCU&    cu          = *tu.getCU();
  const UInt           tuPartIndex = tu.GetAbsPartIdxTU(compID);
  const TComRectangle& tuRect      = tu.getRect(compID);
  const UInt           tuWidth     = tuRect.width;
  const UInt           tuHeight    = tuRect.height;
  const TComSlice&     slice       = *cu.getSlice();
  const TComSPS&       sps         = *slice.getSPS();
  const TComPPS&       pps         = *slice.getPPS();


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
  if (cu.getCUTransquantBypass(tuPartIndex) || isImplicitlyRDPCMCoded(tu, compID)) {
    isSignHidingEnabled = false;
    if (!cu.isIntra(tuPartIndex) && cu.isRDPCMEnabled(tuPartIndex)) {
      codeExplicitRdpcmMode(tu, compID);
    }
  }


  // Code transform skip mode
  if (pps.getUseTransformSkip()) {
    xCodeTransformSkipFlag(tu, compID);
    if (cu.getTransformSkip(tuPartIndex, compID) && !cu.isIntra(tuPartIndex) && cu.isRDPCMEnabled(tuPartIndex)) {
      //  This TU has coefficients and is transform skipped. Check whether is inter coded and if yes encode the explicit RDPCM mode
      codeExplicitRdpcmMode(tu, compID);

      //  Sign data hiding is force-disabled for horizontal and vertical explicit RDPCM modes
      if (cu.getExplicitRdpcmMode(compID, tuPartIndex) != RDPCM_OFF) {
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
    xCodeLastSignificantXY(
      posLastX,
      posLastY,
      tuWidth,
      tuHeight,
      compID,
      codingParameters.scanType
    );
  }


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
      const UInt contextOffset = TComTrQuant::getSigCoeffGroupCtxInc(
        isGroupSignificant,
        cgXPos,
        cgYPos,
        codingParameters.widthInGroups,
        codingParameters.heightInGroups
      );

      xCodeSigGroupFlag(
        isGroupSignificant[cgRasterOrderIndex],
        contextOffset,
        channelType
      );
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
            const UInt contextOffset = TComTrQuant::getSigCtxInc(
              sigCoeffContext,
              codingParameters,
              coeffScanOrderIndex,
              tuWidthLog2,
              tuHeightLog2,
              channelType
            );

            xCodeSigCoeffFlag(
              isGroupSignificant[cgRasterOrderIndex],
              contextOffset + getSignificanceMapContextOffset(compID)
            );
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



/**
 * Encode (X,Y) position of the last significant coefficient
 * 
 * \param xCoord     X component of last coefficient
 * \param yCoord     Y component of last coefficient
 * \param width      Block width
 * \param height     Block height
 * \param component  chroma component ID
 * \param scanIndex  scan type (zig-zag, hor, ver)
 * 
 * This method encodes the X and Y component within a block of the last significant coefficient.
 */
Void TDbrCoeffEnc::xCodeLastSignificantXY(UInt xCoord, UInt yCoord, Int width, Int height, ComponentID component, UInt scanIndex) {
  if (scanIndex == SCAN_VER) {
    swap(xCoord, yCoord);
    swap(width,  height);
  }

  const UInt xCoordGroup = g_uiGroupIdx[xCoord];
  const UInt yCoordGroup = g_uiGroupIdx[yCoord];

  ContextModel* xCoordContext = m_cCuCtxLastX.get(0, toChannelType(component));
  ContextModel* yCoordContext = m_cCuCtxLastY.get(0, toChannelType(component));

  Int blkSizeOffsetX, blkSizeOffsetY, shiftX, shiftY;
  getLastSignificantContextParameters(
    component,
    width,
    height,
    blkSizeOffsetX,
    blkSizeOffsetY,
    shiftX,
    shiftY
  );


  // Unary-code group x coordinate
  {
    UInt lastContext;
    for (lastContext = 0; lastContext < xCoordGroup; lastContext++) {
      m_pcBinIf->encodeBin(1, *(xCoordContext + blkSizeOffsetX + (lastContext >> shiftX)));
    }

    if (xCoordGroup < g_uiGroupIdx[width - 1]) {
      m_pcBinIf->encodeBin(0, *(xCoordContext + blkSizeOffsetX + (lastContext >> shiftX)));
    }
  }


  // Unary-code group y coordinate
  {
    UInt lastContext;
    for (lastContext = 0; lastContext < yCoordGroup; lastContext++) {
      m_pcBinIf->encodeBin(1, *(yCoordContext + blkSizeOffsetY + (lastContext >> shiftY)));
    }

    if (yCoordGroup < g_uiGroupIdx[height - 1]) {
      m_pcBinIf->encodeBin(0, *(yCoordContext + blkSizeOffsetY + (lastContext >> shiftY)));
    }
  }


  // Code equiprobable x coordinate part
  if (xCoordGroup > 3) {
    UInt numBins = (xCoordGroup - 2) >> 1;
    xCoord = xCoord - g_uiMinInGroup[xCoordGroup];

    for (Int i = numBins - 1; i >= 0; i--) {
      m_pcBinIf->encodeBinEP((xCoord >> i) & 1);
    }
  }

  
  // Code equiprobable y coordinate part
  if (yCoordGroup > 3) {
    UInt numBins = (yCoordGroup - 2) >> 1;
    yCoord = yCoord - g_uiMinInGroup[yCoordGroup];

    for (Int i = numBins - 1 ; i >= 0; i--) {
      m_pcBinIf->encodeBinEP((yCoord >> i) & 1);
    }
  }
}



/**
 * Codes coeff_abs_level_minus3
 * \param symbol                  value of coeff_abs_level_minus3
 * \param rParam                  reference to Rice parameter
 * \param useLimitedPrefixLength
 * \param maxLog2TrDynamicRange 
 */
Void TDbrCoeffEnc::xWriteCoefRemainExGolomb(UInt symbol, UInt &rParam, const Bool useLimitedPrefixLength, const Int maxLog2TrDynamicRange) {
  Int  codeNumber = static_cast<Int>(symbol);


  if (codeNumber < (COEF_REMAIN_BIN_REDUCTION << rParam)) {
    UInt length = codeNumber >> rParam;
    m_pcBinIf->encodeBinsEP((1 << (length + 1)) - 2, length + 1);
    m_pcBinIf->encodeBinsEP((codeNumber % (1 << rParam)), rParam);


  } else if (useLimitedPrefixLength) {
    const UInt maximumPrefixLength =
      (32 - (COEF_REMAIN_BIN_REDUCTION + maxLog2TrDynamicRange));

    UInt prefixLength = 0;
    UInt suffixLength = MAX_UINT;
    UInt codeValue    = (symbol >> rParam) - COEF_REMAIN_BIN_REDUCTION;

    if (codeValue >= ((1 << maximumPrefixLength) - 1)) {
      prefixLength = maximumPrefixLength;
      suffixLength = maxLog2TrDynamicRange - rParam;
    } else {
      while (codeValue > ((2 << prefixLength) - 2)) {
        prefixLength++;
      }

      suffixLength = prefixLength + 1; // +1 for the separator bit
    }

    const UInt suffix = codeValue - ((1 << prefixLength) - 1);

    const UInt totalPrefixLength = prefixLength + COEF_REMAIN_BIN_REDUCTION;
    const UInt prefix            = (1 << totalPrefixLength) - 1;
    const UInt rParamBitMask     = (1 << rParam) - 1;

    // prefix
    m_pcBinIf->encodeBinsEP(prefix, totalPrefixLength);

    // separator, suffix, and rParam
    m_pcBinIf->encodeBinsEP(
      ((suffix << rParam) | (symbol & rParamBitMask)),
      (suffixLength + rParam)
    );


  } else {
    UInt length = rParam;
    codeNumber = codeNumber - (COEF_REMAIN_BIN_REDUCTION << rParam);

    while (codeNumber >= (1 << length)) {
      codeNumber -= (1 << length++);
    }

    m_pcBinIf->encodeBinsEP(
      (1 << (COEF_REMAIN_BIN_REDUCTION+length + 1 - rParam)) - 2,
      COEF_REMAIN_BIN_REDUCTION + length + 1 - rParam
    );
    m_pcBinIf->encodeBinsEP(codeNumber, length);
  }
}



Void TDbrCoeffEnc::xCodeTransformSkipFlag(TComTU &tu, ComponentID component) {
        TComDataCU& cu        = *tu.getCU();
  const TComPPS&    pps       = *cu.getSlice()->getPPS();
  const UInt        partIndex = tu.GetAbsPartIdxTU();

  if (cu.getCUTransquantBypass(partIndex)) {
    return;
  }

  if (!TUCompRectHasAssociatedTransformSkipFlag(
      tu.getRect(component),
      pps.getPpsRangeExtension().getLog2MaxTransformSkipBlockSize())) {
    return;
  }

  UInt useTransformSkip = cu.getTransformSkip(partIndex,component);
  m_pcBinIf->encodeBin(
    useTransformSkip,
    m_cTransformSkipSCModel.get(0, toChannelType(component), 0)
  );
}



/**
 * Performs CABAC encoding of the explicit RDPCM mode
 * \param tu         transform unit
 * \param component  component
 */
Void TDbrCoeffEnc::codeExplicitRdpcmMode(TComTU &tu, const ComponentID component) {
  const TComDataCU&    cu          = *tu.getCU();
  const TComRectangle& rect        = tu.getRect(component);
  const UInt           partIndex   = tu.GetAbsPartIdxTU(component);
  const UInt           tuHeight    = g_aucConvertToBit[rect.height];
  const UInt           tuWidth     = g_aucConvertToBit[rect.width];
  const ChannelType    channelType = toChannelType(component);
  const UInt           rdpcmMode   = cu.getExplicitRdpcmMode(component, partIndex);

  assert(tuHeight == tuWidth);
  assert(tuHeight < 4);

  if (rdpcmMode == RDPCM_OFF) {
    m_pcBinIf->encodeBin(0, m_explicitRdpcmFlagSCModel.get(0, channelType, 0));

  } else if (rdpcmMode == RDPCM_HOR || rdpcmMode == RDPCM_VER) {
    m_pcBinIf->encodeBin(1, m_explicitRdpcmFlagSCModel.get(0, channelType, 0));

    if (rdpcmMode == RDPCM_HOR) {
      m_pcBinIf->encodeBin(0, m_explicitRdpcmDirSCModel.get(0, channelType, 0));
    } else {
      m_pcBinIf->encodeBin(1, m_explicitRdpcmDirSCModel.get(0, channelType, 0));
    }
  } else {
    assert(0);
  }
}



Void TDbrCoeffEnc::xCodeSigGroupFlag(UInt isGroupSig, UInt contextOffset, ChannelType channelType) {
  // Layer for coding
  const Int l = 0;

  // Flag value to be coded
  const UInt flag = (isGroupSig ? 1 : 0);

  // Code flag in layer
  layers[l].encodeBin(
    flag,
    layers[l]
      .m_cCUSigCoeffGroupSCModel
      .get(0, channelType, contextOffset)
  );

  // Update higher layer contexts
  for (int i = l + 1; i < layers.size(); i++) {
    layers[i]
      .m_cCUSigCoeffGroupSCModel
      .get(0, channelType, contextOffset)
      .update(flag);
  }
}



Void TDbrCoeffEnc::xCodeSigCoeffFlag(UInt isCoeffSig, UInt contextOffset) {
  // Layer for coding
  const Int l = 0;

  // Flag value to be coded
  const UInt flag = (isCoeffSig ? 1 : 0);

  // Code flag in layer
  layers[l].encodeBin(
    flag,
    layers[l]
      .m_cCUSigSCModel
      .get(0, 0, contextOffset)
  );

  // Update higher layer contexts
  for (int i = l + 1; i < layers.size(); i++) {
    layers[i]
      .m_cCUSigSCModel
      .get(0, 0, contextOffset)
      .update(flag);
  }
}
