#pragma once

// Includes
#include "API.h"
#include "Common/InitializerStack.h"

namespace Luna
{
  namespace LunaCinematicEvents
  {
    extern Nocturnal::InitializerStack g_RegisteredTypes ;
    LUNA_CINEMATIC_EVENTS_API void InitializeModule();
    LUNA_CINEMATIC_EVENTS_API void CleanupModule();
  }
}
