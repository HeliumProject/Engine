#pragma once

#include "Framework/ComponentQuery.h"
#include "Framework/Framework.h"
#include "Framework/SceneDefinition.h"

namespace Helium
{
	class Entity;
	class EntityDefinition;
	
	class Slice;
	typedef Helium::StrongPtr< Slice > SlicePtr;

	/// World instance.
	///
	/// A world contains a discrete group of entities that can be simulated within an application environment.  Multiple
	/// world instances can exist at the same time, allowing the use of specific worlds for special-case scenarios, such
	/// as rendering scenes outside the game world to a texture or editor preview windows.
	class HELIUM_FRAMEWORK_API World : public Reflect::Object, public Components::IHasComponents
	{
		HELIUM_DECLARE_CLASS( Helium::World, Reflect::Object);

	public:
		/// @name Construction/Destruction
		//@{
		World();
		virtual ~World();
		//@}

		/// @name World Initialization
		//@{
		virtual bool Initialize();
		virtual void Shutdown();
		//@}
		
		/// @name Component API
		//@{
		inline ComponentCollection &GetComponents();

		inline ComponentManager *GetComponentManager();
		//@}

		/// @name Asset Interface
		//@{
		virtual void RefCountPreDestroy();
		//@}

		/// @name EntityDefinition Creation
		//@{
		//virtual EntityDefinition* CreateEntity(
		//    SceneDefinition* pSlice, Entity* pEntity);
		//virtual bool DestroyEntity( Entity* pEntity );
		//virtual Entity *CreateEntity(EntityDefinition *pEntityDefinition, Slice *pSlice = 0);
		//virtual Entity *DestroyEntity(Entity *pEntity);
		Slice *GetRootSlice() { return m_RootSlice; }
		//@}

		/// @name SceneDefinition Registration
		//@{
		virtual bool AddSlice( Slice* pSlice );
		virtual bool RemoveSlice( Slice* pSlice );

		inline size_t GetSliceCount() const;
		Slice* GetSlice( size_t index ) const;
		//@}

		/// @name Scene Access
		//@{
		SceneDefinition* GetSceneDefinition() { return m_spSceneDefinition.Get(); }
		//@}

	public:
		// TEMPORARY!
		ComponentManagerPtr m_ComponentManager;
	private:
		// Avoid using this vfunc if you can! Use GetComponents()
		virtual ComponentCollection& VirtualGetComponents();
		virtual ComponentManager* VirtualGetComponentManager();
		
		SceneDefinitionPtr m_spSceneDefinition;

		ComponentCollection m_Components;

		/// Active slices.
		DynamicArray< SlicePtr > m_Slices;
		SlicePtr m_RootSlice;
	};

	typedef Helium::StrongPtr< World > WorldPtr;
	typedef Helium::StrongPtr< const World > ConstWorldPtr;

	template <class A, void (*F)(A *)>
	inline void QueryComponents( World *pWorld )
	{ 
		ComponentManager *pComponentManager = pWorld->GetComponentManager();
		HELIUM_ASSERT( pComponentManager );
		for (ImplementingComponentIterator<A> iter( *pComponentManager ); iter.GetBaseComponent(); iter.Advance())
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

		ComponentManager *pComponentManager = pWorld->GetComponentManager();
		HELIUM_ASSERT( pComponentManager );
		QueryComponentsInternal( *pComponentManager, types, HELIUM_ARRAY_COUNT(types), TupleHandler<A, B, F> );
	}
	
	template <class A, class B, class C, void (*F)(A *, B *, C *)>
	inline void QueryComponents( World *pWorld )
	{
		static Components::TypeId types[] = {
			Components::GetType<A>(),
			Components::GetType<B>(),
			Components::GetType<C>()
		};

		ComponentManager *pComponentManager = pWorld->GetComponentManager();
		HELIUM_ASSERT( pComponentManager );
		QueryComponentsInternal( *pComponentManager, types, HELIUM_ARRAY_COUNT(types), TupleHandler<A, B, C, F> );
	}
}

#include "Framework/Slice.h"
#include "Framework/World.inl"
