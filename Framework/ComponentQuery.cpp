
#include "FrameworkPch.h"
#include "Framework/ComponentQuery.h"
#include <limits>
#include <vector>

using namespace Helium;

struct FoundComponentList
{
	Component *m_Component;
	size_t m_TypeIndex;
	size_t m_Count;
	Components::TypeId m_TypeId;
};

bool SortFoundComponentList(const FoundComponentList &lhs, const FoundComponentList &rhs)
{
	return lhs.m_Count < rhs.m_Count;
}

void EmitTuples(DynamicArray<Component *> &tuple, std::vector<FoundComponentList> &found_components, size_t type_index, ComponentTupleCallback emit_tuple_callback)
{
	Component *c = found_components[ type_index ].m_Component;
	HELIUM_ASSERT( c );
	do
	{
		tuple[found_components[type_index].m_TypeIndex] = c;
		
		if (type_index < found_components.size() - 1)
		{
			EmitTuples(tuple, found_components, ++type_index, emit_tuple_callback);
		}
		else
		{
			emit_tuple_callback(tuple);
		}
	} 
	while ( ( c = c->GetNextComponent() ) );
}

void Helium::QueryComponentsInternal(ComponentManager &rManager, const Components::TypeId *types, size_t typesCount, ComponentTupleCallback emit_tuple_callback)
{
	// If no types to query, do nothing
	if (!typesCount)
	{
		return;
	}
	
	// Prepare the structure that will help us emit all permutations of found components
	std::vector<FoundComponentList> found_components;
	found_components.resize(typesCount);
	
	// Find the component with the least instances
	for (size_t index = 0; index < typesCount; ++index)
	{
		found_components[index].m_TypeIndex = index;
		found_components[index].m_TypeId = types[index];
		found_components[index].m_Count = rManager.CountAllocatedComponentsThatImplement(types[index]);
		
		// Bail if any component type doesn't exist
		if (!found_components[index].m_Count)
		{
			return;
		}
	}

	// Sort the types by commonality
	std::sort(found_components.begin(), found_components.end(), SortFoundComponentList);
	
	const DynamicArray< Components::TypeId > &implementing_types = Components::GetTypeData( found_components[0].m_TypeId )->m_ImplementingTypes;
	
	// For every component
	for ( ComponentIteratorBase iterator(rManager, implementing_types); iterator.GetBaseComponent(); iterator.Advance() )
	{
		Component *outer_component = iterator.GetBaseComponent();

		found_components[0].m_Component = outer_component;

		ComponentCollection *collection = outer_component->GetComponentCollection();

		// Walk the other types we need components of
		bool emit_tuples = true;
		for (size_t type_index = 1; type_index < found_components.size(); ++type_index)
		{
			found_components[type_index].m_Component = collection->GetFirst( found_components[type_index].m_TypeId );
			if ( !found_components[type_index].m_Component )
			{
				emit_tuples = false;
				break;
			}
		}
		
		if (emit_tuples)
		{
			DynamicArray<Component *> tuple;
			tuple.Resize(typesCount);
			tuple[found_components[0].m_TypeIndex] = outer_component;
			EmitTuples(tuple, found_components, 1, emit_tuple_callback);
		}
	}
}
