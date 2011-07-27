#pragma once

#include "Platform/Types.h"

namespace Helium
{
  static const int32_t FIX_1_082392200=277;     // .8 fixed (1.082392200)
  static const int32_t FIX_1_414213562=362;     // .8 fixed (1.414213562)
  static const int32_t FIX_1_847759065=473;     // .8 fixed (1.847759065)
  static const int32_t FIX_2_613125930=669;     // .8 fixed (2.613125930)

  //-----------------------------------------------------------------------------
  struct JpegType                        // some type definitions (for coherence)
  {
    uint16_t Rows;                           // image height
    uint16_t Cols;                           // image width
    uint16_t SamplesY;                       // sampling ratios
    uint16_t SamplesCbCr;
    uint16_t QuantTableY;                    // quantization table numbers
    uint16_t QuantTableCbCr;
    uint16_t HuffDCTableY;                   // huffman table numbers
    uint16_t HuffDCTableCbCr;
    uint16_t HuffACTableY;
    uint16_t HuffACTableCbCr;
    uint16_t NumComp;                        // number of components
  };

  //-----------------------------------------------------------------------------
  struct JPGHuffmanEntry                 // a type for huffman tables
  {
    uint16_t Index;
    int16_t Code;
    uint16_t Length;
  };
}