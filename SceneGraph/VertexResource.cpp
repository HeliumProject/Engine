#include "SceneGraphPch.h"
#include "VertexResource.h"

#include "Rendering/RVertexBuffer.h"

using namespace Helium;
using namespace Helium::SceneGraph;

VertexResource::VertexResource()
: Resource( ResourceTypes::Vertex )
, m_ElementType( VertexElementTypes::Unknown )
, m_LockedVerts( NULL )
{

}

VertexResource::~VertexResource()
{

}

uint32_t VertexResource::GetElementSize() const
{
    return VertexElementSizes[ m_ElementType ];
}

uint8_t* VertexResource::Lock() 
{
    void* data = m_Buffer->Map(
        IsDynamic() ? Helium::RENDERER_BUFFER_MAP_HINT_DISCARD : Helium::RENDERER_BUFFER_MAP_HINT_NONE );
    HELIUM_ASSERT( data );

    return static_cast< uint8_t* >( data );
}

void VertexResource::Unlock() 
{
    m_Buffer->Unmap();
}

bool VertexResource::Allocate() 
{
    uint32_t size = GetElementCount() * VertexElementSizes[ GetElementType() ];
    if ( size == 0 )
    {
        return false; 
    }

    Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    m_Buffer = pRenderer->CreateVertexBuffer(
        size,
        ( IsDynamic() ? Helium::RENDERER_BUFFER_USAGE_DYNAMIC : Helium::RENDERER_BUFFER_USAGE_STATIC ) );
    HELIUM_ASSERT( m_Buffer );

    return ( m_Buffer != NULL );
}

void VertexResource::Release() 
{
    if ( m_Buffer )
    {
        m_Buffer.Release();
    }
}
