
#pragma once

#include "Foundation/Map.h"
#include "Reflect/Object.h"
#include "Framework/Slice.h"
#include "Editor/Proxy/EntityProxy.h"


namespace Helium
{
    class SliceProxy : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::SliceProxy, Reflect::Object);

        Slice *GetSlice() const { return m_SliceWeakPtr.Get(); }

        void Invalidate();

    private:
        // Reference to entity
        Helium::StrongPtr<SliceDefinition> m_SliceDefinition;
        Helium::StrongPtr<Slice> m_SliceStrongPtr;
        Helium::WeakPtr<Slice> m_SliceWeakPtr;

        // References to component
        Helium::DynamicArray<EntityProxyPtr> m_EntityProxies;
    };
    typedef Helium::StrongPtr<SliceProxy> SliceProxyPtr;
}
