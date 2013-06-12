//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_ENTITY_H
#define HELIUM_FRAMEWORK_ENTITY_H

#include "Framework/Framework.h"

#include "Framework/Components.h"
#include "Framework/ComponentDefinitionSet.h"

namespace Helium
{
	class Slice;
	typedef Helium::WeakPtr< Slice > SliceWPtr;
	typedef Helium::WeakPtr< const Slice > ConstSliceWPtr;
	
	class World;
	typedef Helium::WeakPtr< World > WorldWPtr;
	typedef Helium::WeakPtr< const World > ConstWorldWPtr;

	class ComponentDefinitionSet;
	class ParameterSet;

	class HELIUM_FRAMEWORK_API Entity : public Reflect::Object, public Components::IHasComponents
	{
	public:
		REFLECT_DECLARE_OBJECT(Helium::Entity, Helium::Reflect::Object);
		static void PopulateStructure( Reflect::Structure& comp );
		
		~Entity();
		
		// TODO: Wish I could inline this but cyclical #includes..
		World *GetWorld();
		inline ComponentCollection &GetComponents();		

		/// @name General Info
		//@{
		const AssetPath &GetDefinitionPath() { return m_DefinitionPath; }
		//@}

		/// @name Component Management
		//@{
		template <class T>  inline T*  Allocate();
		template <class T>  inline T*  GetFirst();
							
		inline void DeployComponents(const ComponentDefinitionSet &_components, const ParameterSet &_parameters);
		//@}
		
		/// @name SceneDefinition Registration
		//@{
		inline const SliceWPtr& GetSlice() const;
		inline size_t GetSliceIndex() const;
		void SetSliceInfo( Slice* pSlice, size_t sliceIndex );
		void SetSliceIndex( size_t sliceIndex );
		void ClearSliceInfo();
		//@}
		
	private:
		// Avoid using these vfuncs if you can! Use GetComponents() and GetWorld
		virtual ComponentManager* VirtualGetComponentManager();
		virtual ComponentCollection& VirtualGetComponents();

		ComponentCollection m_Components;
		
		/// EntityDefinition slice.
		SliceWPtr m_spSlice;
		/// Runtime index for the entity within its slice.
		size_t m_sliceIndex;
		/// Path to creating definition. Not storing definition because we don't want to
		/// keep it allocated if we don't need to.
		AssetPath m_DefinitionPath;
		
	};
	typedef Helium::StrongPtr<Entity> EntityPtr;
	typedef Helium::WeakPtr<Entity> EntityWPtr;
}

#include "Framework/Entity.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H
