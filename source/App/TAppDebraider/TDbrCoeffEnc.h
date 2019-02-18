/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2017, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


/**
 *  \file     TDbrCoeffEnc.h
 *  \project  TAppDebraider
 *  \brief    Layered coefficient encoder
 */


#pragma once

#include "TLibCommon/TComTu.h"
#include "TDbrCabacContexts.h"
#include "TDbrXmlWriter.h"
#include "TDbrLayer.h"
#include "XmlBinEncoder.h"

#include <vector>
using std::vector;


//! \ingroup TAppDebraider
//! \{


class TDbrCoeffEnc {
public:
  // Number of bits (as fixed point) to allot for each coded remaining level
  static const Int allowancePerCoeffPerLayer;

  // Initial bit budget (as fixed point) for each layer
  static const Int initialBudgetPerLayer;


protected:
  // Tracks total number of bits used to code remaining level
  Int64 numLayeredBits;

  // Layers
  vector<TDbrLayer> layers;

  // Xml bin encoder
  XmlBinEncoder xmlBinEncoder;

  // Pointer to xmlBinEncoder
  TEncBinIf* const m_pcBinIf;

  // Cabac context
  TDbrCabacContexts context;


public:
  // Constructors
  TDbrCoeffEnc();
  TDbrCoeffEnc(Int numLayers);

  // Sets the bits allotted for each coded remaining level
  Void setNumLayers(Int numLayers);

  // Codes the coefficients for a given transform block
  Void codeCoeffNxN(TComTU &tu, const ComponentID component);

  // Codes the given coefficients for a given transform block
  Void codeCoeffNxN(TComTU &tu, TCoeff* coefficients, const ComponentID component);

  // Sets the bin writer
  Void setXmlWriter(TDbrXmlWriter* writer);

  // Resets the internal entropy
  Void resetEntropy(TComSlice* pSlice);

  // Gets the total number of bits used to code remaining level
  Int64 getNumLayeredBits() const;


protected:
  // Codes the (x, y) position of the last significant coefficient within a
  //   transform block
  Void xCodeLastSignificantXY(UInt xCoord, UInt yCoord, Int width, Int height, ComponentID component, UInt scanIndex);

  // Codes the remaining level for a coefficient
  Void xWriteCoefRemainExGolomb(UInt symbol, UInt &rParam, const Bool useLimitedPrefixLength, const Int maxLog2TrDynamicRange);

  // Codes the transform skip flag for the given transform block
  Void xCodeTransformSkipFlag(TComTU &tu, ComponentID component);

  // Codes the explicit RDPCM mode data
  Void xCodeExplicitRdpcmMode(TComTU &tu, const ComponentID component);

  // Codes the significance flag a coefficient group
  Void xCodeSigGroupFlag(UInt flag, UInt contextOffset, ChannelType channelType);

  // Codes the significance flag for a coefficient
  Void xCodeSigCoeffFlag(UInt isCoeffSig, UInt contextOffset);

  // Separates and writes equiprobable bins one at a time
  Void xEncodeBinsEpOneAtATime(UInt bins, UInt numBins);

  // Adjusts the coded value based on the available budget
  Void xAdjustCodedValue(UInt& value, UInt riceParam, Bool useLimitedPrefixLength, Int maxLog2TrDynamicRange, Int startingLayer=0);

  // Allocates bits to each layer's bit budget
  Void xAllocateBits();

  // Layers coefficient data while individually tracking the bit budget, coded
  //   coefficient flags/levels, and CABAC contexts for each layer
  Void xLayerCoefficients(
    const TComTU& tu,
    ComponentID component,
    Int lscScanIndex,
    const UInt* isGroupSignificant,
    const TUEntropyCodingParameters& codingParameters,
    TCoeff* coefficients
  );
};


//! \}
