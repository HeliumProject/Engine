//----------------------------------------------------------------------------------------------------------------------
// MeshEntity.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/MeshEntity.h"

#include "Rendering/RVertexDescription.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/RenderResourceManager.h"
#include "Framework/World.h"

namespace Lunar
{
    L_IMPLEMENT_OBJECT( MeshEntity, Framework, 0 );

    /// Constructor.
    MeshEntity::MeshEntity()
        : m_graphicsSceneObjectId( Invalid< size_t >() )
    {
    }

    /// Destructor.
    MeshEntity::~MeshEntity()
    {
        HELIUM_ASSERT( IsInvalid( m_graphicsSceneObjectId ) );
    }

    /// @copydoc Object::Serialize()
    void MeshEntity::Serialize( Serializer& s )
    {
        L_SERIALIZE_SUPER( s );

        s << L_TAGGED( m_spMesh );
        s << L_TAGGED_DYNARRAY( m_overrideMaterials );
    }

    /// @copydoc Entity::Attach()
    void MeshEntity::Attach()
    {
        Super::Attach();

        HELIUM_ASSERT( IsInvalid( m_graphicsSceneObjectId ) );

        Mesh* pMesh = m_spMesh;
        if( pMesh && pMesh->GetVertexBuffer() && pMesh->GetIndexBuffer() )
        {
            size_t meshSectionCount = pMesh->GetSectionCount();
            if( meshSectionCount != 0 )
            {
                World* pWorld = GetWorld();
                HELIUM_ASSERT( pWorld );
                GraphicsScene* pGraphicsScene = pWorld->GetGraphicsScene();
                HELIUM_ASSERT( pGraphicsScene );

                m_graphicsSceneObjectId = pGraphicsScene->AllocateSceneObject();
                HELIUM_ASSERT( IsValid( m_graphicsSceneObjectId ) );

                m_graphicsSceneObjectSubMeshDataIds.Reserve( meshSectionCount );
                m_graphicsSceneObjectSubMeshDataIds.Resize( meshSectionCount );

                for( size_t meshSectionIndex = 0; meshSectionIndex < meshSectionCount; ++meshSectionIndex )
                {
                    size_t subMeshId = pGraphicsScene->AllocateSceneObjectSubMeshData( m_graphicsSceneObjectId );
                    HELIUM_ASSERT( IsValid( subMeshId ) );
                    m_graphicsSceneObjectSubMeshDataIds[ meshSectionIndex ] = subMeshId;
                }

                GraphicsSceneObject* pSceneObject = pGraphicsScene->GetSceneObject( m_graphicsSceneObjectId );
                HELIUM_ASSERT( pSceneObject );
                pSceneObject->SetUpdateCallback( GetGraphicsSceneObjectUpdateCallback(), this );
                pSceneObject->SetNeedsUpdate();
            }
        }
    }

    /// @copydoc Entity::Detach()
    void MeshEntity::Detach()
    {
        World* pWorld = GetWorld();
        HELIUM_ASSERT( pWorld );
        GraphicsScene* pGraphicsScene = pWorld->GetGraphicsScene();
        HELIUM_ASSERT( pGraphicsScene );

        size_t subMeshIdCount = m_graphicsSceneObjectSubMeshDataIds.GetSize();
        for( size_t subMeshIndex = 0; subMeshIndex < subMeshIdCount; ++subMeshIndex )
        {
            size_t subMeshId = m_graphicsSceneObjectSubMeshDataIds[ subMeshIndex ];
            if( IsValid( subMeshId ) )
            {
                pGraphicsScene->ReleaseSceneObjectSubMeshData( subMeshId );
            }
        }

        m_graphicsSceneObjectSubMeshDataIds.Resize( 0 );

        if( IsValid( m_graphicsSceneObjectId ) )
        {
            pGraphicsScene->ReleaseSceneObject( m_graphicsSceneObjectId );
            SetInvalid( m_graphicsSceneObjectId );
        }

        Super::Detach();
    }

    /// @copydoc Entity::SetPosition()
    void MeshEntity::SetPosition( const Simd::Vector3& rPosition )
    {
        Super::SetPosition( rPosition );
        SetNeedsGraphicsSceneObjectUpdate( GraphicsSceneObject::UPDATE_TRANSFORM_ONLY );
    }

    /// @copydoc Entity::SetRotation()
    void MeshEntity::SetRotation( const Simd::Quat& rRotation )
    {
        Super::SetRotation( rRotation );
        SetNeedsGraphicsSceneObjectUpdate( GraphicsSceneObject::UPDATE_TRANSFORM_ONLY );
    }

    /// @copydoc Entity::SetScale()
    void MeshEntity::SetScale( const Simd::Vector3& rScale )
    {
        Super::SetScale( rScale );
        SetNeedsGraphicsSceneObjectUpdate( GraphicsSceneObject::UPDATE_TRANSFORM_ONLY );
    }

    /// Set the mesh used by this entity.
    ///
    /// @param[in] pMesh  Mesh to assign.
    ///
    /// @see GetMesh()
    void MeshEntity::SetMesh( Mesh* pMesh )
    {
        VerifySafety();

        if( m_spMesh.Get() != pMesh )
        {
            m_spMesh = pMesh;
            DeferredReattach();
        }
    }

    /// Flag the graphics scene object as requiring an update if one exists.
    ///
    /// This is safe to call by an entity during its pre-update.  It should only ever be called by the entity itself.
    ///
    /// @param[in] updateMode  Scene object update mode.
    void MeshEntity::SetNeedsGraphicsSceneObjectUpdate( GraphicsSceneObject::EUpdate updateMode ) const
    {
        VerifySafetySelfOnly();

        if( IsValid( m_graphicsSceneObjectId ) )
        {
            World* pWorld = GetWorld();
            HELIUM_ASSERT( pWorld );
            GraphicsScene* pGraphicsScene = pWorld->GetGraphicsScene();
            HELIUM_ASSERT( pGraphicsScene );
            GraphicsSceneObject* pSceneObject = pGraphicsScene->GetSceneObject( m_graphicsSceneObjectId );
            HELIUM_ASSERT( pSceneObject );
            pSceneObject->SetNeedsUpdate( updateMode );
        }
    }

    /// Get the callback to use to update the graphics scene object for this entity.
    ///
    /// @return  Graphics scene object update callback for this entity.
    GraphicsSceneObject::UPDATE_FUNC* MeshEntity::GetGraphicsSceneObjectUpdateCallback() const
    {
        return GraphicsSceneObjectUpdate;
    }

    /// Callback used to update graphics scene object information prior to the graphics scene update.
    ///
    /// @param[in] pData         Callback data (pointer to the MeshEntity).
    /// @param[in] pScene        Graphics scene to which the object is attached.
    /// @param[in] pSceneObject  Graphics scene object to update.
    void MeshEntity::GraphicsSceneObjectUpdate(
        void* pData,
        GraphicsScene* pScene,
        GraphicsSceneObject* pSceneObject )
    {
        HELIUM_ASSERT( pData );
        HELIUM_ASSERT( pScene );
        HELIUM_ASSERT( pSceneObject );

        MeshEntity* pThis = static_cast< MeshEntity* >( pData );

        const Simd::Vector3& rPosition = pThis->GetPosition();
        Simd::Matrix44 transform(
            Simd::Matrix44::INIT_ROTATION_TRANSLATION_SCALING,
            pThis->GetRotation(),
            rPosition,
            pThis->GetScale() );
        pSceneObject->SetTransform( transform );

        Mesh* pMesh = pThis->m_spMesh;

        Simd::AaBox worldBounds( rPosition, rPosition );

        // Only thing remaining if this is a transform-only update is the world bounds, so update it and return.
        if( pSceneObject->GetUpdateMode() == GraphicsSceneObject::UPDATE_TRANSFORM_ONLY )
        {
            if( pMesh )
            {
                worldBounds = pMesh->GetBounds();
                worldBounds.TransformBy( transform );
            }

            pSceneObject->SetWorldBounds( worldBounds );

            return;
        }

        RVertexBuffer* pVertexBuffer = NULL;
        RIndexBuffer* pIndexBuffer = NULL;
        if( pMesh )
        {
            pVertexBuffer = pMesh->GetVertexBuffer();
            pIndexBuffer = pMesh->GetIndexBuffer();

            worldBounds = pMesh->GetBounds();
            worldBounds.TransformBy( transform );
        }

        pSceneObject->SetWorldBounds( worldBounds );

        const DynArray< size_t >& rSubMeshDataIds = pThis->m_graphicsSceneObjectSubMeshDataIds;
        size_t subMeshCount = rSubMeshDataIds.GetSize();
        size_t meshSectionCount = 0;

        if( !pVertexBuffer || !pIndexBuffer )
        {
            pSceneObject->SetVertexData( NULL, NULL, 0 );
            pSceneObject->SetIndexBuffer( NULL );
        }
        else
        {
            RenderResourceManager& rResourceManager = RenderResourceManager::GetStaticInstance();

            RVertexDescription* pVertexDescription;
            uint32_t vertexStride;
            if( pMesh->IsSkinned() )
            {
                pVertexDescription = rResourceManager.GetSkinnedMeshVertexDescription();
                vertexStride = static_cast< uint32_t >( sizeof( SkinnedMeshVertex ) );
            }
            else
            {
                pVertexDescription = rResourceManager.GetStaticMeshVertexDescription( 1 );
                vertexStride = static_cast< uint32_t >( sizeof( StaticMeshVertex< 1 > ) );
            }

            pSceneObject->SetVertexData( pVertexBuffer, pVertexDescription, vertexStride );
            pSceneObject->SetIndexBuffer( pIndexBuffer );

            meshSectionCount = pMesh->GetSectionCount();
            if( meshSectionCount > subMeshCount )
            {
                meshSectionCount = subMeshCount;
            }

            uint32_t sectionVertexOffset = 0;
            uint32_t sectionIndexOffset = 0;
            for( size_t meshSectionIndex = 0; meshSectionIndex < meshSectionCount; ++meshSectionIndex )
            {
                GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
                    rSubMeshDataIds[ meshSectionIndex ] );
                HELIUM_ASSERT( pSubMeshData );

                uint32_t vertexCount = pMesh->GetSectionVertexCount( meshSectionIndex );
                uint32_t triangleCount = pMesh->GetSectionTriangleCount( meshSectionIndex );

                pSubMeshData->SetMaterial( pThis->GetMaterial( meshSectionIndex ) );
                pSubMeshData->SetPrimitiveType( RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST );
                pSubMeshData->SetPrimitiveCount( triangleCount );
                pSubMeshData->SetStartVertex( sectionVertexOffset );
                pSubMeshData->SetVertexRange( vertexCount );
                pSubMeshData->SetStartIndex( sectionIndexOffset );

                sectionVertexOffset += vertexCount;
                sectionIndexOffset += triangleCount * 3;
            }
        }

        for( size_t unusedSubMeshIndex = meshSectionCount; unusedSubMeshIndex < subMeshCount; ++unusedSubMeshIndex )
        {
            GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
                rSubMeshDataIds[ unusedSubMeshIndex ] );
            HELIUM_ASSERT( pSubMeshData );

            pSubMeshData->SetMaterial( NULL );
            pSubMeshData->SetPrimitiveType( RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST );
            pSubMeshData->SetPrimitiveCount( 0 );
            pSubMeshData->SetStartVertex( 0 );
            pSubMeshData->SetVertexRange( 0 );
            pSubMeshData->SetStartIndex( 0 );
        }
    }
}
