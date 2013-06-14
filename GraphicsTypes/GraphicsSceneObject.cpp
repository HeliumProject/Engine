#include "GraphicsTypesPch.h"
#include "GraphicsTypes/GraphicsSceneObject.h"

#include "Rendering/RIndexBuffer.h"
#include "Rendering/RVertexBuffer.h"
#include "Rendering/RVertexDescription.h"
#include "Graphics/Material.h"

using namespace Helium;

/// Constructor.
GraphicsSceneObject::GraphicsSceneObject()
#if HELIUM_USE_GRANNY_ANIMATION
: m_pBoneData( NULL )
#else
: m_pInverseReferencePose( NULL )
#endif
, m_pBonePalette( NULL )
, m_vertexStride( 0 )
, m_boneCount( 0 )
, m_updateMode( static_cast< uint8_t >( UPDATE_INVALID ) )
{
}

/// Set the instance transform matrix.
///
/// @param[in] rTransform  Transform matrix to set.
///
/// @see GetTransform()
void GraphicsSceneObject::SetTransform( const Simd::Matrix44& rTransform )
{
    m_transform = rTransform;
}

/// Set the world-space axis-aligned bounding box for this instance.
///
/// @param[in] rBox  World-space axis-aligned bounding box to set.
///
/// @see GetWorldBox(), GetWorldSphere()
void GraphicsSceneObject::SetWorldBounds( const Simd::AaBox& rBox )
{
    m_worldBox = rBox;
    m_worldSphere.Set( rBox );
}

/// Set the instance vertex information.
///
/// @param[in] pVertexBuffer       Vertex buffer to set.
/// @param[in] pVertexDescription  Vertex format description.
/// @param[in] vertexStride        Stride between each vertex in the specified buffer, in bytes.
///
/// @see GetVertexBuffer(), GetVertexDescription(), GetVertexStride()
void GraphicsSceneObject::SetVertexData(
                                        RVertexBuffer* pVertexBuffer,
                                        RVertexDescription* pVertexDescription,
                                        uint32_t vertexStride )
{
    m_spVertexBuffer = pVertexBuffer;
    m_spVertexDescription = pVertexDescription;
    m_vertexStride = vertexStride;
}

/// Set the index buffer used for rendering.
///
/// @param[in] pIndexBuffer  Sub-mesh index buffer.
///
/// @see GetIndexBuffer()
void GraphicsSceneObject::SetIndexBuffer( RIndexBuffer* pIndexBuffer )
{
    m_spIndexBuffer = pIndexBuffer;
}

#if HELIUM_USE_GRANNY_ANIMATION

/// Set the core bone data for skinned meshes.
///
/// @param[in] pBoneData  Granny bone information for the mesh.
/// @param[in] boneCount  Number of bones in the mesh.
///
/// @see GetBoneData(), GetBoneCount()
void GraphicsSceneObject::SetBoneData( const void* pBoneData, uint8_t boneCount )
{
    HELIUM_ASSERT( pBoneData || boneCount == 0 );

    m_pBoneData = pBoneData;
    m_boneCount = boneCount;
}

#else  // HELIUM_USE_GRANNY_ANIMATION

/// Set the core bone data for skinned meshes.
///
/// @param[in] pInverseReferencePose  Array of the inverse transform of each bone's reference pose.
/// @param[in] boneCount              Number of bones in the mesh.
///
/// @see GetInverseReferencePose(), GetBoneCount()
void GraphicsSceneObject::SetBoneData( const Simd::Matrix44* pInverseReferencePose, uint8_t boneCount )
{
    HELIUM_ASSERT( pInverseReferencePose || boneCount == 0 );

    m_pInverseReferencePose = pInverseReferencePose;
    m_boneCount = boneCount;
}

#endif  // HELIUM_USE_GRANNY_ANIMATION

/// Update the bone transform palette for skinned mesh rendering.
///
/// @param[in] pTransforms  Array of bone transforms.  Note that this should contain as many bones as specified in
///                         the most recent call to SetBoneData().
///
/// @see GetBonePalette()
void GraphicsSceneObject::SetBonePalette( const Simd::Matrix44* pTransforms )
{
    m_pBonePalette = pTransforms;
}

/// Flag this object as needing an update prior to the next scene update.
///
/// @param[in] updateMode  Identifier specifying the type of update needed.
///
/// @see GetNeedsUpdate(), GetUpdateMode(), Update()
void GraphicsSceneObject::SetNeedsUpdate( EUpdate updateMode )
{
    HELIUM_ASSERT( static_cast< size_t >( updateMode ) < static_cast< size_t >( UPDATE_MAX ) );

    if( m_updateMode != static_cast< uint8_t >( UPDATE_FULL ) )
    {
        m_updateMode = static_cast< uint8_t >( updateMode );
    }
}

/// Constructor.
///
/// @param[in] sceneObjectId  ID of the parent graphics scene object used to control the placement of this object as
///                           well as provide its vertex data.
GraphicsSceneObject::SubMeshData::SubMeshData( size_t sceneObjectId )
: m_sceneObjectId( sceneObjectId )
, m_pSkinningPaletteMap( NULL )
, m_primitiveType( RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST )
, m_primitiveCount( 0 )
, m_startVertex( 0 )
, m_vertexRange( 0 )
, m_startIndex( 0 )
{
    HELIUM_ASSERT( IsValid( sceneObjectId ) );
}

/// Set the material used for rendering.
///
/// @param[in] pMaterial  Sub-mesh material.
///
/// @see GetMaterial()
void GraphicsSceneObject::SubMeshData::SetMaterial( Material* pMaterial )
{
    m_spMaterial = pMaterial;
}

/// Set the map between mesh bone and GPU skinning palette indices to use for rendering.
///
/// @param[in] pMap  Array used to map mesh bone indices to skinning palette indices.
///
/// @see GetSkinningPaletteMap()
void GraphicsSceneObject::SubMeshData::SetSkinningPaletteMap( const uint8_t* pMap )
{
    m_pSkinningPaletteMap = pMap;
}

/// Set the primitive type to render.
///
/// @param[in] type  Primitive type.
///
/// @see GetPrimitiveType()
void GraphicsSceneObject::SubMeshData::SetPrimitiveType( ERendererPrimitiveType type )
{
    HELIUM_ASSERT( static_cast< size_t >( type ) < static_cast< size_t >( RENDERER_PRIMITIVE_TYPE_MAX ) );

    m_primitiveType = type;
}

/// Set the number of primitives to render.
///
/// @param[in] count  Primitive count.
///
/// @see GetPrimitiveCount()
void GraphicsSceneObject::SubMeshData::SetPrimitiveCount( uint32_t count )
{
    m_primitiveCount = count;
}

/// Set the offset of the first vertex to use within the vertex buffer.
///
/// @param[in] startVertex  Offset of the first vertex.
///
/// @see GetStartVertex()
void GraphicsSceneObject::SubMeshData::SetStartVertex( uint32_t startVertex )
{
    m_startVertex = startVertex;
}

/// Set the total range of vertices, starting from the start vertex, that are addressed when rendering this
/// sub-mesh.
///
/// @param[in] count  Number of vertices addressed.
///
/// @see GetVertexRange()
void GraphicsSceneObject::SubMeshData::SetVertexRange( uint32_t count )
{
    m_vertexRange = count;
}

/// Set the offset of the first index to use within the index buffer.
///
/// @param[in] startIndex  Offset of the first index.
///
/// @see GetStartIndex()
void GraphicsSceneObject::SubMeshData::SetStartIndex( uint32_t startIndex )
{
    m_startIndex = startIndex;
}
