//----------------------------------------------------------------------------------------------------------------------
// Mesh.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/Mesh.h"

#include "Foundation/AsyncLoader.h"
#include "Platform/Math/Simd/Matrix44.h"
#include "Engine/CacheManager.h"
#include "Rendering/RIndexBuffer.h"
#include "Rendering/Renderer.h"
#include "Rendering/RVertexBuffer.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannyMeshInterface.cpp.inl"
#endif

using namespace Lunar;

L_IMPLEMENT_OBJECT( Mesh, Framework, GameObjectType::FLAG_NO_TEMPLATE );

/// Constructor.
Mesh::Mesh()
:
#if !L_USE_GRANNY_ANIMATION
m_pBoneNames( NULL )
, m_pParentBoneIndices( NULL )
, m_pReferencePose( NULL )
,
#endif
m_vertexCount( 0 )
, m_triangleCount( 0 )
, m_vertexBufferLoadId( Invalid< size_t >() )
, m_indexBufferLoadId( Invalid< size_t >() )
#if !L_USE_GRANNY_ANIMATION
, m_boneCount( 0 )
#endif
{
}

/// Destructor.
Mesh::~Mesh()
{
    HELIUM_ASSERT( !m_spVertexBuffer );
    HELIUM_ASSERT( !m_spIndexBuffer );
    HELIUM_ASSERT( IsInvalid( m_vertexBufferLoadId ) );
    HELIUM_ASSERT( IsInvalid( m_indexBufferLoadId ) );

#if !L_USE_GRANNY_ANIMATION
    delete [] m_pBoneNames;
    delete [] m_pParentBoneIndices;
    delete [] m_pReferencePose;
#endif
}

/// @copydoc GameObject::PreDestroy()
void Mesh::PreDestroy()
{
    HELIUM_ASSERT( IsInvalid( m_vertexBufferLoadId ) );
    HELIUM_ASSERT( IsInvalid( m_indexBufferLoadId ) );

    m_spVertexBuffer.Release();
    m_spIndexBuffer.Release();

    Base::PreDestroy();
}

/// @copydoc GameObject::Serialize()
void Mesh::Serialize( Serializer& s )
{
    L_SERIALIZE_SUPER( s );

    s << L_TAGGED_DYNARRAY( m_materials );
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool Mesh::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
bool Mesh::BeginPrecacheResourceData()
{
    HELIUM_ASSERT( IsInvalid( m_vertexBufferLoadId ) );
    HELIUM_ASSERT( IsInvalid( m_indexBufferLoadId ) );

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return true;
    }

    if( m_vertexCount != 0 )
    {
        size_t vertexDataSize = GetSubDataSize( 0 );
        if( IsInvalid( vertexDataSize ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to locate cached vertex buffer data for mesh " )
                TXT( "\"%s\".\n" ) ),
                *GetPath().ToString() );
        }
        else
        {
            m_spVertexBuffer = pRenderer->CreateVertexBuffer( vertexDataSize, RENDERER_BUFFER_USAGE_STATIC );
            if( !m_spVertexBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to create a vertex buffer of %" ) TPRIuSZ
                    TXT( " bytes for mesh \"%s\".\n" ) ),
                    vertexDataSize,
                    *GetPath().ToString() );
            }
            else
            {
                void* pData = m_spVertexBuffer->Map();
                if( !pData )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to map vertex buffer for loading for " )
                        TXT( "mesh \"%s\".\n" ) ),
                        *GetPath().ToString() );

                    m_spVertexBuffer.Release();
                }
                else
                {
                    m_vertexBufferLoadId = BeginLoadSubData( pData, 0 );
                    if( IsInvalid( m_vertexBufferLoadId ) )
                    {
                        HELIUM_TRACE(
                            TRACE_ERROR,
                            ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to queue async load request for " )
                            TXT( "vertex buffer data for mesh \"%s\".\n" ) ),
                            *GetPath().ToString() );

                        m_spVertexBuffer->Unmap();
                        m_spVertexBuffer.Release();
                    }
                }
            }
        }
    }

    if( m_triangleCount != 0 )
    {
        size_t indexDataSize = GetSubDataSize( 1 );
        if( IsInvalid( indexDataSize ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to locate cached index buffer data for mesh " )
                TXT( "\"%s\".\n" ) ),
                *GetPath().ToString() );
        }
        else
        {
            m_spIndexBuffer = pRenderer->CreateIndexBuffer(
                indexDataSize,
                RENDERER_BUFFER_USAGE_STATIC,
                RENDERER_INDEX_FORMAT_UINT16 );
            if( !m_spIndexBuffer )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to create an index buffer of %" ) TPRIuSZ
                    TXT( " bytes for mesh \"%s\".\n" ) ),
                    indexDataSize,
                    *GetPath().ToString() );
            }
            else
            {
                void* pData = m_spIndexBuffer->Map();
                if( !pData )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to map index buffer for loading for " )
                        TXT( "mesh \"%s\".\n" ) ),
                        *GetPath().ToString() );

                    m_spIndexBuffer.Release();
                }
                else
                {
                    m_indexBufferLoadId = BeginLoadSubData( pData, 1 );
                    if( IsInvalid( m_indexBufferLoadId ) )
                    {
                        HELIUM_TRACE(
                            TRACE_ERROR,
                            ( TXT( "Mesh::BeginPrecacheResourceData(): Failed to queue async load request for " )
                            TXT( "index buffer data for mesh \"%s\".\n" ) ),
                            *GetPath().ToString() );

                        m_spIndexBuffer->Unmap();
                        m_spIndexBuffer.Release();
                    }
                }
            }
        }
    }

    return true;
}

/// @copydoc GameObject::TryFinishPrecacheResourceData()
bool Mesh::TryFinishPrecacheResourceData()
{
    if( IsValid( m_vertexBufferLoadId ) )
    {
        if( !TryFinishLoadSubData( m_vertexBufferLoadId ) )
        {
            return false;
        }

        SetInvalid( m_vertexBufferLoadId );

        HELIUM_ASSERT( m_spVertexBuffer );
        m_spVertexBuffer->Unmap();
    }

    if( IsValid( m_indexBufferLoadId ) )
    {
        if( !TryFinishLoadSubData( m_indexBufferLoadId ) )
        {
            return false;
        }

        SetInvalid( m_indexBufferLoadId );

        HELIUM_ASSERT( m_spIndexBuffer );
        m_spIndexBuffer->Unmap();
    }

    return true;
}

/// @copydoc Resource::SerializePersistentResourceData()
void Mesh::SerializePersistentResourceData( Serializer& s )
{
    s << Serializer::WrapDynArray( m_sectionVertexCounts );
    s << Serializer::WrapDynArray( m_sectionTriangleCounts );
    s << Serializer::WrapDynArray( m_skinningPaletteMap );
    s << m_vertexCount;
    s << m_triangleCount;
    s << m_bounds;

#if L_USE_GRANNY_ANIMATION
    m_grannyData.SerializePersistentResourceData( s );
#else
    s << m_boneCount;
    uint_fast8_t boneCountFast = m_boneCount;

    if( s.GetMode() == Serializer::MODE_LOAD )
    {
        m_spVertexBuffer.Release();
        m_spIndexBuffer.Release();

        delete [] m_pBoneNames;
        delete [] m_pParentBoneIndices;
        delete [] m_pReferencePose;

        m_pBoneNames = NULL;
        m_pParentBoneIndices = NULL;
        m_pReferencePose = NULL;

        if( boneCountFast != 0 )
        {
            m_pBoneNames = new Name [ boneCountFast ];
            HELIUM_ASSERT( m_pBoneNames );
            m_pParentBoneIndices = new uint8_t [ boneCountFast ];
            HELIUM_ASSERT( m_pParentBoneIndices );
            m_pReferencePose = new Simd::Matrix44 [ boneCountFast ];
            HELIUM_ASSERT( m_pReferencePose );
        }
    }

    for( uint_fast8_t boneIndex = 0; boneIndex < boneCountFast; ++boneIndex )
    {
        s << m_pBoneNames[ boneIndex ];
        s << m_pParentBoneIndices[ boneIndex ];
        s << m_pReferencePose[ boneIndex ];
    }
#endif
}

/// @copydoc Resource::GetCacheName()
Name Mesh::GetCacheName() const
{
    static Name cacheName( TXT( "Mesh" ) );

    return cacheName;
}


/// Get the GPU skinning palette map for a specific mesh section.
///
/// The skinning palette map provides the indices within the bone palette passed to a shader for each bone defined
/// in the main mesh.  Invalid index values are used to signify bones that do not directly influence any bones in
/// the mesh section, and as such do not need to be provided to the GPU.
///
/// @param[in] sectionIndex  Mesh section index.
///
/// @return  Pointer to the GPU skinning palette map for the section associated with the specified index.
///
/// @see GetSectionVertexCount(), GetSectionTriangleCount(), GetSectionCount()
const uint8_t* Mesh::GetSectionSkinningPaletteMap( size_t sectionIndex ) const
{
    HELIUM_ASSERT( sectionIndex < m_sectionTriangleCounts.GetSize() );

#if L_USE_GRANNY_ANIMATION
    size_t boneCount = m_grannyData.GetBoneCount();
#else
    size_t boneCount = m_boneCount;
#endif

    return m_skinningPaletteMap.GetData() + sectionIndex * boneCount;
}
