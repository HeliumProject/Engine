#pragma once

#include "API.h" 
#include "Stream.h" 

namespace Reflect
{
  // returns the size of the compressed data. 
  int CompressToStream(Reflect::Stream& reflectStream, const char* data, u32 size);

  // returns number of bytes written to the output (after decompression)
  int DecompressFromStream(Reflect::Stream& reflectStream, int inputBytes, char* output, int outputBytes);

}