//----------------------------------------------------------------------------------------------------------------------
// SkeletalMeshEntity.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/SkeletalMeshEntity.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannySkeletalMeshEntityInterface.cpp.inl"
#endif

namespace Lunar
{
    L_IMPLEMENT_OBJECT( SkeletalMeshEntity, Framework, 0 );

    /// Constructor.
    SkeletalMeshEntity::SkeletalMeshEntity()
    {
    }

    /// Destructor.
    SkeletalMeshEntity::~SkeletalMeshEntity()
    {
    }

    /// @copydoc Entity::Attach()
    void SkeletalMeshEntity::Attach()
    {
        Super::Attach();

#if L_USE_GRANNY_ANIMATION
        m_grannyData.Attach( this );
#endif
    }

    /// @copydoc Entity::Detach()
    void SkeletalMeshEntity::Detach()
    {
#if L_USE_GRANNY_ANIMATION
        m_grannyData.Detach( this );
#endif

        Super::Detach();
    }

    /// @copydoc Entity::SynchronousUpdate()
    void SkeletalMeshEntity::SynchronousUpdate( float32_t deltaSeconds )
    {
#if L_USE_GRANNY_ANIMATION
        m_grannyData.SynchronousUpdate( this, deltaSeconds );
#else
        HELIUM_UNREF( deltaSeconds );
#endif
    }

    /// @copydoc Object::Serialize()
    void SkeletalMeshEntity::Serialize( Serializer& s )
    {
        L_SERIALIZE_SUPER( s );

        s << L_TAGGED( m_spAnimation );
    }

    /// Assign an animation to this entity.
    ///
    /// @param[in] pAnimation  Animation to assign.
    ///
    /// @see GetAnimation()
    void SkeletalMeshEntity::SetAnimation( Animation* pAnimation )
    {
        VerifySafety();

        if( m_spAnimation != pAnimation )
        {
            DeactivateAssignedAnimation();

            m_spAnimation = pAnimation;
            ActivateAssignedAnimation();
        }
    }

    /// Activate any assigned animation for playback.
    ///
    /// @see DeactivateAssignedAnimation()
    void SkeletalMeshEntity::ActivateAssignedAnimation()
    {
        VerifySafety();

#if L_USE_GRANNY_ANIMATION
        m_grannyData.ActivateAssignedAnimation( this );
#endif
    }

    /// Deactivate any assigned animation and release allocated resources for playback.
    ///
    /// @see ActivateAssignedAnimation()
    void SkeletalMeshEntity::DeactivateAssignedAnimation()
    {
        VerifySafety();

#if L_USE_GRANNY_ANIMATION
        m_grannyData.DeactivateAssignedAnimation( this );
#endif
    }

    /// @copydoc MeshEntity::GetGraphicsSceneObjectUpdateCallback()
    GraphicsSceneObject::UPDATE_FUNC* SkeletalMeshEntity::GetGraphicsSceneObjectUpdateCallback() const
    {
        return GraphicsSceneObjectUpdate;
    }

    /// @copydoc MeshEntity::GraphicsSceneObjectUpdate()
    void SkeletalMeshEntity::GraphicsSceneObjectUpdate(
        void* pData,
        GraphicsScene* pScene,
        GraphicsSceneObject* pSceneObject )
    {
        HELIUM_ASSERT( pData );
        HELIUM_ASSERT( pScene );
        HELIUM_ASSERT( pSceneObject );

        Super::GraphicsSceneObjectUpdate( pData, pScene, pSceneObject );

        SkeletalMeshEntity* pThis = static_cast< SkeletalMeshEntity* >( pData );

        // If performing a full update, update the base bone information for the mesh.
        if( pSceneObject->GetUpdateMode() == GraphicsSceneObject::UPDATE_FULL )
        {
            const DynArray< size_t >& rSubMeshDataIds = pThis->m_graphicsSceneObjectSubMeshDataIds;
            size_t subMeshCount = rSubMeshDataIds.GetSize();
            size_t meshSectionCount = 0;

            Mesh* pMesh = pThis->m_spMesh;
            if( pMesh )
            {
#if L_USE_GRANNY_ANIMATION
                Granny::SkeletalMeshEntityData::SetSceneObjectBoneData( pMesh, pSceneObject );
#else
#pragma TODO( "Lunar::Mesh needs to be updated to store the inverse reference pose, which should be used here." )
                const Simd::Matrix44* pInverseReferencePose = pMesh->GetReferencePose();
                uint8_t boneCount = pMesh->GetBoneCount();

                pSceneObject->SetBoneData( pInverseReferencePose, boneCount );
#endif

                meshSectionCount = pMesh->GetSectionCount();
                if( meshSectionCount > subMeshCount )
                {
                    meshSectionCount = subMeshCount;
                }

                for( size_t meshSectionIndex = 0; meshSectionIndex < meshSectionCount; ++meshSectionIndex )
                {
                    GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
                        rSubMeshDataIds[ meshSectionIndex ] );
                    HELIUM_ASSERT( pSubMeshData );
                    pSubMeshData->SetSkinningPaletteMap( pMesh->GetSectionSkinningPaletteMap( meshSectionIndex ) );
                }
            }
            else
            {
                pSceneObject->SetBoneData( NULL, 0 );
            }

            for( size_t unusedSubMeshIndex = meshSectionCount; unusedSubMeshIndex < subMeshCount; ++unusedSubMeshIndex )
            {
                GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
                    rSubMeshDataIds[ unusedSubMeshIndex ] );
                HELIUM_ASSERT( pSubMeshData );
                pSubMeshData->SetSkinningPaletteMap( NULL );
            }
        }

#if L_USE_GRANNY_ANIMATION
        pThis->m_grannyData.SetSceneObjectBonePalette( pSceneObject );
#endif  // L_USE_GRANNY_ANIMATION
    }
}
