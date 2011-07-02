//----------------------------------------------------------------------------------------------------------------------
// UpdateGraphicsSceneSubMeshBuffersJob.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

#include "Engine/JobManager.h"
#include "GraphicsTypes/VertexTypes.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannySceneObjectInterface.h"
#endif

using namespace Helium;

/// Update the instance buffer data for a set of graphics scene object sub-meshes.
///
/// @param[in] pContext  Context in which this job is running.
void UpdateGraphicsSceneSubMeshBuffersJob::Run( JobContext* /*pContext*/ )
{
    const GraphicsSceneObject* pSceneObjects = m_parameters.pSceneObjects;
    HELIUM_ASSERT( pSceneObjects );

    const GraphicsSceneObject::SubMeshData* pSubMeshes = m_parameters.pSubMeshes;
    HELIUM_ASSERT( pSubMeshes );

    float32_t* const* ppConstantBufferData = m_parameters.ppConstantBufferData;
    HELIUM_ASSERT( ppConstantBufferData );

    uint_fast32_t subMeshCount = m_parameters.subMeshCount;
    for( uint_fast32_t subMeshIndex = 0;
        subMeshIndex < subMeshCount;
        ++subMeshIndex, ++pSubMeshes, ++ppConstantBufferData )
    {
        float32_t* pConstantBuffer = *ppConstantBufferData;
        if( !pConstantBuffer )
        {
            continue;
        }

        const GraphicsSceneObject::SubMeshData& rSubMesh = *pSubMeshes;

        size_t sceneObjectIndex = rSubMesh.GetSceneObjectId();
        const GraphicsSceneObject& rSceneObject = pSceneObjects[ sceneObjectIndex ];

        const Simd::Matrix44* pBonePalette = rSceneObject.GetBonePalette();

#if HELIUM_USE_GRANNY_ANIMATION
        const void* pBoneData = rSceneObject.GetBoneData();
        HELIUM_ASSERT( pBoneData );

        Simd::Matrix44 inverseBoneReferencePose;
#else
        const Simd::Matrix44* pInverseReferencePose = rSceneObject.GetInverseReferencePose();
        HELIUM_ASSERT( pInverseReferencePose );
#endif

        const uint8_t* pSkinningPaletteMap = rSubMesh.GetSkinningPaletteMap();
        HELIUM_ASSERT( pSkinningPaletteMap );

        Simd::Matrix44 skinningMatrix;

        uint_fast8_t boneCount = rSceneObject.GetBoneCount();
        for( uint_fast8_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
        {
            size_t skinningPaletteIndex = pSkinningPaletteMap[ boneIndex ];
            if( skinningPaletteIndex >= BONE_COUNT_MAX )
            {
                continue;
            }

            float32_t* pSkinningMatrix43 = pConstantBuffer + skinningPaletteIndex * 12;

            const Simd::Matrix44& rBoneTransform = pBonePalette[ boneIndex ];
#if HELIUM_USE_GRANNY_ANIMATION
            Granny::GetInverseBoneReferencePose( inverseBoneReferencePose, pBoneData, boneIndex );
            skinningMatrix.MultiplySet( inverseBoneReferencePose, rBoneTransform );
#else
            const Simd::Matrix44& rInverseBoneReferencePose = pInverseReferencePose[ boneIndex ];
            skinningMatrix.MultiplySet( rInverseBoneReferencePose, rBoneTransform );
#endif

            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 0 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 4 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 8 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 12 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 1 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 5 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 9 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 13 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 2 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 6 );
            *( pSkinningMatrix43++ ) = skinningMatrix.GetElement( 10 );
            *pSkinningMatrix43       = skinningMatrix.GetElement( 14 );
        }
    }

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}
