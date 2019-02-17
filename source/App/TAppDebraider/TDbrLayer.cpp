#include "TDbrLayer.h"


TDbrLayer::TDbrLayer() :
  numCodedGt1Flags(0),
  numCodedGt2Flags(0),
  budget(0) {
  cabacEncoder.init(&bitstream);
}


Void TDbrLayer::encodeBin(UInt value, ContextModel& context) {
  cabacEncoder.encodeBin(value, context);
}


Void TDbrLayer::encodeBinEP(UInt value) {
  cabacEncoder.encodeBinEP(value);
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


Void TDbrLayer::resetContexts(TComSlice& slice) {
  contexts.resetEntropy(&slice);
}


Void TDbrLayer::resetBudget() {
  budget = 0;
}


Void TDbrLayer::resetFlagCounts() {
  numCodedGt1Flags = 0;
  numCodedGt2Flags = 0;
}
