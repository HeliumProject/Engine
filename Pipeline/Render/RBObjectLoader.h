#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Render/ObjectLoader.h"

namespace Helium
{
    namespace Render
    {
        class PIPELINE_API RBObjectLoader : public ObjectLoader
        {
        public:
            RBObjectLoader();
            virtual ~RBObjectLoader();

            uint32_t ParseFile(const tchar_t* fname,bool winding);
        };
    }
}