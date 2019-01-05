#include "TDbrXmlWriter.h"



TDbrXmlWriter::TDbrXmlWriter() :
  isWritable(true) {
}


TDbrXmlWriter::TDbrXmlWriter(std::ostream& stream) :
  stream(&stream) {
}


std::ostream* TDbrXmlWriter::getStream() {
  return stream;
}


Void TDbrXmlWriter::setStream(std::ostream* stream) {
  this->stream = stream;
}


Bool TDbrXmlWriter::isWritingEnabled() const {
  return isWritable;
}


Void TDbrXmlWriter::enableWriting() {
  isWritable = true;
}


Void TDbrXmlWriter::disableWriting() {
  isWritable = false;
}


Void TDbrXmlWriter::writeOpenTag(const std::string& tagName) {
  if (isWritable) {
    assert(stream != nullptr);
    *stream << "<" << tagName << ">\n";
  }
}


Void TDbrXmlWriter::writeCloseTag(const std::string& tagName) {
  if (isWritable) {
    assert(stream != nullptr);
    *stream << "</" << tagName << ">\n";
  }
}


Void TDbrXmlWriter::writeOpenTag(const InputNALUnit& nalu) {
  if (isWritable) {
    *stream <<
      "<nalu" <<
      " type=\"" << nalu.m_nalUnitType << "\"" <<
      " layer=\"" << nalu.m_nuhLayerId << "\"" <<
      " time=\"" << nalu.m_temporalId << "\">\n";
  }
}
