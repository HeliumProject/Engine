//----------------------------------------------------------------------------------------------------------------------
// World.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WORLD_H
#define HELIUM_FRAMEWORK_WORLD_H

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
		REFLECT_DECLARE_OBJECT( Helium::World, Reflect::Object);

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
		virtual void PreDestroy();
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
		virtual ComponentCollection &VirtualGetComponents();
		virtual World *VirtualGetWorld();
		
		SceneDefinitionPtr m_spSceneDefinition;

		ComponentCollection m_Components;

		/// Active slices.
		DynamicArray< SlicePtr > m_Slices;
		SlicePtr m_RootSlice;
	};

	typedef Helium::StrongPtr< World > WorldPtr;
	typedef Helium::StrongPtr< const World > ConstWorldPtr;
}

#include "Framework/Slice.h"
#include "Framework/World.inl"

#endif  // HELIUM_FRAMEWORK_WORLD_H
