#pragma once

#include "Luna/API.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Helper functions for saving/loading custom colors that are shown in
  // a color picker dialog (wxColourDialog).
  // 
  namespace CustomColors
  {
    enum CustomColor
    {
      NumColors = 16, // Total number of custom colors in a wxColorData
    };

    // Gets the location where custom colors should be saved in the registry by default.
    LUNA_CORE_API std::string GetDefaultRegistryKey();

    // Converts the custom colors located in colorData into a string that can be loaded later.
    LUNA_CORE_API std::string Save( wxColourData& colorData );

    // Loads custom colors (from a string that was previously made with Save) into colorData.
    LUNA_CORE_API void Load( wxColourData& colorData, const std::string& info );
  }
}
