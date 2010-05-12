#pragma once

#include "API.h" 
#include "Common/InitializerStack.h"

namespace Luna
{
  namespace LunaCharacter
  {
    extern Nocturnal::InitializerStack g_RegisteredTypes;
    LUNA_CHARACTER_API void InitializeModule(); 
    LUNA_CHARACTER_API void CleanupModule(); 
  }
}
