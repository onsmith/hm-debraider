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
 *  \file     TDbrLayer.h
 *  \project  TAppDebraider
 *  \brief    Cabac layer
 */


#pragma once

#include "TDbrCabacContexts.h"
#include "TLibCommon/TComBitStream.h"
#include "TLibEncoder/TEncBinCoderCABACCounter.h"


//! \ingroup TAppDebraider
//! \{


class TDbrLayer {
public:
  static const UInt FIXED_POINT_PRECISION = 15;

  static const UInt toFixedPoint(UInt val);
  static const UInt toInt(UInt val);

  typedef TDbrCabacContexts::SyntaxElement SyntaxElement;


protected:
  // CABAC counter
  //TEncBinCABACCounter cabacEncoder;

  // Fake output bitstream
  //TComOutputBitstream bitstream;

  // Contexts
  TDbrCabacContexts contexts;

  // Bit budget
  UInt64 budget;

  // Tracks number of coded flags
  UInt numCodedSigFlags;
  UInt numCodedGt1Flags;
  UInt numCodedGt2Flags;

  // Tracks context state for coefficient level flags
  UInt flagContextState;
  UInt lastFlagContextState;

  // Tracks Golomb-Rice parameter
  UInt golombRiceParam;


public:
  // Constructor
  TDbrLayer();

  // High-level state resets
  Void naluReset(TComSlice& slice);
  Void transformBlockReset();
  Void coeffGroupReset();

  // Golomb-Rice parameter management
  UInt getGolombRiceParam() const;
  Void updateGolombRiceParam(UInt absCoeff);

  // Bit budget management
  UInt64 getBudget() const;
  UInt getBudgetInBits() const;
  Void spendBits(UInt numBits);
  Void addBitsToBudget(UInt numBits);
  Void addFixedPointToBudget(UInt amount);

  // Check if there's room for another flag
  Bool hasRoomForGt1Flag() const;
  Bool hasRoomForGt2Flag() const;

  // Gets the current flag context group index
  UInt getFlagContextState() const;
  Bool didLastCoeffGroupHaveAGt1Flag() const;

  // Flag coding decision
  Bool hasBudgetForFlag(SyntaxElement flagType, UInt contextIndex);
  Bool hasBudgetForSigFlag(UInt contextIndex);
  Bool hasBudgetForGt1Flag(UInt contextIndex);
  Bool hasBudgetForGt2Flag(UInt contextIndex);

  // Flag coding
  UInt codeFlag(SyntaxElement flagType, UInt flag, UInt contextIndex);
  UInt codeSigFlag(UInt flag, UInt contextIndex);
  UInt codeGt1Flag(UInt flag, UInt contextIndex);
  UInt codeGt2Flag(UInt flag, UInt contextIndex);

  // Flag accounting
  Void observeFlag(SyntaxElement flagType, UInt flag, UInt contextIndex);
  Void observeSigFlag(UInt flag, UInt contextIndex);
  Void observeGt1Flag(UInt flag, UInt contextIndex);
  Void observeGt2Flag(UInt flag, UInt contextIndex);

  // Gets the most probable symbol for a given flag/context
  UInt mps(SyntaxElement flagType, UInt contextIndex);

  // Calculates the bit cost of coding a given flag
  UInt bitCost(SyntaxElement flagType, UInt flag, UInt contextIndex);

  // Calculates the bit cost of coding a given flag using a given context
  static UInt bitCost(ContextModel& context, UInt flag);

  // Calculates the bit cost associated with encoding the least probable symbol
  //   of a given context
  static UInt maxBitCost(ContextModel& context);

  // Calculates the bit cost associated with encoding the most probable symbol
  //   of a given context
  static UInt minBitCost(ContextModel& context);


protected:
  // State reset
  Void xResetBudget();
  Void xResetContexts(TComSlice& slice);
  Void xResetCoeffGroupFlagCounts();
  Void xResetFlagContextState();
  Void xResetGolombRiceParam();
};


//! \}
