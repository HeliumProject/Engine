
#pragma once

#include "Engine/Engine.h"
#include "Foundation/DynamicArray.h"
#include "Engine/Components.h"

namespace Helium
{
    typedef void (*ComponentTupleCallback)(DynamicArray<Component *> &tuple);
    
    struct ITupleCallback;

    void HELIUM_ENGINE_API QueryComponents(const DynamicArray<Components::TypeId> &types, ITupleCallback *emit_tuple_callback);
    
    struct ITupleCallback
    {
        virtual void HandleTupleInternal(DynamicArray<Component *> &components) = 0;
    };

    template <class A, class B>
    class ComponentQuery : public ITupleCallback
    {
    public:
        void Run()
        {
            DynamicArray<Components::TypeId> types;
            types.Add(Components::GetType<A>());
            types.Add(Components::GetType<B>());

            QueryComponents(types, this);
        }

    protected:
        virtual void HandleTuple(A *, B *) = 0;

    private:
        void HandleTupleInternal(DynamicArray<Component *> &components)
        {
            HandleTuple(static_cast<A *>(components[0]), static_cast<B *>(components[1]));
        }
    };
}