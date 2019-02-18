#include "TDbrCoeffEnc.h"

#include "TLibCommon/TComSlice.h"
#include "TLibCommon/TComDataCu.h"
#include "TLibEncoder/TEncEntropy.h"



// Number of bits (as fixed point) to allot for each coded remaining level
// Currently set to 0.25 bits
const Int TDbrCoeffEnc::allowancePerCoeffPerLayer = 0x1 << (TDbrLayer::FIXED_POINT_PRECISION - 2);



// Initial bit budget (as fixed point) for each layer
const Int TDbrCoeffEnc::initialBudgetPerLayer = TDbrLayer::toFixedPoint(3);



TDbrCoeffEnc::TDbrCoeffEnc() :
  layers(1),
  numLayeredBits(0),
  m_pcBinIf(&xmlBinEncoder) {
  for (TDbrLayer& layer : layers) {
    layer.addFixedPointToBudget(initialBudgetPerLayer);
  }
}



TDbrCoeffEnc::TDbrCoeffEnc(Int numLayers) :
  layers(numLayers),
  numLayeredBits(0),
  m_pcBinIf(&xmlBinEncoder) {
  for (TDbrLayer& layer : layers) {
    layer.addFixedPointToBudget(initialBudgetPerLayer);
  }
}



Void TDbrCoeffEnc::setNumLayers(Int numLayers) {
  layers.clear();
  layers.resize(numLayers);
  for (TDbrLayer& layer : layers) {
    layer.addFixedPointToBudget(initialBudgetPerLayer);
  }
}



Void TDbrCoeffEnc::setXmlWriter(TDbrXmlWriter* writer) {
  xmlBinEncoder.setXmlWriter(writer);
}



Int64 TDbrCoeffEnc::getNumLayeredBits() const {
  return numLayeredBits;
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



Void TDbrCoeffEnc::codeCoeffNxN(TComTU &tu, const ComponentID component) {
  codeCoeffNxN(
    tu,
    tu.getCU()->getCoeff(component) + tu.getCoefficientOffset(component),
    component
  );
}



Void TDbrCoeffEnc::codeCoeffNxN(TComTU &tu, TCoeff* coefficients, const ComponentID component) {
        TComDataCU&    cu          = *tu.getCU();
  const UInt           tuPartIndex = tu.GetAbsPartIdxTU(component);
  const TComRectangle& tuRect      = tu.getRect(component);
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
    TEncEntropy::countNonZeroCoeffs(coefficients, tuWidth * tuHeight);
  if (totalNumSigCoeffs == 0) {
    std::cerr << "ERROR: codeCoeffNxN called for empty TU!" << std::endl;
    assert(false);
    exit(1);
  }


  // Prepare coding block parameters
  const UInt        tuWidthLog2            = g_aucConvertToBit[tuWidth] + 2;
  const UInt        tuHeightLog2           = g_aucConvertToBit[tuHeight] + 2;
  const ChannelType channelType            = toChannelType(component);
  const Bool        isPrecisionExtended    = sps.getSpsRangeExtension().getExtendedPrecisionProcessingFlag();
  const Bool        alignCABACBeforeBypass = sps.getSpsRangeExtension().getCabacBypassAlignmentEnabledFlag();
  const Int         maxLog2TrDynamicRange  = sps.getMaxLog2TrDynamicRange(channelType);
        Bool        isSignHidingEnabled    = pps.getSignDataHidingEnabledFlag();


  // Code implicit RDPCM mode
  if (cu.getCUTransquantBypass(tuPartIndex) || isImplicitlyRDPCMCoded(tu, component)) {
    isSignHidingEnabled = false;
    if (!cu.isIntra(tuPartIndex) && cu.isRDPCMEnabled(tuPartIndex)) {
      xCodeExplicitRdpcmMode(tu, component);
    }
  }


  // Code transform skip mode
  if (pps.getUseTransformSkip()) {
    xCodeTransformSkipFlag(tu, component);
    if (cu.getTransformSkip(tuPartIndex, component) && !cu.isIntra(tuPartIndex) && cu.isRDPCMEnabled(tuPartIndex)) {
      //  This TU has coefficients and is transform skipped. Check whether is inter coded and if yes encode the explicit RDPCM mode
      xCodeExplicitRdpcmMode(tu, component);

      //  Sign data hiding is force-disabled for horizontal and vertical explicit RDPCM modes
      if (cu.getExplicitRdpcmMode(component, tuPartIndex) != RDPCM_OFF) {
        isSignHidingEnabled = false;
      }
    }
  }


  // Golomb-Rice adaptive parameters
  //const Bool  bUseGolombRiceParameterAdaptation = sps.getSpsRangeExtension().getPersistentRiceAdaptationEnabledFlag();
  //      UInt& currentGolombRiceStatistic        = m_golombRiceAdaptationStatistics[tu.getGolombRiceStatisticsIndex(component)];


  // Get the scan iterator for this transform block
  TUEntropyCodingParameters codingParameters;
  getTUEntropyCodingParameters(codingParameters, tu, component);


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

    if (coefficients[coeffRasterOrderIndex] != 0) {
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
      component,
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

  // Layer the coefficient data
  xLayerCoefficients(
    tu,                 // Tu structure
    component,          // Component
    lscScanOrderIndex,  // Last significant coefficient
    isGroupSignificant, // Group significant flags
    codingParameters,   // Scan order object
    coefficients        // Coefficients
  );

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
      absCoeffs[0] = static_cast<Int>(abs(coefficients[lscRasterOrderIndex]));
      coeffSigns = (coefficients[lscRasterOrderIndex] < 0);
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
          UInt isCoeffSigFlag = (coefficients[coeffRasterOrderIndex] ? 1 : 0);

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
              isCoeffSigFlag,
              contextOffset + getSignificanceMapContextOffset(component)
            );
          }

          // Store significant coefficients to encode during later scans
          if (isCoeffSigFlag) {
            absCoeffs[numSCsInCG] =
              static_cast<Int>(abs(coefficients[coeffRasterOrderIndex]));
            numSCsInCG++;
            coeffSigns = (coeffSigns << 1) + (coefficients[coeffRasterOrderIndex] < 0 ? 1 : 0);
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
    const UInt contextSet = getContextSetIndex(component, cgScanOrderIndex, gt1Context == 0);
    ContextModel* baseContextModel = &context
      .get(TDbrCabacContexts::SyntaxElement::CoeffGt1Flag)
      .get(0, 0, NUM_ONE_FLAG_CTX_PER_SET * contextSet);
    gt1Context = 1;

    // Set Golomb-Rice parameter
    UInt& currentGolombRiceStatistic =
      context.getGolombRiceStats(tu.getGolombRiceStatisticsIndex(component));
    
    UInt golombRiceParam = currentGolombRiceStatistic / RExt__GOLOMB_RICE_INCREMENT_DIVISOR;
    const Bool isPersistentGolombRiceAdaptionEnabled =
      sps.getSpsRangeExtension().getPersistentRiceAdaptationEnabledFlag();
    Bool shouldAdaptGolombRiceParam = isPersistentGolombRiceAdaptionEnabled;

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
      baseContextModel = &context
        .get(TDbrCabacContexts::SyntaxElement::CoeffGt2Flag)
        .get(0, 0, NUM_ABS_FLAG_CTX_PER_SET * contextSet);
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
          // Adjust coded value
          UInt escapeCodeValue = absCoeffs[sigCoeffIndex] - baseLevel;
          /*xAdjustCodedValue(
            escapeCodeValue,
            golombRiceParam,
            isPrecisionExtended,
            maxLog2TrDynamicRange
          );*/

          // Update coefficient level based on adjusted coded value
          //absCoeffs[sigCoeffIndex] = escapeCodeValue + baseLevel;

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
              isPersistentGolombRiceAdaptionEnabled ?
              golombRiceParam + 1 :
              std::min<UInt>(golombRiceParam + 1, 4);
          }

          // Update Golumb-Rice parameter statistics
          if (shouldAdaptGolombRiceParam) {
            const UInt initialGolombRiceParameter = currentGolombRiceStatistic / RExt__GOLOMB_RICE_INCREMENT_DIVISOR;

            if (escapeCodeValue >= (3 << initialGolombRiceParameter)) {
              currentGolombRiceStatistic++;
            } else if (2*escapeCodeValue < (1 << initialGolombRiceParameter) && currentGolombRiceStatistic > 0) {
              currentGolombRiceStatistic--;
            }

            shouldAdaptGolombRiceParam = false;
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
  // Open tag
  xmlBinEncoder.getXmlWriter()->writeOpenTag("last-sig-xy");


  if (scanIndex == SCAN_VER) {
    swap(xCoord, yCoord);
    swap(width,  height);
  }

  const UInt xCoordGroup = g_uiGroupIdx[xCoord];
  const UInt yCoordGroup = g_uiGroupIdx[yCoord];

  ContextModel* xCoordContext = context
    .get(TDbrCabacContexts::SyntaxElement::LastSigCoeffX)
    .get(0, toChannelType(component));

  ContextModel* yCoordContext = context
    .get(TDbrCabacContexts::SyntaxElement::LastSigCoeffY)
    .get(0, toChannelType(component));

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


  // Close tag
  xmlBinEncoder.getXmlWriter()->writeCloseTag("last-sig-xy");
}



/**
 * Codes coeff_abs_level_minus3
 * \param symbol                  value of coeff_abs_level_minus3
 * \param rParam                  reference to Rice parameter
 * \param useLimitedPrefixLength
 * \param maxLog2TrDynamicRange
 */
Void TDbrCoeffEnc::xWriteCoefRemainExGolomb(UInt symbol, UInt &rParam, const Bool useLimitedPrefixLength, const Int maxLog2TrDynamicRange) {
  Int codeNumber = static_cast<Int>(symbol);


  if (codeNumber < (COEF_REMAIN_BIN_REDUCTION << rParam)) {
    UInt length = codeNumber >> rParam;
    xEncodeBinsEpOneAtATime((1 << (length + 1)) - 2, length + 1);
    //m_pcBinIf->encodeBinsEP((1 << (length + 1)) - 2, length + 1);
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

    xEncodeBinsEpOneAtATime(
      (1 << (COEF_REMAIN_BIN_REDUCTION+length + 1 - rParam)) - 2,
      COEF_REMAIN_BIN_REDUCTION + length + 1 - rParam
    );
    //m_pcBinIf->encodeBinsEP(
    //  (1 << (COEF_REMAIN_BIN_REDUCTION+length + 1 - rParam)) - 2,
    //  COEF_REMAIN_BIN_REDUCTION + length + 1 - rParam
    //);
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

  UInt flag = cu.getTransformSkip(partIndex,component);

  ContextModel& flagContext = context
    .get(TDbrCabacContexts::SyntaxElement::TransformSkip)
    .get(0, toChannelType(component), 0);

  m_pcBinIf->encodeBin(flag, flagContext);
}



/**
 * Performs CABAC encoding of the explicit RDPCM mode
 * \param tu         transform unit
 * \param component  component
 */
Void TDbrCoeffEnc::xCodeExplicitRdpcmMode(TComTU &tu, const ComponentID component) {
  const TComDataCU&    cu          = *tu.getCU();
  const TComRectangle& rect        = tu.getRect(component);
  const UInt           partIndex   = tu.GetAbsPartIdxTU(component);
  const UInt           tuHeight    = g_aucConvertToBit[rect.height];
  const UInt           tuWidth     = g_aucConvertToBit[rect.width];
  const ChannelType    channelType = toChannelType(component);
  const UInt           rdpcmMode   = cu.getExplicitRdpcmMode(component, partIndex);

  assert(tuHeight == tuWidth);
  assert(tuHeight < 4);

  ContextModel& flagContext = context
    .get(TDbrCabacContexts::SyntaxElement::ExplicitRdpcmFlag)
    .get(0, channelType, 0);

  if (rdpcmMode == RDPCM_OFF) {
    m_pcBinIf->encodeBin(0, flagContext);

  } else if (rdpcmMode == RDPCM_HOR || rdpcmMode == RDPCM_VER) {
    m_pcBinIf->encodeBin(1, flagContext);

    ContextModel& dirContext = context
      .get(TDbrCabacContexts::SyntaxElement::ExplicitRdpcmDir)
      .get(0, channelType, 0);

    if (rdpcmMode == RDPCM_HOR) {
      m_pcBinIf->encodeBin(0, dirContext);
    } else {
      m_pcBinIf->encodeBin(1, dirContext);
    }
  } else {
    assert(0);
  }
}



Void TDbrCoeffEnc::xCodeSigGroupFlag(UInt isGroupSig, UInt contextGroup, ChannelType channelType) {
  // Flag value to be coded
  const UInt flag = (isGroupSig ? 1 : 0);

  // Context
  ContextModel& flagContext = context
    .get(TDbrCabacContexts::SyntaxElement::SigCoeffGroupFlag)
    .get(0, channelType, contextGroup);

  // Code flag
  m_pcBinIf->encodeBin(flag, flagContext);
}



Void TDbrCoeffEnc::xCodeSigCoeffFlag(UInt isCoeffSig, UInt contextGroup) {
  // Flag value to be coded
  const UInt flag = (isCoeffSig ? 1 : 0);

  // Context
  ContextModel& flagContext = context
    .get(TDbrCabacContexts::SyntaxElement::SigCoeffFlag)
    .get(0, 0, contextGroup);

  // Code flag
  m_pcBinIf->encodeBin(flag, flagContext);
}



Void TDbrCoeffEnc::xEncodeBinsEpOneAtATime(UInt bins, UInt numBins) {
  for (int i = numBins - 1; i >= 0; i--) {
    m_pcBinIf->encodeBinEP((bins >> i) & 0x1);
  }
}



Void TDbrCoeffEnc::resetEntropy(TComSlice* pSlice) {
  context.resetEntropy(pSlice);
}



static inline Void writeBits(UInt& bits, Int& numCodedBits, UInt bitsToWrite, UInt numBitsToWrite) {
  assert(numCodedBits + numBitsToWrite <= 32);
  bits = (bits << numBitsToWrite) | bitsToWrite;
  numCodedBits += numBitsToWrite;
}



static inline Void readBits(UInt& bits, Int& numBitsLeft, UInt& bitsRead, UInt numBitsToRead) {
  assert(numBitsLeft >= numBitsToRead);
  bitsRead = (bits >> (numBitsLeft - numBitsToRead)) & ((0x1 << numBitsToRead) - 1);
  numBitsLeft -= numBitsToRead;
}



/**
 * Codes coeff_abs_level_minus3
 * \param symbol                  value of coeff_abs_level_minus3
 * \param riceParam               Rice parameter
 * \param useLimitedPrefixLength
 * \param maxLog2TrDynamicRange
 * \param codedBits               coded bits
 * \param numCodedBits            number of coded bits
 */
Void xGetRemCoeffBits(UInt ValueToEncode, UInt riceParam, Bool useLimitedPrefixLength, Int maxLog2TrDynamicRange, UInt& codedBits, Int& numCodedBits) {
  Int codeNumber = static_cast<Int>(ValueToEncode);
  codedBits = 0;
  numCodedBits = 0;

  if (codeNumber < (COEF_REMAIN_BIN_REDUCTION << riceParam)) {
    UInt length = codeNumber >> riceParam;
    writeBits(codedBits, numCodedBits, (1 << (length + 1)) - 2, length + 1);
    writeBits(codedBits, numCodedBits, codeNumber % (1 << riceParam), riceParam);


  } else if (useLimitedPrefixLength) {
    const UInt maximumPrefixLength =
      (32 - (COEF_REMAIN_BIN_REDUCTION + maxLog2TrDynamicRange));

    UInt prefixLength = 0;
    UInt suffixLength = MAX_UINT;
    UInt codeValue    = (ValueToEncode >> riceParam) - COEF_REMAIN_BIN_REDUCTION;

    if (codeValue >= ((1 << maximumPrefixLength) - 1)) {
      prefixLength = maximumPrefixLength;
      suffixLength = maxLog2TrDynamicRange - riceParam;
    } else {
      while (codeValue > ((2 << prefixLength) - 2)) {
        prefixLength++;
      }

      suffixLength = prefixLength + 1; // +1 for the separator bit
    }

    const UInt suffix = codeValue - ((1 << prefixLength) - 1);

    const UInt totalPrefixLength = prefixLength + COEF_REMAIN_BIN_REDUCTION;
    const UInt prefix            = (1 << totalPrefixLength) - 1;
    const UInt rParamBitMask     = (1 << riceParam) - 1;

    // prefix
    writeBits(codedBits, numCodedBits, prefix, totalPrefixLength);

    // separator, suffix, and rParam
    writeBits(
      codedBits,
      numCodedBits,
      (suffix << riceParam) | (ValueToEncode & rParamBitMask),
      suffixLength + riceParam
    );


  } else {
    UInt length = riceParam;
    codeNumber = codeNumber - (COEF_REMAIN_BIN_REDUCTION << riceParam);

    while (codeNumber >= (1 << length)) {
      codeNumber -= (1 << length++);
    }

    writeBits(
      codedBits,
      numCodedBits,
      (1 << (COEF_REMAIN_BIN_REDUCTION + length + 1 - riceParam)) - 2,
      COEF_REMAIN_BIN_REDUCTION + length + 1 - riceParam
    );

    writeBits(codedBits, numCodedBits, codeNumber, length);
  }
}



/** Parsing of coeff_abs_level_remaing
 * \param rSymbol                 reference to coeff_abs_level_remaing
 * \param rParam                  reference to parameter
 * \param useLimitedPrefixLength
 * \param maxLog2TrDynamicRange
 * \param codedBits               coded bits
 * \param numBitsLeft             number of coded bits
 */
Void xGetRemCoeffValue(UInt& decodedValue, UInt riceParam, Bool useLimitedPrefixLength, Int maxLog2TrDynamicRange, UInt codedBits, Int numCodedBits) {
  UInt prefix   = 0;
  UInt codeWord = 0;

  if (useLimitedPrefixLength) {
    const UInt longestPossiblePrefix = (32 - (COEF_REMAIN_BIN_REDUCTION + maxLog2TrDynamicRange)) + COEF_REMAIN_BIN_REDUCTION;

    do {
      prefix++;
      readBits(codedBits, numCodedBits, codeWord, 1);
    } while (codeWord != 0 && prefix < longestPossiblePrefix);
  } else {
    do {
      prefix++;
      readBits(codedBits, numCodedBits, codeWord, 1);
    } while (codeWord);
  }

  codeWord = 1 - codeWord;
  prefix -= codeWord;
  codeWord = 0;

  if (prefix < COEF_REMAIN_BIN_REDUCTION) {
    readBits(codedBits, numCodedBits, codeWord, riceParam);
    decodedValue = (prefix << riceParam) + codeWord;
  } else if (useLimitedPrefixLength) {
    const UInt maximumPrefixLength = (32 - (COEF_REMAIN_BIN_REDUCTION + maxLog2TrDynamicRange));

    const UInt prefixLength = prefix - COEF_REMAIN_BIN_REDUCTION;
    const UInt suffixLength = (prefixLength == maximumPrefixLength) ? (maxLog2TrDynamicRange - riceParam) : prefixLength;
    
    readBits(codedBits, numCodedBits, codeWord, suffixLength + riceParam);

    decodedValue = codeWord + ((((1 << prefixLength) - 1) + COEF_REMAIN_BIN_REDUCTION) << riceParam);
  } else {
    readBits(codedBits, numCodedBits, codeWord, prefix - COEF_REMAIN_BIN_REDUCTION + riceParam);
    decodedValue = (((1 << (prefix - COEF_REMAIN_BIN_REDUCTION)) + COEF_REMAIN_BIN_REDUCTION - 1) << riceParam) + codeWord;
  }
}



Void TDbrCoeffEnc::xAllocateBits() {
  for (TDbrLayer& layer : layers) {
    layer.addFixedPointToBudget(allowancePerCoeffPerLayer);
  }
}



Void TDbrCoeffEnc::xAdjustCodedValue(UInt& value, UInt riceParam, Bool useLimitedPrefixLength, Int maxLog2TrDynamicRange, Int startingLayer) {
  // Trial-encode the Golomb-Rice code
  UInt codedBits;
  Int numCodedBits;
  xGetRemCoeffBits(
    value,
    riceParam,
    useLimitedPrefixLength,
    maxLog2TrDynamicRange,
    codedBits,
    numCodedBits
  );

  // Determine the layer in which each bit ended up
  Int numActualCodedBits = 0;
  for (int i = startingLayer; i < layers.size(); i++) {
    TDbrLayer& layer = layers[i];

    const Int numAvailableBitsInThisLayer = layer.getBudgetInBits();

    // The rest of the bits could be coded in this layer
    if (numCodedBits <= numActualCodedBits + numAvailableBitsInThisLayer) {
      layer.spendBits(numCodedBits - numActualCodedBits);
      numActualCodedBits = numCodedBits;
      break;

    // Only some of the bits could be coded in this layer
    } else {
      layer.spendBits(numAvailableBitsInThisLayer);
      numActualCodedBits += numAvailableBitsInThisLayer;
    }
  }

  // The number of bits that couldn't be coded
  UInt numUncodedBits = numCodedBits - numActualCodedBits;

  // If not all bits could be coded, zero out all bits that couldn't and
  //   re-calculate the value represented by the new group of bits
  if (numUncodedBits > 0) {
    codedBits &= ~((0x1 << numUncodedBits) - 1);
    xGetRemCoeffValue(
      value,
      riceParam,
      useLimitedPrefixLength,
      maxLog2TrDynamicRange,
      codedBits,
      numCodedBits
    );
  }

  // Account for the bits that were actually signaled
  numLayeredBits += TDbrLayer::toFixedPoint(numActualCodedBits);
}



static UInt getSigFlagContextOffset(
  const TComTU& tu,
  ComponentID component,
  const UInt* isGroupSignificant,
  const TUEntropyCodingParameters& codingParameters,
  UInt coeffScanOrderIndex
) {
  const Int cgScanOrderIndex = coeffScanOrderIndex >> MLS_CG_SIZE;
  const Int cgRasterOrderIndex = codingParameters.scanCG[cgScanOrderIndex];
  const Int cgYPos = cgRasterOrderIndex / codingParameters.widthInGroups;
  const Int cgXPos = cgRasterOrderIndex - (cgYPos * codingParameters.widthInGroups);

  const Int sigCoeffContext = TComTrQuant::calcPatternSigCtx(
    isGroupSignificant,
    cgXPos,
    cgYPos,
    codingParameters.widthInGroups,
    codingParameters.heightInGroups
  );

  const TComRectangle& tuRect       = tu.getRect(component);
  const UInt           tuWidth      = tuRect.width;
  const UInt           tuHeight     = tuRect.height;
  const UInt           tuWidthLog2  = g_aucConvertToBit[tuWidth]  + 2;
  const UInt           tuHeightLog2 = g_aucConvertToBit[tuHeight] + 2;

  const UInt contextOffset = TComTrQuant::getSigCtxInc(
    sigCoeffContext,
    codingParameters,
    coeffScanOrderIndex,
    tuWidthLog2,
    tuHeightLog2,
    toChannelType(component)
  );

  return contextOffset + getSignificanceMapContextOffset(component);
}



Void TDbrCoeffEnc::xLayerCoefficients(
  const TComTU& tu,
  ComponentID component,
  Int lscScanIndex,
  const UInt* isGroupSignificant,
  const TUEntropyCodingParameters& codingParameters,
  TCoeff* coefficients
) {
  // Reset layers for the transform block
  for (TDbrLayer& layer : layers) {
    layer.transformBlockReset();
  }

  // Iterate over the coefficients in the block
  for (Int coeffScanIndex = 0; coeffScanIndex <= lscScanIndex; coeffScanIndex++) {
    // Coefficient group scan-order index
    const Int cgScanIndex = (coeffScanIndex >> MLS_CG_SIZE);

    // True if this is the first coefficient in the group
    const Bool isFirstCoeffInGroup = coeffScanIndex & ((0x1 << MLS_CG_SIZE) - 1) == 0;

    // Reset layers for this coefficient group
    if (isFirstCoeffInGroup) {
      for (TDbrLayer& layer : layers) {
        layer.coeffGroupReset();
      }
    }

    // Coefficient raster-order index
    const UInt coeffRasterIndex = codingParameters.scan[coeffScanIndex];

    // Absolute value of coefficient
    const UInt absCoeff = abs(coefficients[coeffRasterIndex]);
    const Bool isSignNegative = (coefficients[coeffRasterIndex] < 0);

    // Coefficient flags
    const UInt isCoeffSigFlag = (absCoeff > 0 ? 1 : 0);
    const UInt isCoeffGt1Flag = (absCoeff > 1 ? 1 : 0);
    const UInt isCoeffGt2Flag = (absCoeff > 2 ? 1 : 0);

    // Calculate context offset for significance flag
    const UInt sigFlagContextOffset = getSigFlagContextOffset(
      tu,
      component,
      isGroupSignificant,
      codingParameters,
      coeffScanIndex
    );

    // Layer in which the significance flag is coded
    Int significanceFlagLayer = -1;

    // Code significance flag
    for (int l = 0; l < layers.size(); l++) {
      TDbrLayer& layer = layers[l];
      const Bool isLastLayer = (l + 1 == layers.size());

      // Significance flag was signaled in a lower layer, so update context
      if (significanceFlagLayer > -1) {
        layer.observeSigFlag(isCoeffSigFlag, sigFlagContextOffset);

      // Try to signal significance flag in this layer
      } else if (layer.hasBudgetForSigFlag(sigFlagContextOffset)) {
        numLayeredBits += layer.codeSigFlag(isCoeffSigFlag, sigFlagContextOffset);
        significanceFlagLayer = l;

      // Adjust coded flag value if this is the last layer
      } else if (isLastLayer) {
        coefficients[coeffRasterIndex] = layer.mps(
          TDbrCabacContexts::SyntaxElement::SigCoeffFlag,
          sigFlagContextOffset
        );
        if (isSignNegative) {
          coefficients[coeffRasterIndex] = -coefficients[coeffRasterIndex];
        }
      }
    }

    // If the significance flag wasn't coded, this coefficient can't be finished
    if (significanceFlagLayer == -1 || !isCoeffSigFlag) {
      break;
    }

    // Layer in which the gt1 flag is coded
    Int gt1FlagLayer = -1;

    // Code gt1 flag
    for (int l = significanceFlagLayer; l < layers.size(); l++) {
      TDbrLayer& layer = layers[l];
      const Bool isLastLayer = (l + 1 == layers.size());

      // Calculate context group for gt1 flag
      const UInt flagContextGroup = getContextSetIndex(
        component,
        cgScanIndex,
        layer.didLastCoeffGroupHaveAGt1Flag()
      );

      // Calculate context offset for gt1 flag
      const UInt gt1FlagContextOffset = layer.getFlagContextState() +
        flagContextGroup * NUM_ONE_FLAG_CTX_PER_SET;

      // Gt1 flag was signaled in a lower layer, so update context
      if (gt1FlagLayer > -1) {
        layer.observeGt1Flag(isCoeffGt1Flag, gt1FlagContextOffset);

      // Try to signal gt1 flag in this layer
      } else if (layer.hasBudgetForGt1Flag(gt1FlagContextOffset)) {
        numLayeredBits += layer.codeGt1Flag(isCoeffGt1Flag, gt1FlagContextOffset);
        gt1FlagLayer = l;

      // Adjust coded flag value if this is the last layer
      } else if (isLastLayer) {
        coefficients[coeffRasterIndex] = 1 + layer.mps(
          TDbrCabacContexts::SyntaxElement::CoeffGt1Flag,
          gt1FlagContextOffset
        );
        if (isSignNegative) {
          coefficients[coeffRasterIndex] = -coefficients[coeffRasterIndex];
        }
      }
    }

    // If the gt1 flag wasn't coded, this coefficient can't be finished
    if (gt1FlagLayer == -1 || !isCoeffGt1Flag) {
      break;
    }

    // Layer in which the gt2 flag is coded
    Int gt2FlagLayer = -1;

    // Code gt2 flag
    for (int l = gt1FlagLayer; l < layers.size(); l++) {
      TDbrLayer& layer = layers[l];
      const Bool isLastLayer = (l + 1 == layers.size());

      // Calculate context group for gt2 flag
      const UInt flagContextGroup = getContextSetIndex(
        component,
        cgScanIndex,
        layer.didLastCoeffGroupHaveAGt1Flag()
      );

      // Calculate context offset for gt2 flag
      const UInt gt2FlagContextOffset =
        flagContextGroup * NUM_ABS_FLAG_CTX_PER_SET;

      // Gt1 flag was signaled in a lower layer, so update context
      if (gt2FlagLayer > -1) {
        layer.observeGt2Flag(isCoeffGt2Flag, gt2FlagContextOffset);

      // Try to signal gt2 flag in this layer
      } else if (layer.hasBudgetForGt2Flag(gt2FlagContextOffset)) {
        numLayeredBits += layer.codeGt2Flag(isCoeffGt2Flag, gt2FlagContextOffset);
        gt2FlagLayer = l;

      // Adjust coded flag value if this is the last layer
      } else if (isLastLayer) {
        coefficients[coeffRasterIndex] = 2 + layer.mps(
          TDbrCabacContexts::SyntaxElement::CoeffGt2Flag,
          gt2FlagContextOffset
        );
        if (isSignNegative) {
          coefficients[coeffRasterIndex] = -coefficients[coeffRasterIndex];
        }
      }
    }

    // If the gt2 flag wasn't coded, this coefficient can't be finished
    if (gt2FlagLayer == -1 || !isCoeffGt2Flag) {
      break;
    }

    // Remaining coefficient level
    UInt escapeCodeValue = absCoeff - 3;

    // Code the remaining level with Exp-Golomb
    xAdjustCodedValue(
      escapeCodeValue,
      golombRiceParam,
      isPrecisionExtended,
      maxLog2TrDynamicRange,
      gt2FlagLayer
    );

    // Adjust Golomb-Rice parameter
    if (absCoeff > (3 << golombRiceParam)) {
      golombRiceParam =
        isPersistentGolombRiceAdaptionEnabled ?
        golombRiceParam + 1 :
        std::min<UInt>(golombRiceParam + 1, 4);
    }

    // Update Golumb-Rice parameter statistics
    if (shouldAdaptGolombRiceParam) {
      const UInt initialGolombRiceParameter = currentGolombRiceStatistic / RExt__GOLOMB_RICE_INCREMENT_DIVISOR;

      if (escapeCodeValue >= (3 << initialGolombRiceParameter)) {
        currentGolombRiceStatistic++;
      } else if (2*escapeCodeValue < (1 << initialGolombRiceParameter) && currentGolombRiceStatistic > 0) {
        currentGolombRiceStatistic--;
      }

      shouldAdaptGolombRiceParam = false;
    }
  }
}
