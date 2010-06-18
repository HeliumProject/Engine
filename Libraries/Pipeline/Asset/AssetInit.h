#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Registry.h"

namespace Asset
{
  void PIPELINE_API Initialize();
  void PIPELINE_API Cleanup();

  // List of all asset class type ids
  extern PIPELINE_API V_i32 g_AssetClassTypes;
}