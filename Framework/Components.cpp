
#include "FrameworkPch.h"
#include "Framework/Components.h"
#include "Framework/SystemDefinition.h"

#include "Foundation/Numeric.h"
#include "Reflect/TranslatorDeduction.h"
#include "Engine/Asset.h"

HELIUM_DEFINE_BASE_STRUCT(Helium::Component);

using namespace Helium;
using namespace Helium::Components;

////////////////////////////////////////////////////////////////////////
//       System Implementation
////////////////////////////////////////////////////////////////////////

#if HELIUM_HEAP
Helium::DynamicMemoryHeap      Private::g_ComponentAllocator;
#endif

namespace
{
	int32_t                    g_ComponentsInitCount = 0;
	int32_t                    g_ComponentManagerInstanceCount = 0;
	DynamicArray<TypeData *>   g_ComponentTypes;
	ComponentPtrBase*          g_ComponentPtrRegistry[COMPONENT_PTR_CHECK_FREQUENCY];
	uint16_t                   g_ComponentProcessPendingDeletesCallCount = 0;
}

ComponentRegistrar<Helium::Component, void> Helium::Component::s_ComponentRegistrar("Helium::Component");

void Components::Initialize( SystemDefinition *pSystemDefinition )
{
	// Register base component with reflect
	if ( !g_ComponentsInitCount )
	{
		if ( pSystemDefinition )
		{
			DynamicArray< ComponentTypeConfig > &rTypeConfigs = pSystemDefinition->m_ComponentTypeConfigs;
			for (DynamicArray< ComponentTypeConfig >::Iterator configIter = rTypeConfigs.Begin(); 
				configIter != rTypeConfigs.End(); ++configIter)
			{
				bool found = false;
				for (DynamicArray< TypeData * >::Iterator componentTypeIter = g_ComponentTypes.Begin(); 
					componentTypeIter != g_ComponentTypes.End(); ++componentTypeIter)
				{
					TypeData &rTypeData = **componentTypeIter;
					if (rTypeData.m_Name == configIter->m_ComponentTypeName)
					{
						rTypeData.m_DefaultCount = configIter->m_PoolSize;
						found = true;
						break;
					}
				}

				if (!found)
				{
					HELIUM_TRACE(
						TraceLevels::Warning,
						"Components::Initialize - SystemDefinition specifies pool size for component type '%s', but this component type was not in the "
						"component type list. Check spelling and that the component in question was registered.",
						*configIter->m_ComponentTypeName);
				}
			}
		}
	}

	++g_ComponentsInitCount;
}

void Components::Cleanup()
{
	--g_ComponentsInitCount;

	if ( !g_ComponentsInitCount )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "Components shutting down.\n" ));
		HELIUM_ASSERT( !g_ComponentManagerInstanceCount );

		for (DynamicArray<TypeData *>::Iterator iter = g_ComponentTypes.Begin();
			iter != g_ComponentTypes.End(); ++iter)
		{
			TypeData *data = *iter;
			data->m_DefaultCount = 0;
			data->m_ImplementedTypes.Clear();
			data->m_ImplementingTypes.Clear();
			data->m_Structure = NULL;
			data->m_TypeId = Invalid<TypeId>();
		}

		g_ComponentTypes.Clear();
	}
}

TypeId Components::RegisterType( 
	const Reflect::MetaStruct *pStructure, 
	TypeData &rTypeData, 
	TypeData *pBaseType, 
	uint16_t defaultCount )
{
	// Some validation of parameters/state
	HELIUM_ASSERT( pStructure );
	HELIUM_ASSERT( defaultCount >= 0 );
	HELIUM_ASSERT( !pBaseType || pBaseType->m_TypeId != Invalid<Components::TypeId>() );
	HELIUM_ASSERT( pStructure->m_Size <= NumericLimits<ComponentSizeType>::Maximum );
	
	// Cache a reference to the type
	g_ComponentTypes.New(&rTypeData);

	// Set type id on static member of the class so we can easily get this data later
	// Assert that we haven't already registered this type and then set up the data
	TypeId type_id = (uint16_t)g_ComponentTypes.GetSize() - 1;
	HELIUM_ASSERT(rTypeData.m_TypeId == Invalid<TypeId>());
	HELIUM_ASSERT( IsValid<TypeId>(type_id) );
	rTypeData.m_TypeId = type_id;
	rTypeData.m_Name = Name( pStructure->m_Name );

	// Update bookkeeping fields
	rTypeData.m_Structure = pStructure;
	rTypeData.m_DefaultCount = defaultCount;

	// We implement ourselves
	g_ComponentTypes[type_id]->m_ImplementingTypes.New( type_id );
	g_ComponentTypes[type_id]->m_ImplementedTypes.New( type_id );

	// If we have a parent
	if (pBaseType)
	{
		DynamicArray<TypeId> &base_implemented_types = g_ComponentTypes[pBaseType->m_TypeId]->m_ImplementedTypes;

		// Add base to implemented types, and ourselves to base's implementing types
		rTypeData.m_ImplementedTypes.New(pBaseType->m_TypeId);
		g_ComponentTypes[pBaseType->m_TypeId]->m_ImplementingTypes.New(type_id);

		// For all the base's types, 
		for (DynamicArray<TypeId>::Iterator iter = base_implemented_types.Begin();
			iter != base_implemented_types.End(); ++iter)
		{
			rTypeData.m_ImplementedTypes.New(*iter);
			g_ComponentTypes[*iter]->m_ImplementingTypes.New(type_id);
		}
	}

	// Return the component type's id
	return type_id;
}

const TypeData* Components::GetTypeData( TypeId type )
{
	return g_ComponentTypes[ type ];
}

ComponentManager *Components::CreateManager( World *pWorld )
{
	HELIUM_ASSERT( g_ComponentsInitCount );
	++g_ComponentsInitCount;
	ComponentManagerPtr return_value( new ComponentManager(pWorld) );

	return return_value.Release();
}

#define PAD_VALUE( _VALUE , _PAD ) ((_VALUE + (_PAD-1)) & (~(_PAD-1)))

Pool * Pool::CreatePool( ComponentManager *pComponentManager, const TypeData &rTypeData, ComponentIndex count )
{
	if ( !count )
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			"Components::Pool::CreatePool - Creating null component pool for type %s\n",
			rTypeData.m_Structure->m_Name);
		return NULL;
	}

	ComponentSizeType componentSize = rTypeData.GetSize();
	HELIUM_ASSERT( componentSize );
	componentSize = PAD_VALUE(componentSize, HELIUM_SIMD_ALIGNMENT);

	size_t poolSize = PAD_VALUE( sizeof( Components::Pool ), HELIUM_SIMD_ALIGNMENT );
	size_t memoryRequried = poolSize + componentSize * count;
	Pool *pool = (Pool *)g_ComponentAllocator.AllocateAligned( POOL_ALIGN_SIZE, memoryRequried );
	new(pool) Pool();
	
	pool->m_ParallelData = HELIUM_NEW_A( g_ComponentAllocator, DataParallel, count );

	pool->m_World = pComponentManager->GetWorld();
	pool->m_ComponentManager = pComponentManager;
	pool->m_Type = &rTypeData;
	pool->m_TypeId = rTypeData.m_TypeId;
	pool->m_ComponentSize = componentSize;
	pool->m_FirstUnallocatedIndex = 0;
	pool->m_ComponentOffset = rTypeData.GetOffsetOfComponent();
		
	pool->m_Roster.Resize( count );

	for (uint16_t i = 0; i < count; ++i)
	{
		Component *component = pool->GetComponent( i );
		pool->m_Roster[i] = component;

		uintptr_t offset = (static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(component) & POOL_ALIGN_SIZE_MASK) - reinterpret_cast<uintptr_t>(pool)) / HELIUM_COMPONENT_POOL_ALIGN_SIZE;
		HELIUM_ASSERT(offset <= NumericLimits<uint16_t>::Maximum);
		HELIUM_ASSERT(offset);
		component->m_InlineData.m_OffsetToPoolStart = static_cast<uint16_t>(offset);
			
		component->m_InlineData.m_Owner = NULL;
		component->m_InlineData.m_Next = Invalid<ComponentIndex>();
		component->m_InlineData.m_Previous = Invalid<ComponentIndex>();
		component->m_InlineData.m_Delete = false;
		component->m_InlineData.m_Generation = 0;
		pool->m_ParallelData[i].m_Collection = NULL;
		pool->m_ParallelData[i].m_RosterIndex = i;

		HELIUM_ASSERT( Pool::GetPool( component ) == pool );
		HELIUM_ASSERT( Pool::GetPool( component )->GetComponentIndex( component ) == i );
		HELIUM_ASSERT( Pool::GetPool( component )->GetComponent( i ) == component );
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		"Components::Pool::CreatePool - [%5d] %s (%d bytes at %x)\n",
		count,
		rTypeData.m_Structure->m_Name,
		memoryRequried,
		pool);

	return pool;
}

void Pool::DestroyPool( Pool *pPool )
{   
	if (pPool->m_FirstUnallocatedIndex > 0)
	{
		HELIUM_TRACE( TraceLevels::Warning, TXT( "Found %d components of type %s allocated during component system shutdown!\n" ),
			pPool->m_FirstUnallocatedIndex,
			pPool->m_Type->m_Structure->m_Name);
	}

	HELIUM_DELETE_A( g_ComponentAllocator, pPool->m_ParallelData );
	pPool->~Pool();
	g_ComponentAllocator.FreeAligned( pPool );
	
}

void Pool::InsertIntoChain(Component *_insertee, ComponentIndex _insertee_index, Component *nextComponent)
{
	// If we are inserting into a 0-length chain do nothing
	if (nextComponent)
	{
		// Fix inserted node's next/previous pointers
		ComponentIndex nextIndex = GetComponentIndex( nextComponent );
		_insertee->m_InlineData.m_Next = nextIndex;
		ComponentIndex previous_index = GetPreviousIndex( nextComponent );
		//m_ParallelData[ _insertee_index ].m_Previous = previous_index;
		_insertee->m_InlineData.m_Previous = previous_index;

		// Fix previous component's next pointer
		if (previous_index != Invalid<uint16_t>())
		{
			GetComponent( previous_index )->m_InlineData.m_Next = _insertee_index;
		}

		// Fix next component's previous pointer
		//m_ParallelData[ nextIndex ].m_Previous = _insertee_index;
		nextComponent->m_InlineData.m_Previous = _insertee_index;
	}
}

void Pool::RemoveFromChain(Component *_component, ComponentIndex index)
{
	// Components were already unlinked. Multiple calls to delete the component are OK, so just bail
	if ( !m_ParallelData[ index ].m_Collection )
	{
		return;
	}

	ComponentIndex previous_index = GetPreviousIndex( index );

	Component *pNextComponent = GetComponent( _component->m_InlineData.m_Next );

	// If we have a previous node, repoint its next pointer to our next pointer
	if ( previous_index != Invalid<ComponentIndex>() )
	{
		GetComponent( previous_index )->m_InlineData.m_Next = _component->m_InlineData.m_Next;
	}
	else if ( _component->m_InlineData.m_Next != Invalid<uint16_t>() )
	{
		//m_ParallelData[ index ].m_Collection->m_Components[m_TypeId] = GetComponent( _component->m_InlineData.m_Next );
		m_ParallelData[ index ].m_Collection->m_Components[m_TypeId] = pNextComponent;
	}
	else
	{
		m_ParallelData[ index ].m_Collection->m_Components.Remove( m_TypeId );
	}

	// If we have a next node, repoint its previous pointer to our previous pointer
	if ( _component->m_InlineData.m_Next != Invalid<uint16_t>() )
	{
		//m_ParallelData[ _component->m_InlineData.m_Next ].m_Previous = m_ParallelData[ index ].m_Previous;
		pNextComponent->m_InlineData.m_Previous = _component->m_InlineData.m_Previous;
	}

	// wipe our node
	_component->m_InlineData.m_Next = Invalid<uint16_t>();
	//m_ParallelData[ index ].m_Previous = Invalid<uint16_t>();
	_component->m_InlineData.m_Previous = Invalid<uint16_t>();
}

Component* Pool::Allocate( IHasComponents *owner, ComponentCollection &collection )
{
	// Null owner is allowed

	// Do we have a free component to allocate?
	if (m_FirstUnallocatedIndex >= m_Roster.GetSize())
	{
		// Could not allocate the component because we ran out..
		HELIUM_ASSERT_MSG( false, TXT( "Could not allocate component of type %s for host %x. No free instances are available. Maximum instances: %d" ), 
			g_ComponentTypes[ m_TypeId ]->m_Structure->m_Name,
			owner,
			m_Roster.GetSize());
		return NULL;
	}

	// Find out where the component we should allocate is in the roster
	ComponentIndex roster_index = m_FirstUnallocatedIndex++;
	
	Component *component = m_Roster[roster_index];
	ComponentIndex component_index = GetComponentIndex( component );

	// Insert into chain
	Map<TypeId, Component *>::Iterator iter = collection.m_Components.Find(m_TypeId);
	if (iter != collection.m_Components.End())
	{
		InsertIntoChain(component, component_index, iter->Second());
		iter->Second() = component;
	}
	else
	{
		collection.m_Components.Insert(iter, Map<TypeId, Component *>::ValueType(m_TypeId, component));
	}

	//m_ParallelData[ component_index ].m_Owner =  owner;
	component->m_InlineData.m_Owner = owner;

	m_ParallelData[ component_index ].m_Collection = &collection;

	m_Type->Construct( component );
	HELIUM_ASSERT( component->m_InlineData.m_OffsetToPoolStart);

	return component;
}

void Pool::Free( Component *component )
{
	ComponentIndex index = GetComponentIndex( component );
	
	// Component is already freed or component doesn't have a good handle for some reason
	HELIUM_ASSERT( m_ParallelData[ index ].m_Collection );

	m_Type->Destruct( component );
	RemoveFromChain( component, index );
	
	// Increment generation to invalidate old handles
	++component->m_InlineData.m_Generation;
	component->m_InlineData.m_Delete = false;
	component->m_InlineData.m_Owner = NULL;

	m_ParallelData[ index ].m_Collection = NULL;

	// Get roster indices we will manipulate
	ComponentIndex used_roster_index = m_ParallelData[ index ].m_RosterIndex;
	HELIUM_ASSERT( m_FirstUnallocatedIndex );

	ComponentIndex freed_roster_index = --m_FirstUnallocatedIndex;

	// Move the roster index that we are freeing to the end, swapping with the end
	// index if necessary
	if (used_roster_index == freed_roster_index)
	{
		// Do nothing because the roster index we are freeing is at the end of the list
	}
	else
	{
		// Swap the roster values
		// - component index: this component's roster index
		// - other component's index: highest in-use component's roster index
		//   - i.e. used_roster_index < freed_roster_index
		Component *component_index = m_Roster[used_roster_index];
		Component *other_component_index = m_Roster[freed_roster_index];
		m_Roster[used_roster_index] = other_component_index;
		m_Roster[freed_roster_index] = component_index;

		// Swap the roster index of the highest in-use component and the recently freed component
		m_ParallelData[ index ].m_RosterIndex = freed_roster_index;
		m_ParallelData[ GetComponentIndex( other_component_index ) ].m_RosterIndex = used_roster_index;
	}
}

#if HELIUM_TOOLS
void Helium::Components::Pool::SpewRosterToTty()
{
	HELIUM_TRACE(
		TraceLevels::Debug,
		"Spewing roster for pool %x (%s) - %d components allocated\n",
		this,
		m_Type->m_Structure->m_Name,
		m_FirstUnallocatedIndex);

	for (int i = 0; i < m_FirstUnallocatedIndex; ++i)
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			"    - Index: %d  Component Addr: %x  Owner Addr: %x\n",
			GetComponentIndex(m_Roster[i]),
			m_Roster[i],
			m_Roster[i]->m_InlineData.m_Owner);
	}
}
#endif

Helium::ComponentManager::ComponentManager(World *pWorld)
	: m_World(pWorld)
{
	for (DynamicArray<TypeData *>::Iterator iter = g_ComponentTypes.Begin();
		iter != g_ComponentTypes.End(); ++iter)
	{
		const TypeData &type_data = **iter;

		m_Pools.New( Pool::CreatePool( this, type_data, type_data.m_DefaultCount ) );
	}
}

Helium::ComponentManager::~ComponentManager()
{
	Tick(); // Process pending deletes if necessary

	for (DynamicArray<Pool *>::Iterator iter = m_Pools.Begin();
		iter != m_Pools.End(); ++iter)
	{
		Pool *pPool = *iter;

		if ( pPool && pPool->GetAllocatedCount() > 0)
		{
			HELIUM_TRACE( TraceLevels::Warning, TXT( "Found %d components of type %s allocated during component system shutdown!\n" ),
				pPool->GetAllocatedCount(),
				g_ComponentTypes[ pPool->GetTypeId() ]->m_Structure->m_Name);
		}

		if ( pPool )
		{
			Pool::DestroyPool( pPool );
		}
	}

	m_Pools.Clear();
}

void Helium::Components::Tick()
{
	++g_ComponentProcessPendingDeletesCallCount;

	// Look at our registry of component ptrs, we may need to force some of them to invalidate (a ptr must be checked
	// at least once every 256 frames in case the generation counter overlaps)
	uint32_t registry_index = g_ComponentProcessPendingDeletesCallCount % COMPONENT_PTR_CHECK_FREQUENCY;
	ComponentPtrBase *component_ptr = g_ComponentPtrRegistry[registry_index];

	// Double-check that the component head index is set
	HELIUM_ASSERT(!component_ptr || component_ptr->m_ComponentPtrRegistryHeadIndex == registry_index);

	while (component_ptr)
	{
		// Don't do the check until we get our next pointer, as a failed check will splice out the ptr from our doubly
		// linked list.
		ComponentPtrBase *component_ptr_to_check = component_ptr;
		component_ptr = component_ptr->GetNextComponetPtr();
		component_ptr_to_check->Check();
	}
	
	// Even after we evict components, we still need to verify that we either have no component or the head component
	// has a properly set index
	HELIUM_ASSERT(
		!g_ComponentPtrRegistry[registry_index] || 
		g_ComponentPtrRegistry[registry_index]->m_ComponentPtrRegistryHeadIndex == registry_index);
}

void Helium::ComponentManager::RegisterComponentPtr( ComponentPtrBase &pPtr )
{
	uint16_t registry_index = g_ComponentProcessPendingDeletesCallCount % COMPONENT_PTR_CHECK_FREQUENCY;
	pPtr.m_Next = g_ComponentPtrRegistry[registry_index];
	
	if (pPtr.m_Next)
	{
		// Make the current head component's previous pointer point to the new component, and clear the head component index
		HELIUM_ASSERT(pPtr.m_Next->m_ComponentPtrRegistryHeadIndex == registry_index);
		pPtr.m_Next->m_Previous = &pPtr;
		pPtr.m_Next->m_ComponentPtrRegistryHeadIndex = Helium::Invalid<uint16_t>();
	}

	pPtr.m_ComponentPtrRegistryHeadIndex = registry_index;
	g_ComponentPtrRegistry[registry_index] = &pPtr;
}

size_t Helium::ComponentManager::CountAllocatedComponentsThatImplement( Components::TypeId typeId ) const
{
	TypeData *pTypeData = g_ComponentTypes[ typeId ];

	size_t count = 0;
	for (DynamicArray< TypeId >::Iterator iter = pTypeData->m_ImplementingTypes.Begin();
		iter != pTypeData->m_ImplementingTypes.End(); ++iter)
	{
		count += m_Pools[ *iter ]->GetAllocatedCount();
	}

	return count;
}

void Helium::ComponentPtrBase::Unlink() const
{
	// If we are the head node in the component ptr registry, we need to point it to the new head
	if (m_ComponentPtrRegistryHeadIndex != Helium::Invalid<uint16_t>())
	{
		ComponentPtrBase *current_head = g_ComponentPtrRegistry[m_ComponentPtrRegistryHeadIndex];
		HELIUM_ASSERT(current_head == this);
		HELIUM_ASSERT(!m_Previous);

		// If this new node is legit, mark it as a head node
		if (m_Next)
		{
			g_ComponentPtrRegistry[m_ComponentPtrRegistryHeadIndex] = m_Next;
			m_Next->m_ComponentPtrRegistryHeadIndex = m_ComponentPtrRegistryHeadIndex;
			m_Next->m_Previous = NULL;
		}
		else
		{
			g_ComponentPtrRegistry[m_ComponentPtrRegistryHeadIndex] = NULL;
		}

		// Unmark ourself as a head node
		m_ComponentPtrRegistryHeadIndex = Helium::Invalid<uint16_t>();
	} 
	// Unlink ourself from doubly linked list of component ptrs
	else if (m_Previous)
	{
		m_Previous->m_Next = m_Next;
	}

	if (m_Next)
	{
		m_Next->m_Previous = m_Previous;
	}

	m_Previous = 0;
	m_Next = 0;
}

#if HELIUM_TOOLS
void Helium::ComponentCollection::SpewToTty()
{
	HELIUM_TRACE(
		TraceLevels::Debug,
		"-- SPEWING COMPONENTS for component set %x--\n",
		this);

	for (Map< Components::TypeId, Component * >::Iterator iter = m_Components.Begin(); iter != m_Components.End(); ++iter)
	{
		TypeId typeId = iter->First();
		Component *pComponent = iter->Second();

		HELIUM_TRACE(
			TraceLevels::Debug,
			"  Component Type: %s\n",
			g_ComponentTypes[typeId]->m_Structure->m_Name);

		while (pComponent)
		{
			HELIUM_TRACE(
				TraceLevels::Debug,
				"    %x\n",
				pComponent);

			pComponent = pComponent->GetNextComponent();
		}
	}
}
#endif
