#pragma once

#include "Platform/Types.h"

namespace Helium
{
  namespace AnimationSettings
  {
    namespace Compression
    {
      static const u32 SCALE_SCALER     = 2048;  // 1:4:11 = +-16x to 0.00049x
      static const u32 SCALE_SCALER_LOG = 11;    // 

      static const u32 ROTATION_SCALER     = 32768; // 1:15
      static const u32 ROTATION_SCALER_LOG = 15;    // 1:15

      static const u32 TRANSLATION_SCALER     = 2048;  // 1:4:11 = +-16m to 0.48mm
      static const u32 TRANSLATION_SCALER_LOG = 11;    // 
    }
  }
}