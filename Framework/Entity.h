#pragma once

#include "Framework/Framework.h"

#include "Framework/Components.h"
#include "Framework/ComponentSet.h"
#include "Framework/Slice.h"

namespace Helium
{
	typedef Helium::WeakPtr< Slice > SliceWPtr;
	typedef Helium::WeakPtr< const Slice > ConstSliceWPtr;

	class World;
	typedef Helium::WeakPtr< World > WorldWPtr;
	typedef Helium::WeakPtr< const World > ConstWorldWPtr;

	class ComponentSet;
	class ParameterSet;

	class HELIUM_FRAMEWORK_API Entity : public Reflect::Object, public Components::IHasComponents
	{
	public:
		HELIUM_DECLARE_CLASS(Helium::Entity, Helium::Reflect::Object);
		static void PopulateMetaType( Reflect::MetaStruct& comp );
		
		Entity()
			: m_DeferredDestroy(false)
			, m_sliceIndex(Invalid<size_t>()) { }
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
							
		inline void DeployComponents(const ComponentSet &_components, const ParameterSet *_parameters);
		inline void DeployComponents(const DynamicArray<ComponentDefinitionPtr> &_components);
		//@}
		
		/// @name SceneDefinition Registration
		//@{
		inline const SliceWPtr& GetSlice() const;
		inline size_t GetSliceIndex() const;
		void SetSliceInfo( Slice* pSlice, size_t sliceIndex );
		void SetSliceIndex( size_t sliceIndex );
		void ClearSliceInfo();
		//@}

		void DeferredDestroy() { m_DeferredDestroy = true; }
		bool IsDeferredDestroySet() { return m_DeferredDestroy; }
		
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

		bool m_DeferredDestroy;
		
	};
	typedef Helium::StrongPtr<Entity> EntityPtr;
	typedef Helium::WeakPtr<Entity> EntityWPtr;
}

#include "Framework/Entity.inl"
