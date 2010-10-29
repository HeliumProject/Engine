#pragma once

#include "Core/API.h"
#include "Core/Render/ObjectLoader.h"

namespace Helium
{
    namespace Render
    {
        class CORE_API RBObjectLoader : public ObjectLoader
        {
        public:
            RBObjectLoader();
            virtual ~RBObjectLoader();

            uint32_t ParseFile(const tchar* fname,bool winding);
        };
    }
}