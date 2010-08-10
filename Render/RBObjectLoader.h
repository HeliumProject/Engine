#pragma once

#include "Render/API.h"
#include "Render/ObjectLoader.h"

namespace Helium
{
    namespace Content
    {
        class RENDER_API RBObjectLoader : public Render::ObjectLoader
        {
        public:
            RBObjectLoader();
            virtual ~RBObjectLoader();

            u32 ParseFile(const tchar* fname,bool winding);
        };
    }
}