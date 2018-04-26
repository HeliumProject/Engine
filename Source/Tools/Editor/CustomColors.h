#pragma once

#include "Platform/Types.h"

#include <string>

#include <wx/cmndata.h>

namespace Helium
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
    std::string GetConfigKey();

    // Converts the custom colors located in colorData into a string that can be loaded later.
    std::string Save( wxColourData& colorData );

    // Loads custom colors (from a string that was previously made with Save) into colorData.
    void Load( wxColourData& colorData, const std::string& info );
  }
}
