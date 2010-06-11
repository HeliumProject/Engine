#include "API.h"

#include "Platform/Types.h"

namespace MayaCore
{
  MAYACORE_API extern u32 g_InitCount;
  MAYACORE_API void Initialize();
  MAYACORE_API void Cleanup();
}