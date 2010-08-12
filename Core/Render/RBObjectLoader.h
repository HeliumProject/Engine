#pragma once

#include "Core/API.h"
#include "Core/Render/ObjectLoader.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API RBObjectLoader : public Render::ObjectLoader
        {
        public:
            RBObjectLoader();
            virtual ~RBObjectLoader();

            u32 ParseFile(const tchar* fname,bool winding);
        };
    }
}