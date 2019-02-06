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
 *  \file     TDbrXmlWriter.h
 *  \project  TAppDebraider
 *  \brief    Debraider XML writer class header
 */


#pragma once

#include <string>
#include <fstream>

#include "TLibCommon/CommonDef.h"
#include "TLibDecoder/NALread.h"


//! \ingroup TAppDebraider
//! \{


class TDbrXmlWriter {
private:
  // Stores an ostream for writing xml
  std::ostream* stream;

  // Allows the writer to be temporarily disabled
  Bool isWritableFlag;


public:
  // Constructors
  TDbrXmlWriter();
  TDbrXmlWriter(std::ostream& stream);


  // Stream management
  std::ostream* getStream();
  Void setStream(std::ostream* stream);


  // Stream enabled/disabled status management
  Bool isWritable() const;
  Void enableWriting();
  Void disableWriting();


  // Tag output
  Void writeOpenTag(const std::string& tagName);
  Void writeCloseTag(const std::string& tagName);


  // Nalu output
  Void writeOpenTag(const InputNALUnit& nalu);


  // Value output
  template<typename T>
  Void writeValueTag(const std::string& tagName, T body) {
    if (isWritableFlag) {
      *stream <<
        "<" << tagName << ">" <<
        body <<
        "</" << tagName << ">\n";
    }
  }


  // Value output with bits
  template<typename T>
  Void writeValueTag(const std::string& tagName, T body, UInt numBits) {
    if (isWritableFlag) {
      *stream <<
        "<" << tagName << " bits=\"" << numBits << "\">" <<
        body <<
        "</" << tagName << ">\n";
    }
  }
};


//! \}
