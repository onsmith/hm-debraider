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
#include "TLibEncoder/TEncBinCoderCABACCounter.h"
#include "TLibCommon/ContextModel3DBuffer.h"

#include <vector>


using std::vector;


//! \ingroup TAppDebraider
//! \{


class TDbrLayer {
public:
  TDbrLayer();

  Void encodeBin(UInt value, ContextModel& context);

protected:
  // CABAC counter
  TEncBinCABACCounter cabacEncoder;

  // Fake output bitstream
  TComOutputBitstream bitstream;

  // Context models
  ContextModel contextModels[MAX_NUM_CTX_MOD];
  Int          numContextModels;

public:
  // 3D context model structures
  ContextModel3DBuffer m_cCUSigCoeffGroupSCModel;  // Is coefficient group significant flag
  ContextModel3DBuffer m_cCUSigSCModel;            // Is coefficient significant flag
  ContextModel3DBuffer m_cCUOneSCModel;            // Is coefficient greater than one flag
  ContextModel3DBuffer m_cCUAbsSCModel;            // Is coefficient greater than two flag
  ContextModel3DBuffer m_cTransformSkipSCModel;    // Is block transform skipped flag
  ContextModel3DBuffer m_explicitRdpcmFlagSCModel; // Is rdpcm enabled flag
  ContextModel3DBuffer m_explicitRdpcmDirSCModel;  // Rdpcm direction
  ContextModel3DBuffer m_cCuCtxLastX;              // Last significant coefficient x coordinate
  ContextModel3DBuffer m_cCuCtxLastY;              // Last significant coefficient y coordinate

  // Golomb-Rice adaption statistics
  UInt m_golombRiceAdaptationStatistics[RExt__GOLOMB_RICE_ADAPTATION_STATISTICS_SETS];
};


class TDbrCoeffEnc {
protected:
  vector<TDbrLayer> layers;

public:
  // Constructors
  TDbrCoeffEnc() = default;
  TDbrCoeffEnc(size_t numLayers);

  // Gets the number of layers in the coder
  size_t numLayers() const;

  // Adds a layer to the encoder
  Void addLayer();

  // Codes the given coefficients for a given transform block
  Void codeCoeffNxN(TComTU &tu, TCoeff* pcCoef, const ComponentID compID);

protected:
  // Codes the (x, y) position of the last significant coefficient within a
  //   transform block
  Void xCodeLastSignificantXY(UInt xCoord, UInt yCoord, Int width, Int height, ComponentID component, UInt scanIndex);

  // Codes the remaining level for a coefficient
  Void xWriteCoefRemainExGolomb(UInt symbol, UInt &rParam, const Bool useLimitedPrefixLength, const Int maxLog2TrDynamicRange);

  // Codes the transform skip flag for the given transform block
  Void xCodeTransformSkipFlag(TComTU &tu, ComponentID component);

  // Codes the explicit RDPCM mode data
  Void codeExplicitRdpcmMode(TComTU &tu, const ComponentID component);

  // Codes the significance flag a coefficient group
  Void xCodeSigGroupFlag(UInt flag, UInt contextOffset, ChannelType channelType);

  // Codes the significance flag for a coefficient
  Void xCodeSigCoeffFlag(UInt isCoeffSig, UInt contextOffset);
};


//! \}
