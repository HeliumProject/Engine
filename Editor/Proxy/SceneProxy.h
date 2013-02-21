
#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/World.h"

#include "Editor/Proxy/EntityProxy.h"

namespace Helium
{
    class SceneProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::SceneProxy, Reflect::Object);

        // Reference to entity
        void Invalidate();

        void Initialize( SceneDefinition * pSceneDefinition );
    private:
        Helium::StrongPtr<SceneDefinition> m_SceneDefinition;
		
        Helium::DynamicArray<EntityProxyPtr> m_EntityProxies;
    };
    typedef Helium::StrongPtr<SceneProxy> SceneProxyPtr;
}
