#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Asset
    {
        void PIPELINE_API Initialize();
        void PIPELINE_API Cleanup();

        // List of all asset class type ids
        extern PIPELINE_API std::vector< int32_t > g_AssetClassTypes;
    }
}