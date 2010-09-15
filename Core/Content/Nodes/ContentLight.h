#pragma once

#include "Core/API.h"
#include "Core/Content/Nodes/ContentInstance.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API Light HELIUM_ABSTRACT : public Instance
        {
        public:
            Math::HDRColor3 m_Color;

            Light ( const Helium::TUID& id = Helium::TUID::Generate() )
                : Instance( id )
            {

            }

            REFLECT_DECLARE_ABSTRACT(Light, Instance);

            static void EnumerateClass( Reflect::Compositor<Light>& comp );
        };

        typedef Helium::SmartPtr<Light> LightPtr;
        typedef std::vector<LightPtr> V_Light;
    }
}