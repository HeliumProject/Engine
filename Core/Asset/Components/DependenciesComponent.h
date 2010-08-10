#pragma once

#include "Core/API.h"
#include "Foundation/File/Path.h"

#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        class CORE_API DependenciesComponent : public Component::ComponentBase
        {
        public:

            std::set< Helium::Path > m_Paths;

        public:
            REFLECT_DECLARE_CLASS( DependenciesComponent, ComponentBase );

            static void EnumerateClass( Reflect::Compositor<DependenciesComponent>& comp );

            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE { return Component::ComponentUsages::Class; }
        };

        typedef Helium::SmartPtr< DependenciesComponent > DependenciesComponentPtr;
    }
}