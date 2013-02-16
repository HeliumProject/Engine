
#pragma once

#include "Foundation/Map.h"

#include "Reflect/Object.h"

#include "Framework/EntityDefinition.h"
//#include "Framework/Entity.h"

namespace Helium
{
    class EntityProxy
    {
        // Reference to entity
        Helium::StrongPtr<EntityDefinition> m_EntityDefinition;
        Helium::StrongPtr<Entity> m_EntityStrongPtr;
        Helium::WeakPtr<Entity> m_EntityWeakPtr;

        // References to component
        struct ComponentInstance
        {
            Helium::ComponentDefinitionPtr m_Definition;
            Helium::ComponentPtr<Helium::Component> m_Instance;
        };
        Helium::Map<Helium::Name, ComponentInstance> m_Components;

        void Invalidate();
    };
}
