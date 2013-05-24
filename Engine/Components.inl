

namespace Helium
{
	namespace Components
	{
		
		TypeData::TypeData() 
			: m_TypeId(Invalid<TypeId>())
		{

		}

		ComponentSizeType TypeData::GetSize() const
		{
			return m_Structure->m_Size;
		}

		template <class T>
		void TypeDataT<T>::Destruct( Component *ptr ) const
		{
			(static_cast<T *>(ptr))->~T();
		}

		template <class T>
		void TypeDataT<T>::Construct( Component *ptr ) const
		{
			T* t = static_cast<T *>( ptr );
			new (t) T;
		}

		template< class ClassT, class BaseT >
		ComponentRegistrar<ClassT, BaseT>::ComponentRegistrar( const tchar_t* name, uint16_t _count ) 
			: StructureRegistrar(name)
			, m_Count(_count)
		{

		}

		template< class ClassT, class BaseT >
		void ComponentRegistrar<ClassT, BaseT>::Register()
		{
			if (ClassT::GetStaticComponentTypeData().m_TypeId == Invalid<TypeId>())
			{
				BaseT::s_ComponentRegistrar.Register();
				StructureRegistrar::Register();
				TypeId type_id = RegisterType(
					Reflect::GetStructure< ClassT >(), 
					ClassT::GetStaticComponentTypeData(), 
					&BaseT::GetStaticComponentTypeData(), 
					m_Count);
			}
		}

		template< class ClassT >
		ComponentRegistrar<ClassT, void >::ComponentRegistrar( const tchar_t* name ) 
			: StructureRegistrar(name)
		{

		}

		template < class ClassT >
		void ComponentRegistrar<ClassT, void>::Register()
		{
			if (ClassT::GetStaticComponentTypeData().m_TypeId == Invalid<TypeId>())
			{
				StructureRegistrar::Register();
				RegisterType( 
					Reflect::GetStructure< ClassT >(), 
					ClassT::GetStaticComponentTypeData(), 
					0, 
					0);
			}
		}

		Pool* Pool::GetPool( const Component *component )
		{
			HELIUM_ASSERT( component->m_InlineData.m_OffsetToPoolStart );
			return reinterpret_cast<Pool *>( 
				( reinterpret_cast<uintptr_t>(component) & POOL_ALIGN_SIZE_MASK ) - 
				( static_cast<uintptr_t>( component->m_InlineData.m_OffsetToPoolStart ) * HELIUM_COMPONENT_POOL_ALIGN_SIZE ) );
		}
		
		TypeId Pool::GetTypeId() const
		{
			return m_TypeId;
		}
		
		ComponentManager* Helium::Components::Pool::GetComponentManager() const
		{
			return m_ComponentManager;
		}
		
		World* Pool::GetWorld() const
		{
			return m_World;
		}

		Component* Pool::GetComponent( ComponentIndex index ) const
		{
			if ( IsValid<ComponentIndex>( index ) )
			{
				return reinterpret_cast<Component *>( GetFirstComponentPtr() + index * m_ComponentSize );
			}

			return NULL;
		}

		ComponentIndex Pool::GetComponentIndex( const Component *component ) const
		{
			return static_cast<ComponentIndex>( ( reinterpret_cast<uintptr_t>( component ) - GetFirstComponentPtr() ) / static_cast<uintptr_t>(m_ComponentSize) );
		}
		
		ComponentCollection* Pool::GetComponentCollection( const Component *component ) const
		{
			return m_ParallelData[ GetComponentIndex( component ) ].m_Collection;
		}

		void* Pool::GetComponentOwner( const Component *component ) const
		{
			return component->m_InlineData.m_Owner;
		}

		Component* Pool::GetNext( Component *component ) const
		{
			return GetComponent( component->m_InlineData.m_Next );
		}
		
		Component* Pool::GetNext( ComponentIndex index ) const
		{
			return GetComponent( GetComponent( index )->m_InlineData.m_Next );
		}
		
		ComponentIndex Pool::GetNextIndex( Component *component ) const
		{
			return component->m_InlineData.m_Next;
		}
		
		ComponentIndex Pool::GetNextIndex( ComponentIndex index ) const
		{
			return GetComponent( index )->m_InlineData.m_Next;
		}

		Component* Pool::GetPrevious( Component *component ) const
		{
			//return GetComponent( m_ParallelData[ GetComponentIndex( component ) ].m_Previous );
			return GetComponent( component->m_InlineData.m_Previous );
		}
		
		Component* Pool::GetPrevious( ComponentIndex index ) const
		{
			//return GetComponent( m_ParallelData[ index ].m_Previous );
			return GetComponent( GetComponent( index )->m_InlineData.m_Previous );
		}
		
		ComponentIndex Pool::GetPreviousIndex( Component *component ) const
		{
			//return m_ParallelData[ GetComponentIndex( component ) ].m_Previous;
			return component->m_InlineData.m_Previous;
		}
		
		ComponentIndex Pool::GetPreviousIndex( ComponentIndex index ) const
		{
			//return m_ParallelData[ index ].m_Previous;
			return GetComponent( index )->m_InlineData.m_Previous;
		}

		GenerationIndex Pool::GetGeneration( ComponentIndex index ) const
		{
			return GetComponent( index )->m_InlineData.m_Generation;
		}
		
		ComponentIndex Pool::GetAllocatedCount() const
		{
			return m_FirstUnallocatedIndex;
		}
		
		Component * const * Pool::GetAllocatedComponents() const
		{
			return m_Roster.GetData();
		}
				
		uintptr_t Pool::GetFirstComponentPtr() const
		{
			static const uintptr_t POOL_SIZE = (  (sizeof(Pool) + (HELIUM_SIMD_ALIGNMENT-1))  &  (~(HELIUM_SIMD_ALIGNMENT-1))  );
			return reinterpret_cast<uintptr_t>(this) + POOL_SIZE + m_ComponentOffset;
		}
				
		template <class T>
		TypeId GetType()
		{
			TypeData &data = T::GetStaticComponentTypeData();
			HELIUM_ASSERT(data.m_TypeId != Invalid<TypeId>());

			return data.m_TypeId;
		}

	}

	ComponentIteratorBase::ComponentIteratorBase( ComponentManager &rManager ) 
		: m_Index( 0 )
		, m_pPool( NULL )
		, m_Types( NULL )
		, m_Manager( rManager )
	{
	}
	
	ComponentIteratorBase::ComponentIteratorBase( ComponentManager &rManager, const DynamicArray< Components::TypeId > &types ) 
		: m_Index( 0 )
		, m_pPool( NULL )
		, m_Types( &types )
		, m_Manager( rManager )
	{
		ResetToBeginning();
	}
	
	Component *ComponentIteratorBase::GetBaseComponent()
	{
		return m_pComponent;
	}

	void ComponentIteratorBase::ResetToBeginning()
	{
		HELIUM_ASSERT( !m_Types->IsEmpty() );

		m_TypesIterator = m_Types->Begin();
		m_pPool = m_Manager.GetPool( *m_TypesIterator );
		m_Index = 0;
		
		if ( m_pPool && m_pPool->GetAllocatedCount() > 0 )
		{
			// If there's a pool, get first component
			m_pComponent = m_pPool->GetComponent( 0 );
		}
		else
		{
			SkipToNextType();
		}
	}
	
	void ComponentIteratorBase::Advance()
	{
		HELIUM_ASSERT( m_pComponent );
		HELIUM_ASSERT( m_pPool );
		HELIUM_ASSERT( m_TypesIterator != m_Types->End() );

		++m_Index;

		if ( m_Index < m_pPool->GetAllocatedCount() )
		{
			m_pComponent = m_pPool->GetAllocatedComponents()[ m_Index ];
		}
		else
		{		
			SkipToNextType();
		}
	}

	void ComponentIteratorBase::SkipToNextType()
	{
		// Get next pool
		m_Index = 0;
		m_pPool = NULL;
		while ( !m_pPool || !m_pPool->GetAllocatedCount() )
		{
			if ( ++m_TypesIterator == m_Types->End() )
			{
				break;
			}
			
			m_pPool = m_Manager.GetPool( *m_TypesIterator );
		}
			
		if ( m_pPool )
		{
			// If there's a pool, get first component
			HELIUM_ASSERT( m_pPool->GetAllocatedCount() > 0 );
			m_pComponent = m_pPool->GetComponent( 0 );
		}
		else
		{
			// No pools means iterator is done
			m_pComponent = NULL;
		}
	}
	
	template <class T>
	ComponentIteratorBaseT<T>::ComponentIteratorBaseT( ComponentManager &rManager ) 
		: ComponentIteratorBase( rManager )
	{
	}
	
	template <class T>
	T * ComponentIteratorBaseT<T>::operator*()
	{
		return static_cast<T*>( GetBaseComponent() );
	}
	
	template <class T>
	T * ComponentIteratorBaseT<T>::operator->()
	{
		return static_cast<T*>( GetBaseComponent() );
	}
	
	Component* ComponentManager::Allocate( Components::TypeId type, void *pOwner, ComponentCollection &rCollection )
	{
		return m_Pools[ type ]->Allocate( pOwner, rCollection );
	}

	size_t ComponentManager::CountAllocatedComponents( Components::TypeId typeId ) const
	{
		return m_Pools[ typeId ]->GetAllocatedCount();
	}
	
	World * ComponentManager::GetWorld() const
	{
		return m_World;
	}
	
	Helium::ComponentCollection::ComponentCollection()
	{

	}

	Helium::ComponentCollection::~ComponentCollection()
	{
		ReleaseAll();
	}

	Component * Helium::ComponentCollection::GetFirst( Components::TypeId type )
	{
		Map< Components::TypeId, Component * >::Iterator iter = m_Components.Find( type );
		if ( iter != m_Components.End() )
		{
			return iter->Second();
		}

		return NULL;
	}

	void ComponentCollection::GetAll( Components::TypeId type, DynamicArray<Component *> &components )
	{
		Component *c = GetFirst( type );

		while ( c )
		{
			components.New( c );
			c = c->GetNextComponent();
		}
	}

	void ComponentCollection::GetAllThatImplement( Components::TypeId type, DynamicArray<Component *> &components )
	{
		const DynamicArray< Components::TypeId > &implementing_types = Components::GetTypeData( type )->m_ImplementingTypes;

		for (DynamicArray< Components::TypeId >::ConstIterator iter = implementing_types.Begin();
			iter != implementing_types.End(); ++iter)
		{
			GetAll( *iter, components );
		}
	}
	
	void ComponentCollection::ReleaseAll( )
	{
		for (size_t i = m_Components.GetSize(); i != 0; --i)
		{
			HELIUM_ASSERT( i == m_Components.GetSize() );

			Component *c = m_Components.Begin()->Second();
			HELIUM_ASSERT( c );
			Components::Pool *pool = Components::Pool::GetPool( c );
			
			while ( c )
			{
				Component *next = c->GetNextComponent();
				pool->Free( c );
				c = next;
			}
		}

		HELIUM_ASSERT( m_Components.IsEmpty() );
	}

	ComponentManager * Component::GetComponentManager() const
	{
		return Components::Pool::GetPool( this )->GetComponentManager();
	}
	
	ComponentCollection * Component::GetComponentCollection() const
	{
		return Components::Pool::GetPool( this )->GetComponentCollection( this );
	}

	void* Component::GetOwner() const
	{
		return m_InlineData.m_Owner;
	}

	World* Component::GetWorld() const
	{
		return Components::Pool::GetPool( this )->GetWorld();
	}

	void Component::FreeComponent()
	{
		Components::Pool *pool = Components::Pool::GetPool( this );
		pool->Free( this );
	}

	void Component::FreeComponentDeferred()
	{
		m_InlineData.m_Delete = true;
	}
	
	const Components::DataInline &Component::GetInlineData() const
	{
		return m_InlineData;
	}

	void ComponentPtrBase::Check() const
	{
		// If no component, we're done
		if (!m_Component)
		{
			return;
		}

		// If generation doesn't match
		if (m_Component->m_InlineData.m_Generation != m_Generation)
		{
			// Drop the component
			Set(NULL);
		}
	}

	bool ComponentPtrBase::IsGood() const
	{
		Check();
		return (m_Component != NULL);
	}

	void ComponentPtrBase::Set( Component *_component )
	{
		const ComponentPtrBase *ptr = this;
		ptr->Set(_component);
	}

	void ComponentPtrBase::Set( Component *_component ) const
	{
		if (m_Component == _component)
		{
			// Is also desired case for assigning null to null ptr
			return;
		}

		Unlink();
		HELIUM_ASSERT(m_ComponentPtrRegistryHeadIndex == Helium::Invalid<uint16_t>());
		m_Component = _component;

		if (m_Component)
		{
			m_Generation = m_Component->m_InlineData.m_Generation;
			m_Component->GetComponentManager()->RegisterComponentPtr( *const_cast<ComponentPtrBase *>(this) );
			HELIUM_ASSERT( m_ComponentPtrRegistryHeadIndex != Helium::Invalid<uint16_t>() );
			HELIUM_ASSERT( !m_Next || m_Next->m_ComponentPtrRegistryHeadIndex == Helium::Invalid<uint16_t>() );
		}
	}

	ComponentPtrBase::ComponentPtrBase() 
		: m_Next(0)
		, m_Previous(0)
		, m_Component(0)
		, m_ComponentPtrRegistryHeadIndex(Helium::Invalid<uint16_t>())
	{

	}

	ComponentPtrBase::~ComponentPtrBase()
	{
		Unlink();
	}
		
	template <class T>
	ComponentPtr<T>::ComponentPtr()
	{
		Set(0);
	}

	template <class T>
	ComponentPtr<T>::ComponentPtr( T *_component )
	{
		Set(_component);
	}

	template <class T>
	ComponentPtr<T>::ComponentPtr( const ComponentPtr& _rhs )
	{
		Set(_rhs.m_Component);
	}

	template <class T>
	void ComponentPtr<T>::operator=( T *_component )
	{
		Set(_component);
	}

	template <class T>
	T * ComponentPtr<T>::UncheckedGet() const
	{
		return static_cast<T*>(m_Component);
	}

	template <class T>
	T * ComponentPtr<T>::Get()
	{
		Check();
		return UncheckedGet();
	}

	template <class T>
	const T * ComponentPtr<T>::Get() const
	{
		Check();
		return UncheckedGet();
	}

	template <class T>
	T & ComponentPtr<T>::operator*()
	{
		return *Get();
	}

	template <class T>
	T * ComponentPtr<T>::operator->()
	{
		return Get();
	}
}
