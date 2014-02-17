
#pragma once

#include "Components/Components.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/GraphicsScene.h"
#include "Components/TransformComponent.h"


namespace Helium
{
	struct MeshComponentDefinition;

	class MeshSceneObjectTransform;
	class GraphicsManagerComponent;

	class HELIUM_COMPONENTS_API MeshComponent : public Component
	{
	public:
		HELIUM_DECLARE_COMPONENT( Helium::MeshComponent, Helium::Component );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		MeshComponent();
		virtual ~MeshComponent();

		void Initialize( const Helium::MeshComponentDefinition& definition );
		void Finalize( const Helium::MeshComponentDefinition& definition );

		/// @name Entity Registration
		//@{
		void Attach(class GraphicsScene *pGraphicsScene, class TransformComponent *pTransformComponent);
		void Detach(class GraphicsScene *pGraphicsScene);
		//@}

		/// @name Mesh Rendering Data
		//@{
		void SetMesh( Mesh* pMesh );
		inline Mesh* GetMesh() const;

		inline size_t GetOverrideMaterialCount() const;
		inline Material* GetOverrideMaterial( size_t index ) const;

		inline Material* GetMaterial( size_t index ) const;
		//@}

		void Update( class GraphicsScene *pGraphicsScene, class TransformComponent *pTransform );
		
		/// @name Scene GameObject Synchronization Callback
		//@{
		static void GraphicsSceneObjectUpdate( 
			MeshComponent* pMesh, 
			GraphicsScene* pScene, 
			TransformComponent *pTransform, 
			GraphicsSceneObject::EUpdate,
			size_t graphicsSceneObjectId);
		//@}

	private:
		/// The mesh
		StrongPtr< Mesh > m_Mesh;
		/// Override material set.
		DynamicArray< MaterialPtr > m_OverrideMaterials;

		/// ID of the scene object representing this entity in the graphics scene.
		size_t m_graphicsSceneObjectId;
		/// IDs of scene object sub-mesh data for each sub-mesh of this entity's mesh.
		DynamicArray< size_t > m_graphicsSceneObjectSubMeshDataIds;

		ComponentPtr<MeshSceneObjectTransform> m_MeshSceneObjectTransformComponent;

		bool m_NeedsReattach;

		/// @name Graphics Scene GameObject Updating
		//@{
		void SetNeedsGraphicsSceneObjectUpdate(
			TransformComponent *pTransform,
			GraphicsSceneObject::EUpdate updateMode = GraphicsSceneObject::UPDATE_FULL );
		//@}

		void DeferredReattach() { m_NeedsReattach = true; }
	};
	typedef Helium::ComponentPtr<MeshComponent> MeshComponentPtr;
	
	struct HELIUM_COMPONENTS_API MeshComponentDefinition : public Helium::ComponentDefinitionHelperWithFinalize<MeshComponent, MeshComponentDefinition>
	{
	public:
		HELIUM_DECLARE_CLASS( Helium::MeshComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Reflect::MetaStruct& comp );
				
		StrongPtr<Mesh> m_Mesh;
		DynamicArray< MaterialPtr > m_OverrideMaterials;
	};
	typedef StrongPtr<MeshComponentDefinition> MeshComponentDefinitionPtr;
	
	class HELIUM_COMPONENTS_API MeshSceneObjectTransform : public SceneObjectTransform
	{
	public:
		MeshSceneObjectTransform();
		MeshSceneObjectTransform(const MeshSceneObjectTransform &rRhs);

		HELIUM_DECLARE_COMPONENT( Helium::MeshSceneObjectTransform, SceneObjectTransform );

		void Setup(TransformComponent *pTransform, MeshComponent *pMesh, GraphicsSceneObject::EUpdate updateMode, size_t graphicsSceneObjectId);
		void Update(GraphicsSceneObject::EUpdate updateMode);

		virtual void GraphicsSceneObjectUpdate(GraphicsScene *pScene);

		const MeshComponent *GetMeshComponent() const { return m_MeshComponent.Get(); }

		TransformComponentPtr m_TransformComponent;
		MeshComponentPtr m_MeshComponent;
		GraphicsSceneObject::EUpdate m_UpdateMode;
		size_t m_graphicsSceneObjectId;
	};
}

#include "MeshComponent.inl"
