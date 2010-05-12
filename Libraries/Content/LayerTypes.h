#pragma once

#include <iostream>

namespace Content
{
  namespace LayerTypes
  {
    enum LayerType
    {
      LT_GeneralPurpose,
      LT_Lighting,
      LT_Unknown,
      LT_COUNT
    };  
  }

  typedef LayerTypes::LayerType LayerType;
}