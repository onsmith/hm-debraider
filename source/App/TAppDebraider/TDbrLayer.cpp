#include "TDbrLayer.h"


TDbrLayer::TDbrLayer() :
  flagContextState(1),
  lastFlagContextState(1),
  golombRiceParam(0),
  numCodedSigFlags(0),
  numCodedGt1Flags(0),
  numCodedGt2Flags(0),
  budget(0) {
  //cabacEncoder.init(&bitstream);
}


UInt TDbrLayer::getBudget() const {
  return budget;
}


UInt TDbrLayer::getBudgetInBits() const {
  return (budget >> FIXED_POINT_PRECISION);
}


Void TDbrLayer::addBitsToBudget(UInt numBits) {
  budget += (numBits << FIXED_POINT_PRECISION);
}


Void TDbrLayer::addFixedPointToBudget(UInt amount) {
  budget += amount;
}


Void TDbrLayer::spendBits(UInt numBits) {
  budget -= (numBits << FIXED_POINT_PRECISION);
}

const UInt TDbrLayer::toFixedPoint(UInt val) {
  return (val << FIXED_POINT_PRECISION);
}


const UInt TDbrLayer::toInt(UInt val) {
  return (val >> FIXED_POINT_PRECISION);
}


Void TDbrLayer::xResetContexts(TComSlice& slice) {
  contexts.resetEntropy(&slice);
}


Void TDbrLayer::xResetBudget() {
  budget = 0;
}


Void TDbrLayer::xResetCoeffGroupFlagCounts() {
  numCodedSigFlags = 0;
  numCodedGt1Flags = 0;
  numCodedGt2Flags = 0;
}


Void TDbrLayer::xResetFlagContextState() {
  lastFlagContextState = flagContextState;
  flagContextState = 1;
}


UInt TDbrLayer::bitCost(SyntaxElement flagType, UInt flag, UInt contextIndex) {
  return bitCost(contexts.get(flagType).get(0, 0, contextIndex), flag);
}


UInt TDbrLayer::bitCost(ContextModel& context, UInt flag) {
  return
    context.getEntropyBits(flag) >>
    (15 - TDbrLayer::FIXED_POINT_PRECISION);
}


UInt TDbrLayer::maxBitCost(ContextModel& context) {
  return bitCost(context, 1 - context.getMps());
}


UInt TDbrLayer::minBitCost(ContextModel& context) {
  return bitCost(context, context.getMps());
}


Bool TDbrLayer::hasBudgetForFlag(SyntaxElement flagType, UInt contextIndex) {
  return (
    budget >= maxBitCost(contexts.get(flagType).get(0, 0, contextIndex))
  );
}


Bool TDbrLayer::hasBudgetForSigFlag(UInt contextIndex) {
  return hasBudgetForFlag(
    SyntaxElement::SigCoeffFlag,
    contextIndex
  );
}


Bool TDbrLayer::hasBudgetForGt1Flag(UInt contextIndex) {
  return hasBudgetForFlag(
    SyntaxElement::CoeffGt1Flag,
    contextIndex
  );
}


Bool TDbrLayer::hasBudgetForGt2Flag(UInt contextIndex) {
  return hasBudgetForFlag(
    SyntaxElement::CoeffGt2Flag,
    contextIndex
  );
}


UInt TDbrLayer::codeFlag(SyntaxElement flagType, UInt flag, UInt contextIndex) {
  // Get context model
  ContextModel& context = contexts.get(flagType).get(0, 0, contextIndex);

  // Update budget
  const UInt cost = bitCost(context, flag);
  budget -= cost;

  // Update context
  context.update(flag);

  return cost;
}


UInt TDbrLayer::codeSigFlag(UInt flag, UInt contextIndex) {
  numCodedSigFlags++;
  return codeFlag(SyntaxElement::SigCoeffFlag, flag, contextIndex);
}


UInt TDbrLayer::codeGt1Flag(UInt flag, UInt contextIndex) {
  numCodedGt1Flags++;
  if (flag) {
    flagContextState = 0;
  } else if (0 < flagContextState && flagContextState < 4) {
    flagContextState++;
  }
  return codeFlag(SyntaxElement::CoeffGt1Flag, flag, contextIndex);
}


UInt TDbrLayer::codeGt2Flag(UInt flag, UInt contextIndex) {
  numCodedGt2Flags++;
  return codeFlag(SyntaxElement::CoeffGt2Flag, flag, contextIndex);
}


UInt TDbrLayer::mps(SyntaxElement flagType, UInt contextIndex) {
  return contexts.get(flagType).get(0, 0, contextIndex).getMps();
}


Void TDbrLayer::observeFlag(SyntaxElement flagType, UInt flag, UInt contextIndex) {
  contexts
    .get(flagType)
    .get(0, 0, contextIndex)
    .update(flag);
}


Void TDbrLayer::observeSigFlag(UInt flag, UInt contextIndex) {
  numCodedSigFlags++;
  observeFlag(SyntaxElement::SigCoeffFlag, flag, contextIndex);
}


Void TDbrLayer::observeGt1Flag(UInt flag, UInt contextIndex) {
  numCodedGt1Flags++;
  if (flag) {
    flagContextState = 0;
  } else if (0 < flagContextState && flagContextState < 4) {
    flagContextState++;
  }
  observeFlag(SyntaxElement::CoeffGt1Flag, flag, contextIndex);
}


Void TDbrLayer::observeGt2Flag(UInt flag, UInt contextIndex) {
  numCodedGt2Flags++;
  observeFlag(SyntaxElement::CoeffGt2Flag, flag, contextIndex);
}


Bool TDbrLayer::hasRoomForGt1Flag() const {
  return numCodedGt1Flags < 8;
}


Bool TDbrLayer::hasRoomForGt2Flag() const {
  return numCodedGt2Flags < 1;
}


UInt TDbrLayer::getFlagContextState() const {
  return flagContextState;
}


Bool TDbrLayer::didLastCoeffGroupHaveAGt1Flag() const {
  return (lastFlagContextState == 0);
}


Void TDbrLayer::naluReset(TComSlice& slice) {
  xResetContexts(slice);
}


Void TDbrLayer::transformBlockReset() {
  xResetCoeffGroupFlagCounts();
  xResetGolombRiceParam();
  lastFlagContextState = 1;
  flagContextState     = 1;
}


Void TDbrLayer::coeffGroupReset() {
  xResetCoeffGroupFlagCounts();
  xResetFlagContextState();
  xResetGolombRiceParam();
}



UInt TDbrLayer::getGolombRiceParam() const {
  return golombRiceParam;
}


UInt TDbrLayer::updateGolombRiceParam(UInt absCoeff) {
  if (absCoeff > (3 << golombRiceParam)) {
    golombRiceParam = std::min<UInt>(golombRiceParam + 1, 4);
  }
}


Void TDbrLayer::xResetGolombRiceParam() {
  golombRiceParam = 0;
}
