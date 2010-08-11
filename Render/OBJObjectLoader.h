#pragma once

#include "Render/API.h"
#include "Render/ObjectLoader.h"

namespace Helium
{
    namespace Render
    {
        class RENDER_API OBJObjectLoader : public ObjectLoader
        {
        public:
            u32 ParseFile(const tchar* fname, bool winding = false);
        };
    }
}
