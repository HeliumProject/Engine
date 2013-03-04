#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Slice.h"

namespace Helium
{
    namespace SceneGraph {
        class Scene;
    }

    class EntityProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT( Helium::EntityProxy, Reflect::Object );

        EntityProxy();
        EntityProxy( EntityDefinitionPtr definition );

        EntityDefinition* GetDefinition() const { return m_EntityDefinition; }
        SceneGraph::Scene* GetScene() const { return m_Scene; }
        Entity* GetRuntimeEntity() const { return m_Entity; }

        bool AttachToScene( SceneGraph::Scene* scene );
        bool DetachFromScene();

    private:
        EntityDefinitionPtr m_EntityDefinition;
        SceneGraph::Scene* m_Scene;
        Entity* m_Entity;
        //Helium::WeakPtr<Entity> m_EntityWeakPtr;

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
