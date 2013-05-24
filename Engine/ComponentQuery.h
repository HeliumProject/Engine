
#pragma once

#include "Engine/Engine.h"
#include "Foundation/DynamicArray.h"
#include "Engine/Components.h"

namespace Helium
{
    typedef void (*ComponentTupleCallback)(DynamicArray<Component *> &tuple);
    
    void HELIUM_ENGINE_API QueryComponentsInternal(ComponentManager &rManager, const Components::TypeId *types, size_t typesCount, ComponentTupleCallback callback);
    
	template <class A, class B, void (*F)(A *, B *)>
    void TupleHandler(DynamicArray<Component *> &components)
    {
        F(
			static_cast<A *>(components[0]), 
			static_cast<B *>(components[1]));
    }
	
	template <class A, class B, class C, void (*F)(A *, B *, C *)>
    void TupleHandler(DynamicArray<Component *> &components)
    {
        F(
			static_cast<A *>(components[0]), 
			static_cast<B *>(components[1]), 
			static_cast<C *>(components[2]));
    }
	
	template <class A, void (*F)(A *)>
	inline void QueryComponents( World *pWorld )
	{ 
		for (ImplementingComponentIterator<A> iter( *pWorld->GetComponentManager() ); iter.GetBaseComponent(); iter.Advance())
		{
			F( *iter );
		}
	}

	template <class A, class B, void (*F)(A *, B *)>
	inline void QueryComponents( World *pWorld )
	{
		static Components::TypeId types[] = {
			Components::GetType<A>(),
			Components::GetType<B>()
		};

		QueryComponentsInternal( *pWorld->GetComponentManager(), types, HELIUM_ARRAY_COUNT(types), TupleHandler<A, B, F> );
	}
	
	template <class A, class B, class C, void (*F)(A *, B *, C *)>
	inline void QueryComponents( World *pWorld )
	{
		static Components::TypeId types[] = {
			Components::GetType<A>(),
			Components::GetType<B>(),
			Components::GetType<C>()
		};

		QueryComponentsInternal( *pWorld->GetComponentManager(), types, HELIUM_ARRAY_COUNT(types), TupleHandler<A, B, C, F> );
	}
}
