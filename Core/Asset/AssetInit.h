#pragma once

#include "Core/API.h"

#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Asset
    {
        void CORE_API Initialize();
        void CORE_API Cleanup();

        // List of all asset class type ids
        extern CORE_API std::vector< i32 > g_AssetClassTypes;
    }
}