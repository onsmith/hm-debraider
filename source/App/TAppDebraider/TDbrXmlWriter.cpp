#include "TDbrXmlWriter.h"



TDbrXmlWriter::TDbrXmlWriter() :
  isWritableFlag(true) {
}


TDbrXmlWriter::TDbrXmlWriter(std::ostream& stream) :
  isWritableFlag(true),
  stream(&stream) {
}


std::ostream* TDbrXmlWriter::getStream() {
  return stream;
}


Void TDbrXmlWriter::setStream(std::ostream* stream) {
  this->stream = stream;
}


Bool TDbrXmlWriter::isWritable() const {
  return isWritableFlag;
}


Void TDbrXmlWriter::enableWriting() {
  isWritableFlag = true;
}


Void TDbrXmlWriter::disableWriting() {
  isWritableFlag = false;
}


Void TDbrXmlWriter::writeOpenTag(const std::string& tagName) {
  if (isWritableFlag) {
    assert(stream != nullptr);
    *stream << "<" << tagName << ">\n";
  }
}


Void TDbrXmlWriter::writeCloseTag(const std::string& tagName) {
  if (isWritableFlag) {
    assert(stream != nullptr);
    *stream << "</" << tagName << ">\n";
  }
}


Void TDbrXmlWriter::writeOpenTag(const InputNALUnit& nalu) {
  if (isWritableFlag) {
    *stream <<
      "<nalu" <<
      " type=\"" << nalu.m_nalUnitType << "\"" <<
      " layer=\"" << nalu.m_nuhLayerId << "\"" <<
      " time=\"" << nalu.m_temporalId << "\">\n";
  }
}
