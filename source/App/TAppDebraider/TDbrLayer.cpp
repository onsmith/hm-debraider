#include "TDbrLayer.h"


TDbrLayer::TDbrLayer() {
  cabacEncoder.init(&bitstream);
}


Void TDbrLayer::encodeBin(UInt value, ContextModel& context) {
  cabacEncoder.encodeBin(value, context);
}


Void TDbrLayer::encodeBinEP(UInt value) {
  cabacEncoder.encodeBinEP(value);
}
