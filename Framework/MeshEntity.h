//----------------------------------------------------------------------------------------------------------------------
// MeshEntity.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_MESH_ENTITY_H
#define HELIUM_FRAMEWORK_MESH_ENTITY_H

#include "Framework/Entity.h"

#include "GraphicsTypes/GraphicsSceneObject.h"
#include "Framework/Mesh.h"

namespace Helium
{
    class GraphicsScene;
    class GraphicsSceneObject;

    HELIUM_DECLARE_PTR( Mesh );
    HELIUM_DECLARE_PTR( Material );

    /// Base class for mesh-based entities.
    class HELIUM_FRAMEWORK_API MeshEntity : public Entity
    {
        HELIUM_DECLARE_OBJECT( MeshEntity, Entity );

    public:
        /// @name Construction/Destruction
        //@{
        MeshEntity();
        virtual ~MeshEntity();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Entity Registration
        //@{
        virtual void Attach();
        virtual void Detach();
        //@}

        /// @name Transform Data
        //@{
        virtual void SetPosition( const Simd::Vector3& rPosition );
        virtual void SetRotation( const Simd::Quat& rRotation );
        virtual void SetScale( const Simd::Vector3& rScale );
        //@}

        /// @name Mesh Rendering Data
        //@{
        void SetMesh( Mesh* pMesh );
        inline Mesh* GetMesh() const;

        inline size_t GetOverrideMaterialCount() const;
        inline Material* GetOverrideMaterial( size_t index ) const;

        inline Material* GetMaterial( size_t index ) const;
        //@}

    protected:
        /// Mesh resource.
        MeshPtr m_spMesh;
        /// Override material set.
        DynArray< MaterialPtr > m_overrideMaterials;

        /// ID of the scene object representing this entity in the graphics scene.
        size_t m_graphicsSceneObjectId;
        /// IDs of scene object sub-mesh data for each sub-mesh of this entity's mesh.
        DynArray< size_t > m_graphicsSceneObjectSubMeshDataIds;

        /// @name Graphics Scene GameObject Updating
        //@{
        void SetNeedsGraphicsSceneObjectUpdate(
            GraphicsSceneObject::EUpdate updateMode = GraphicsSceneObject::UPDATE_FULL ) const;

        virtual GraphicsSceneObject::UPDATE_FUNC* GetGraphicsSceneObjectUpdateCallback() const;
        //@}

        /// @name Scene GameObject Synchronization Callback
        //@{
        static void GraphicsSceneObjectUpdate( void* pData, GraphicsScene* pScene, GraphicsSceneObject* pSceneObject );
        //@}
    };
}

#include "Framework/MeshEntity.inl"

#endif  // HELIUM_FRAMEWORK_MESH_ENTITY_H
