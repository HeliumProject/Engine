
#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Slice.h"

namespace Helium
{
    class EntityProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_CLASS(Helium::EntityProxy, Reflect::Object);

        void Invalidate();

    private:
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
    };
    typedef Helium::StrongPtr<EntityProxy> EntityProxyPtr;
}
