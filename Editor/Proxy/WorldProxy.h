
#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/World.h"
#include "Editor/Proxy/SliceProxy.h"

namespace Helium
{
    class WorldProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::WorldProxy, Reflect::Object);

        // Reference to entity
        World *GetWorld() const { return m_WorldWeakPtr.Get(); }
        void Invalidate();
        void Initialize( WorldDefinition * pWorldDefinition );
    private:
        Helium::StrongPtr<WorldDefinition> m_EntityDefinition;
        Helium::StrongPtr<World> m_WorldStrongPtr;
        Helium::WeakPtr<World> m_WorldWeakPtr;
        
        Helium::DynamicArray<SliceProxyPtr> m_SliceProxies;
    };
    typedef Helium::StrongPtr<WorldProxy> WorldProxyPtr;
}
