#pragma once

#include "Graphics/Graphics.h"

namespace Helium
{
    HELIUM_GRAPHICS_API void PreRegisterGraphicsTypes();
    HELIUM_GRAPHICS_API void PostRegisterGraphicsTypes();

    HELIUM_GRAPHICS_API void RegisterGraphicsEnums();
    HELIUM_GRAPHICS_API void UnregisterGraphicsEnums();

    HELIUM_GRAPHICS_API void RegisterGraphicsStructures();
    HELIUM_GRAPHICS_API void UnregisterGraphicsStructures();
}