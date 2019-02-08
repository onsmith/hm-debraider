#include "TDbrLayer.h"


TDbrLayer::TDbrLayer() :
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


Void TDbrLayer::useBits(UInt numBits) {
  budget += (numBits << FIXED_POINT_PRECISION);
}
