#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Render/ObjectLoader.h"

namespace Helium
{
    namespace Render
    {
        class PIPELINE_API OBJObjectLoader : public ObjectLoader
        {
        public:
            uint32_t ParseFile(const tchar_t* fname, bool winding = false);
        };
    }
}

