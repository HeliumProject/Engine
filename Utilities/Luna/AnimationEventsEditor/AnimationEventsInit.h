#pragma once

// Includes
#include "API.h"
#include "Common/InitializerStack.h"

namespace Luna
{
  namespace LunaAnimationEvents
  {
    extern Nocturnal::InitializerStack g_RegisteredTypes;
    LUNA_ANIMATION_EVENTS_API void InitializeModule();
    LUNA_ANIMATION_EVENTS_API void CleanupModule();
  }
}
