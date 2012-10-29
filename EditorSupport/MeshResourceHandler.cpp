#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/MeshResourceHandler.h"

#include "Foundation/StringConverter.h"
#include "Math/SimdMatrix44.h"
#include "Math/SimdVectorConversion.h"
#include "Engine/BinarySerializer.h"
#include "GraphicsTypes/VertexTypes.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/FbxSupport.h"

HELIUM_IMPLEMENT_OBJECT( Helium::MeshResourceHandler, EditorSupport, 0 );

using namespace Helium;

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

    Mesh::PersistentResourceData persistentResourceData;

    // Load and parse the mesh data.
    DynArray< StaticMeshVertex< 1 > > vertices;
    DynArray< uint16_t > indices;
    //DynArray< uint16_t > sectionVertexCounts;
    //DynArray< uint32_t > sectionTriangleCounts;
    DynArray< FbxSupport::BoneData > bones;
    DynArray< FbxSupport::BlendData > vertexBlendData;
    //DynArray< uint8_t > skinningPaletteMap;

    bool bLoadSuccess = m_rFbxSupport.LoadMesh(
        rSourceFilePath,
        vertices,
        indices,
        persistentResourceData.m_sectionVertexCounts,
        persistentResourceData.m_sectionTriangleCounts,
        bones,
        vertexBlendData,
        persistentResourceData.m_skinningPaletteMap );
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
    persistentResourceData.m_vertexCount = static_cast< uint32_t >( vertexCountActual );

    size_t indexCount = indices.GetSize();
    size_t triangleCountActual = indexCount;
    HELIUM_ASSERT( triangleCountActual % 3 == 0 );
    triangleCountActual /= 3;
    HELIUM_ASSERT( triangleCountActual <= UINT32_MAX );
    persistentResourceData.m_triangleCount = static_cast< uint32_t >( triangleCountActual );

    size_t boneCountActual = bones.GetSize();
    HELIUM_ASSERT( boneCountActual <= UINT8_MAX );
    persistentResourceData.m_boneCount = static_cast< uint8_t >( boneCountActual );

    // Compute the mesh bounding box.
    //Simd::AaBox bounds;
    if( vertexCountActual != 0 )
    {
        const float32_t* pPosition = vertices[ 0 ].position;
        Simd::Vector3 position( pPosition[ 0 ], pPosition[ 1 ], pPosition[ 2 ] );
        persistentResourceData.m_bounds.Set( position, position );
        for( size_t vertexIndex = 1; vertexIndex < vertexCountActual; ++vertexIndex )
        {
            pPosition = vertices[ vertexIndex ].position;
            persistentResourceData.m_bounds.Expand( Simd::Vector3( pPosition[ 0 ], pPosition[ 1 ], pPosition[ 2 ] ) );
        }
    }
    
    persistentResourceData.m_pBoneNames.Resize(persistentResourceData.m_boneCount);
    persistentResourceData.m_pParentBoneIndices.Resize(persistentResourceData.m_boneCount);
    persistentResourceData.m_pReferencePose.Resize(persistentResourceData.m_boneCount);
    for( size_t boneIndex = 0; boneIndex < persistentResourceData.m_boneCount; ++boneIndex )
    {
        FbxSupport::BoneData& rBoneData = bones[ boneIndex ];            
        persistentResourceData.m_pBoneNames[boneIndex] = rBoneData.name;
        persistentResourceData.m_pParentBoneIndices[boneIndex] = rBoneData.parentIndex;
        persistentResourceData.m_pReferencePose[boneIndex] = rBoneData.referenceTransform;
    }
    
    // Cache the data for each supported platform.
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

        Cache::WriteCacheObjectToBuffer(persistentResourceData, rPreprocessedData.persistentDataBuffer);

        // Serialize the vertex buffer.  If the mesh is a skinned mesh, the vertices will need to be converted to
        // and serialized as an array of SkinnedMeshVertex structs.
        if( boneCountActual == 0 )
        {
            HELIUM_ASSERT(vertexCountActual == vertices.GetSize());
            size_t vertexDataSizeInBytes = vertexCountActual * sizeof(StaticMeshVertex< 1 >);
            rSubDataBuffers[0].Resize(vertexDataSizeInBytes);
            MemoryCopy(rSubDataBuffers[0].GetData(), vertices.GetData(), vertexDataSizeInBytes);
        }
        else
        {
            HELIUM_ASSERT( vertexBlendData.GetSize() == vertexCountActual );
            
            size_t vertexDataSizeInBytes = vertexCountActual * sizeof(SkinnedMeshVertex);
            rSubDataBuffers[0].Resize(vertexDataSizeInBytes);
            SkinnedMeshVertex *pVertices = reinterpret_cast<SkinnedMeshVertex *>(rSubDataBuffers[0].GetData());

            for( size_t vertexIndex = 0; vertexIndex < vertexCountActual; ++vertexIndex )
            {
                SkinnedMeshVertex &vertex = pVertices[vertexIndex];
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
            }
        }
        
        size_t indexDataSize = indexCount * sizeof(uint16_t);
        rSubDataBuffers[ 1 ].Resize(indexDataSize);
        MemoryCopy(rSubDataBuffers[1].GetData(), indices.GetData(), indexDataSize);

        // Platform data is now loaded.
        rPreprocessedData.bLoaded = true;
    }

    return true;
}

#endif  // HELIUM_TOOLS
