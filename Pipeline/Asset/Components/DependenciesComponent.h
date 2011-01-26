#pragma once

#include "Pipeline/API.h"
#include "Foundation/File/Path.h"

#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API DependenciesComponent : public Component::ComponentBase
        {
        public:

            std::set< Helium::Path > m_Paths;

        public:
            REFLECT_DECLARE_OBJECT( DependenciesComponent, ComponentBase );

            static void PopulateComposite( Reflect::Composite& comp );

            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE { return Component::ComponentUsages::Class; }
        };

        typedef Helium::SmartPtr< DependenciesComponent > DependenciesComponentPtr;
    }
}