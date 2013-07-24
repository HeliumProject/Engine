
#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/World.h"

#include "Editor/Proxy/EntityProxy.h"

namespace Helium
{
    class SceneProxy;
    typedef Helium::StrongPtr<SceneProxy> SceneProxyPtr;
    typedef Helium::WeakPtr<SceneProxy> SceneProxyWPtr;

    class SceneProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_CLASS(Helium::SceneProxy, Reflect::Object);

        // Reference to entity
        void Invalidate();

        void Initialize( SceneDefinition * pSceneDefinition, SceneProxy *pParentScene );
        World *GetWorld()
        {
            if (m_World)
            {
                return m_World;
            }

            if (m_ParentScene)
            {
                return m_ParentScene->GetWorld();
            }

            HELIUM_ASSERT(0);
            return 0;
        }
    private:
        Helium::StrongPtr<SceneDefinition> m_SceneDefinition;
        Helium::DynamicArray<EntityProxyPtr> m_EntityProxies;

        SceneProxyWPtr m_ParentScene;
        WorldPtr m_World;
    };
}
