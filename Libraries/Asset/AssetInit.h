#pragma once

#include "API.h"

#include "Reflect/Registry.h"
#include "Finder/Finder.h"

namespace Asset
{
  void ASSET_API Initialize();
  void ASSET_API Cleanup();

  // List of all asset class type ids
  extern ASSET_API V_i32 g_AssetClassTypes;
}