#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    HELIUM_FRAMEWORK_API void PreRegisterFrameworkTypes();
    HELIUM_FRAMEWORK_API void PostRegisterFrameworkTypes();

    HELIUM_FRAMEWORK_API void RegisterFrameworkEnums();
    HELIUM_FRAMEWORK_API void UnregisterFrameworkEnums();

    HELIUM_FRAMEWORK_API void RegisterFrameworkStructures();
    HELIUM_FRAMEWORK_API void UnregisterFrameworkStructures();
}