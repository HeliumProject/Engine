//----------------------------------------------------------------------------------------------------------------------
// MeshResourceHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if L_EDITOR

#include "EditorSupport/MeshResourceHandler.h"

#include "Foundation/StringConverter.h"
#include "Platform/Math/Simd/Matrix44.h"
#include "Platform/Math/Simd/VectorConversion.h"
#include "Engine/BinarySerializer.h"
#include "GraphicsTypes/VertexTypes.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/FbxSupport.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannyMeshResourceHandlerInterface.h"
#endif

using namespace Helium;

L_IMPLEMENT_OBJECT( MeshResourceHandler, EditorSupport, 0 );

/// Constructor.
MeshResourceHandler::MeshResourceHandler()
: m_rFbxSupport( FbxSupport::StaticAcquire() )
{
}

/// Destructor.
MeshResourceHandler::~MeshResourceHandler()
{
    m_rFbxSupport.Release();
}

/// @copydoc ResourceHandler::GetResourceType()
const GameObjectType* MeshResourceHandler::GetResourceType() const
{
    return Mesh::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void MeshResourceHandler::GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const
{
    static const tchar_t* extensions[] = { TXT( ".fbx" ), TXT( ".obj" ) };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool MeshResourceHandler::CacheResource(
                                        ObjectPreprocessor* pObjectPreprocessor,
                                        Resource* pResource,
                                        const String& rSourceFilePath )
{
    HELIUM_ASSERT( pObjectPreprocessor );
    HELIUM_ASSERT( pResource );

    // Load and parse the mesh data.
    DynArray< StaticMeshVertex< 1 > > vertices;
    DynArray< uint16_t > indices;
    DynArray< uint16_t > sectionVertexCounts;
    DynArray< uint32_t > sectionTriangleCounts;
    DynArray< FbxSupport::BoneData > bones;
    DynArray< FbxSupport::BlendData > vertexBlendData;
    DynArray< uint8_t > skinningPaletteMap;
    bool bLoadSuccess = m_rFbxSupport.LoadMesh(
        rSourceFilePath,
        vertices,
        indices,
        sectionVertexCounts,
        sectionTriangleCounts,
        bones,
        vertexBlendData,
        skinningPaletteMap );
    if( !bLoadSuccess )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "MeshResourceHandler::CacheResource(): Failed to build mesh from source file \"%s\".\n" ),
            *rSourceFilePath );

        return false;
    }

    size_t vertexCountActual = vertices.GetSize();
    HELIUM_ASSERT( vertexCountActual <= UINT32_MAX );
    uint32_t vertexCount = static_cast< uint32_t >( vertexCountActual );

    size_t indexCount = indices.GetSize();
    size_t triangleCountActual = indexCount;
    HELIUM_ASSERT( triangleCountActual % 3 == 0 );
    triangleCountActual /= 3;
    HELIUM_ASSERT( triangleCountActual <= UINT32_MAX );
    uint32_t triangleCount = static_cast< uint32_t >( triangleCountActual );

    size_t boneCountActual = bones.GetSize();
    HELIUM_ASSERT( boneCountActual <= UINT8_MAX );
#if !L_USE_GRANNY_ANIMATION
    uint8_t boneCount = static_cast< uint8_t >( boneCountActual );
#endif

    // Compute the mesh bounding box.
    Simd::AaBox bounds;
    if( vertexCountActual != 0 )
    {
        const float32_t* pPosition = vertices[ 0 ].position;
        Simd::Vector3 position( pPosition[ 0 ], pPosition[ 1 ], pPosition[ 2 ] );
        bounds.Set( position, position );
        for( size_t vertexIndex = 1; vertexIndex < vertexCountActual; ++vertexIndex )
        {
            pPosition = vertices[ vertexIndex ].position;
            bounds.Expand( Simd::Vector3( pPosition[ 0 ], pPosition[ 1 ], pPosition[ 2 ] ) );
        }
    }

#if L_USE_GRANNY_ANIMATION
    Granny::MeshCachingData grannyMeshCachingData;
    grannyMeshCachingData.BuildResourceData( bones );
#endif  // L_USE_GRANNY_ANIMATION

    // Cache the data for each supported platform.
    BinarySerializer serializer;
    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        PlatformPreprocessor* pPreprocessor = pObjectPreprocessor->GetPlatformPreprocessor(
            static_cast< Cache::EPlatform >( platformIndex ) );
        if( !pPreprocessor )
        {
            continue;
        }

        Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );

        DynArray< DynArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;
        rSubDataBuffers.Reserve( 2 );
        rSubDataBuffers.Resize( 2 );
        rSubDataBuffers.Trim();

        serializer.SetByteSwapping( pPreprocessor->SwapBytes() );

        // Serialize the buffer sizes and mesh bounds first.
        serializer.BeginSerialize();
        serializer << Serializer::WrapDynArray( sectionVertexCounts );
        serializer << Serializer::WrapDynArray( sectionTriangleCounts );
        serializer << Serializer::WrapDynArray( skinningPaletteMap );
        serializer << vertexCount;
        serializer << triangleCount;
        serializer << bounds;

#if L_USE_GRANNY_ANIMATION
        grannyMeshCachingData.CachePlatformResourceData( pPreprocessor, serializer );
#else
        serializer << boneCount;

        for( size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
        {
            FbxSupport::BoneData& rBoneData = bones[ boneIndex ];
            serializer << rBoneData.name;
            serializer << rBoneData.parentIndex;
            serializer << rBoneData.referenceTransform;
        }
#endif

        serializer.EndSerialize();

        rPreprocessedData.persistentDataBuffer = serializer.GetPropertyStreamBuffer();

        // Serialize the vertex buffer.  If the mesh is a skinned mesh, the vertices will need to be converted to
        // and serialized as an array of SkinnedMeshVertex structs.
        serializer.BeginSerialize();

        if( boneCountActual == 0 )
        {
            for( size_t vertexIndex = 0; vertexIndex < vertexCountActual; ++vertexIndex )
            {
                vertices[ vertexIndex ].Serialize( serializer );
            }
        }
        else
        {
            HELIUM_ASSERT( vertexBlendData.GetSize() == vertexCountActual );

            SkinnedMeshVertex vertex;
            for( size_t vertexIndex = 0; vertexIndex < vertexCountActual; ++vertexIndex )
            {
                const StaticMeshVertex< 1 >& rStaticVertex = vertices[ vertexIndex ];
                const FbxSupport::BlendData& rBlendData = vertexBlendData[ vertexIndex ];

                MemoryCopy( vertex.position, rStaticVertex.position, sizeof( vertex.position ) );

                vertex.blendWeights[ 0 ] = static_cast< uint8_t >( Clamp(
                    rBlendData.weights[ 0 ] * 255.0f + 0.5f,
                    0.0f,
                    255.0f ) );
                vertex.blendWeights[ 1 ] = static_cast< uint8_t >( Clamp(
                    rBlendData.weights[ 1 ] * 255.0f + 0.5f,
                    0.0f,
                    255.0f ) );
                vertex.blendWeights[ 2 ] = static_cast< uint8_t >( Clamp(
                    rBlendData.weights[ 2 ] * 255.0f + 0.5f,
                    0.0f,
                    255.0f ) );
                vertex.blendWeights[ 3 ] = static_cast< uint8_t >( Clamp(
                    rBlendData.weights[ 3 ] * 255.0f + 0.5f,
                    0.0f,
                    255.0f ) );

                // Tweak the blend weights to ensure they still add up to 255 (1.0 when normalized by the GPU).
                size_t blendWeightTotal =
                    static_cast< size_t >( vertex.blendWeights[ 0 ] ) +
                    static_cast< size_t >( vertex.blendWeights[ 1 ] ) +
                    static_cast< size_t >( vertex.blendWeights[ 2 ] ) +
                    static_cast< size_t >( vertex.blendWeights[ 3 ] );
                if( blendWeightTotal != 0 && blendWeightTotal != 255 )
                {
                    if( blendWeightTotal > 255 )
                    {
                        // Total blend weight is too large, so decrease blend weights, starting from the lowest
                        // non-zero weight.
                        size_t weightAdjustIndex = 0;
                        do
                        {
                            do
                            {
                                weightAdjustIndex = ( weightAdjustIndex + 3 ) % 4;
                            } while( vertex.blendWeights[ weightAdjustIndex ] == 0 );

                            --vertex.blendWeights[ weightAdjustIndex ];
                            --blendWeightTotal;
                        } while( blendWeightTotal > 255 );
                    }
                    else
                    {
                        // Total blend weight is too small, so increase blend weights, starting from the highest
                        // non-zero blend weight.  Note that we should not have to check whether the blend weight is
                        // already at its max, as that would mean our total blend weight would have to already be at
                        // least 255.
                        size_t weightAdjustIndex = 3;
                        do
                        {
                            do
                            {
                                weightAdjustIndex = ( weightAdjustIndex + 1 ) % 4;
                            } while( vertex.blendWeights[ weightAdjustIndex ] == 0 );

                            HELIUM_ASSERT( vertex.blendWeights[ weightAdjustIndex ] != 255 );

                            ++vertex.blendWeights[ weightAdjustIndex ];
                            ++blendWeightTotal;
                        } while( blendWeightTotal < 255 );
                    }

                    HELIUM_ASSERT( blendWeightTotal == 255 );
                }

                MemoryCopy( vertex.blendIndices, rBlendData.indices, sizeof( vertex.blendIndices ) );

                MemoryCopy( vertex.normal, rStaticVertex.normal, sizeof( vertex.normal ) );
                MemoryCopy( vertex.tangent, rStaticVertex.tangent, sizeof( vertex.tangent ) );
                MemoryCopy( vertex.texCoords, rStaticVertex.texCoords[ 0 ], sizeof( vertex.texCoords ) );

                vertex.Serialize( serializer );
            }
        }

        serializer.EndSerialize();

        rSubDataBuffers[ 0 ] = serializer.GetPropertyStreamBuffer();

        // Serialize the index buffer.
        serializer.BeginSerialize();
        for( size_t indexIndex = 0; indexIndex < indexCount; ++indexIndex )
        {
            serializer << indices[ indexIndex ];
        }

        serializer.EndSerialize();

        rSubDataBuffers[ 1 ] = serializer.GetPropertyStreamBuffer();

        // Platform data is now loaded.
        rPreprocessedData.bLoaded = true;
    }

    return true;
}

#endif  // L_EDITOR
