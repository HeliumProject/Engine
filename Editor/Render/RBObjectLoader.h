#pragma once

#include "Editor/Render/ObjectLoader.h"

namespace Helium
{
    namespace Content
    {
        class RBObjectLoader : public Render::ObjectLoader
        {
        public:
            RBObjectLoader();
            virtual ~RBObjectLoader();

            u32 ParseFile(const tchar* fname,bool winding);
        };
    }
}