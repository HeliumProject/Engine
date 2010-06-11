#pragma once

#include "Platform/Types.h"

namespace IG
{
  static const i32 FIX_1_082392200=277;     // .8 fixed (1.082392200)
  static const i32 FIX_1_414213562=362;     // .8 fixed (1.414213562)
  static const i32 FIX_1_847759065=473;     // .8 fixed (1.847759065)
  static const i32 FIX_2_613125930=669;     // .8 fixed (2.613125930)

  //-----------------------------------------------------------------------------
  struct JpegType                        // some type definitions (for coherence)
  {
    u16 Rows;                           // image height
    u16 Cols;                           // image width
    u16 SamplesY;                       // sampling ratios
    u16 SamplesCbCr;
    u16 QuantTableY;                    // quantization table numbers
    u16 QuantTableCbCr;
    u16 HuffDCTableY;                   // huffman table numbers
    u16 HuffDCTableCbCr;
    u16 HuffACTableY;
    u16 HuffACTableCbCr;
    u16 NumComp;                        // number of components
  };

  //-----------------------------------------------------------------------------
  struct JPGHuffmanEntry                 // a type for huffman tables
  {
    u16 Index;
    i16 Code;
    u16 Length;
  };
}