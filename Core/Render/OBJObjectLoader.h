#pragma once

#include "Core/API.h"
#include "Core/Render/ObjectLoader.h"

namespace Helium
{
    namespace Render
    {
        class CORE_API OBJObjectLoader : public ObjectLoader
        {
        public:
            u32 ParseFile(const tchar* fname, bool winding = false);
        };
    }
}

